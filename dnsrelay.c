#include"dnsrelay.h"
int main(int argc, char* argv[])
{
    get_debug(argc, argv);
    Trienode* root;
    Cachenode* head;
    head = (Cachenode*)malloc(sizeof(Cachenode));
    head->pre = NULL;
    head->empire_time = -1;
    for (int i = 0; i <= 255; i++)
    {
        head->name[i] = '\0';
    }
    head->next = NULL;
    root = (Trienode*)malloc(sizeof(Trienode));
    for (int i = 0; i <= 74; i++)
    {
        root->next[i] = NULL;
    }
    FILE* fp;
    int flag, flag1;
    if (fp = fopen("dnsrelay.txt", "r"))
    {
        char buf[256] = { '\0' };
        while (fgets(buf, 256, fp) != NULL)
        {
            char t_ip[15] ;
            char name[256] = { '\0' };
            sscanf(buf, "%s %s", t_ip, name);
            uint32_t ip = inet_addr(t_ip);
            insert(root, name, ip);
        }
    }
    init_socket();
    while (1)
    {
        uint8_t buf[BUFSIZE];
        HEADER* recv_package;
        memset(buf, 0, BUFSIZE);
        int rec = recvfrom(Sock, buf, sizeof(buf), 0,(struct sockaddr*) &inaddr, &addr_len);
        recv_package = (HEADER*)buf;
        if (rec > 0)
        {
            time_t t;
            time(&t);
            struct tm* p = localtime(&t);
            if (debug_level == 1)
            {
                printf("%d-%d-%d  ", (p->tm_year + 1900), (p->tm_mon + 1), p->tm_mday);
                printf("%02d:%02d:%02d  ", p->tm_hour, p->tm_min, p->tm_sec);
                printf(" %15s : %-5d   ", inet_ntoa(inaddr.sin_addr), ntohs(inaddr.sin_port));
            }
            if (recv_package->qr == 0)//从客户端接收
            {
                if (debug_level == 2)
                {
                    printf("--------------------从客户端接收的包--------------------\n");
                }
                if (debug_level == 2)
                {
                    printf("报头id：%u    ", ntohs(recv_package->id));
                    printf("源ip地址：%s\n", inet_ntoa(inaddr.sin_addr));
                }
                if (recv_package->opcode == 0 && ntohs(recv_package->qdcount) == 1)
                {
                    Question question;
                    uint32_t ip, ip1, ttl;
                    uint8_t* offset = getquestion(&question, buf + 12);//前面的报文头部占12个字节
                    ip1 = cache_search(head, (question.qname), &flag1, &ttl);
                    if (flag1 == 0)
                    {
                        ip = search(root, (question.qname), &flag);
                        if (flag == 1&&question.qtype==A_TYPE)
                        {
                            recv_package->qr = 1;
                            if (ip == 0)
                            {
                                recv_package->rcode = 3;
                            }
                            else
                            {
                                ttl = 86400;
                                recv_package->rcode = 0;
                                recv_package->ancount = htons(1);
                                ResourceRecord answer;
                                answer.name = htons(A_NAME_OFFSET);
                                answer.type = htons(A_TYPE);
                                answer.class = htons(IN_CLASS);
                                *(uint32_t*)&answer.ttl = htonl(ttl);
                                answer.rd_length = htons(A_RDLENGTH);
                                answer.rdata = ip;
                                memcpy(offset, (uint8_t*)(&answer), 16);
                                rec = rec + 16;//添加之后的报文长度；
                            }
                            sendto(Sock, buf, rec, 0, (struct sockaddr*)&inaddr, addr_len);
                            if (debug_level == 1)
                            {
                                printf("Hit local record\n");
                            }
                            else if (debug_level == 2)
                            {
                                printf("已查询到该地址\n");
                            }
                            cache_insert(head, question.qname, ip, 3600);
                        }
                        else
                        {
                            uint16_t serverid;
                            serverid = applyid(ntohl(inaddr.sin_addr.S_un.S_addr), ntohs(inaddr.sin_port), ntohs(recv_package->id));
                            if (debug_level == 1)
                            {
                                printf("%5u  ->  %5u\n", ntohs(recv_package->id), serverid);
                            }
                            else if (debug_level == 2)
                            {
                                printf("新的报头id：%u\n", serverid);
                            }
                            recv_package->id = htons(serverid);
                            int sec = sendto(Sock, buf, rec, 0, (struct sockaddr*)&server, addr_len);
                            if (sec == -1)
                            {
                                deleteid(serverid);
                            }
                        }
                    }
                   else
                    {
                        recv_package->qr = 1;
                        if (ip1 == 0)
                        {
                            recv_package->rcode = 3;
                        }
                        else
                        {
                            recv_package->rcode = 0;
                            recv_package->ancount = htons(1);
                            ResourceRecord answer;
                            answer.name = htons(A_NAME_OFFSET);
                            answer.type = htons(A_TYPE);
                            answer.class = htons(IN_CLASS);
                            *(uint32_t*)&answer.ttl = htonl(ttl);
                            answer.rd_length = htons(A_RDLENGTH);
                            answer.rdata = ip1;
                            memcpy(offset, (uint8_t*)(&answer), 16);
                            rec = rec + 16;//添加之后的报文长度；
                        }
                        sendto(Sock, buf, rec, 0, (struct sockaddr*)&inaddr, addr_len);
                        if (debug_level == 1)
                        {
                            printf("Hit cache record\n");
                        }
                    } 
                }
            }
            else//从服务器接收
            {
                if (recv_package->opcode == 0 && ntohs(recv_package->qdcount) == 1)
                {
                    Question question;
                    uint8_t* offset = getquestion(&question, buf + 12);
                    ResourceRecord* answer;
                    answer = (ResourceRecord*)offset;
                    if (ntohs(answer->name) == A_NAME_OFFSET)
                    {
                        if (ntohs(answer->type) ==A_TYPE)
                        {
                            cache_insert(head, question.qname, ntohl(answer->rdata), ntohl(*(uint32_t*)&answer->ttl));
                        }
                    }
                }
                uint16_t serverid;
                serverid = ntohs(recv_package->id);
                struct sockaddr_in cliaddr;
                memset(&cliaddr, 0, sizeof cliaddr);
                cliaddr.sin_family = AF_INET;
                cliaddr.sin_port = htons(idspool[serverid].clientPort);
                cliaddr.sin_addr.S_un.S_addr = htonl(idspool[serverid].clientIP);
                if (debug_level == 1)
                {
                    printf("%5u  <-  %5u\n", idspool[serverid].clientID,serverid);
                }
                else if (debug_level == 2)
                {
                    printf("--------------------从服务器接收的包--------------------\n");
                    printf("报头id：%u    原报头id：%u \n", ntohs(recv_package->id), idspool[serverid].clientID);
                    printf("源ip地址：%s\n", inet_ntoa(inaddr.sin_addr));
                }
                recv_package->id = htons(idspool[serverid].clientID);
                idspool[serverid].flag = 0;
                sendto(Sock, buf, rec, 0, (struct sockaddr*)&cliaddr, addr_len);
            }
        }
    }
    closesocket(Sock);
    WSACleanup();
}
void get_debug(int argc, char* argv[]) 
{
    if (argc > 1 && argv[1][0] == '-') 
    {
        if (argv[1][1] == 'd') 
        {
            debug_level++;
        }
        if (argv[1][2] == 'd') 
        {
            debug_level++;
        }
        if (argc > 2) 
        {
            strcpy(DNS_IP, argv[2]);
            printf("用户指定dns服务器IP为 %s\n", DNS_IP);
        }
    }
    printf("调试模式为 %d\n", debug_level);
}
void init_socket()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&inaddr, 0, sizeof(inaddr));
    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    inaddr.sin_port = htons(PORT);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.S_un.S_addr = inet_addr(DNS_IP);
    server.sin_port = htons(PORT);
    
    bind(Sock, (struct sockaddr*)&inaddr, addr_len);
}
uint16_t get16bit(uint8_t** buf)
{
    uint16_t rec;
    memcpy(&rec, *buf, 2);
    *buf = *buf + 2;
    return ntohs(rec);
}
char* getquestion(Question* question, char buf[])
{
    int i = 0;
    while (*buf != 0)
    {
        question->qname[i] = *buf;
        buf++;
        i++;
    }
    question->qname[i] = 0;
    buf++;
    question->qtype = get16bit(&buf);
    question->qclass = get16bit(&buf);
    return buf;
}

#include"ID.h"
uint16_t applyid(uint32_t clientIP, uint16_t clientPort, uint16_t clientID)
{
	uint16_t oldid = Nid;
	while (idspool[Nid].flag == 1 && time(NULL) - idspool[Nid].inTime < 2)
	{
		Nid++;
		if (Nid == oldid)
		{
			printf("idspool overflow");
			exit(-1);
		}
	}
	idspool[Nid].clientID = clientID;
	idspool[Nid].clientIP = clientIP;
	idspool[Nid].clientPort = clientPort;
	idspool[Nid].flag = 1;
	idspool[Nid].inTime = time(NULL);
	return Nid++;//先取值后自加
}
void deleteid(uint16_t serverid)
{
	idspool[serverid].flag = 0;
}
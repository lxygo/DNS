#include"table.h"
void insert(Trienode* root, char s[],uint32_t ip)
{
	int i = 0;
	int order;
	Trienode* node;
	node = root;
	while (s[i] != '\0')
	{
		if (s[i] >= '0' && s[i] <= 'z')
		{
			order = s[i] - '0';
			if (node->next[order] == NULL)
			{
				node->next[order] = (Trienode*)malloc(sizeof(Trienode));
				for (int i = 0; i <= MAX - 1; i++)
				{
					node->next[order]->next[i] = NULL;
				}
				node->next[order]->flag = 0;
				node->next[order]->ip = 0;
			}
			if (s[i + 1] != '\0')
			{
				node = node->next[order];
			}
			else
			{
				node->next[order]->flag = 1;
				node->next[order]->ip = ip;
			}
		}
		i++;
	}
}
uint32_t search(Trienode* root, char s[],int*flag)
{
	int i = 0;
	int order;
	Trienode* node;
	node = root;
	while (s[i] != 0)
	{
		if (s[i] >= '0' && s[i] <= 'z')
		{
			order = s[i] - '0';
			if (node->next[order] == NULL)
			{
				(*flag) = 0;
				return 0;
			}
			if (s[i + 1] == '\0' && node->next[order]->flag == 1)
			{
				(*flag) = 1;
				return node->next[order]->ip;
			}
			node = node->next[order];
		}
		i++;
	}
}
void cache_insert(Cachenode* head, char name[], uint32_t ip, uint32_t ttl)
{
	Cachenode* node;
	time_t now_time;
	int lenth;
	if (Cachenow <= Cachesize)
	{
		node = (Cachenode*)malloc(sizeof(Cachenode));
		node->ip = ip;
		lenth = strlen(name);
		for (int i = 0; i <= lenth - 1; i++)
		{
			node->name[i] = name[i];
		}
		if (head->next)
		{
			head->next->pre = node;
		}
		node->pre = head;
		node->next = head->next;
		head->next = node;
		node->empire_time = time(NULL) + ttl;
		Cachenow++;
	}
	else
	{
		node = head;
		while (node != NULL)
		{
			now_time = time(NULL);
			if (node->empire_time < now_time)
			{
				cache_delete(head, node->name);
				Cachenow--;
			}
		}
	}
}
void cache_delete(Cachenode* head, char name[])
{
	Cachenode* node, *t_node;
	node = head;
	while (node->next != NULL)
	{
		if (strcmp(node->name, name) == 0)
		{
			t_node = node;
			node->pre->next = node->next;
			node->next->pre = t_node->pre;
			free(node);
		}
	}
}
uint32_t cache_search(Cachenode* head, char name[256], int* flag, uint32_t* ttl)
{
	Cachenode* node;
	time_t now_time;
	node = head;
	int i = 0;
	while (node!= NULL)
	{
		now_time = time(NULL);
		if (node->empire_time < now_time && node->empire_time != -1)
		{
			cache_delete(head, node->name);
			Cachenow--;
		}
		if (strcmp(node->name, name) == 0)
		{
			*flag = 1;
			*ttl = node->empire_time - now_time;
			return node->ip;
		}
		node = node->next;
	}
	*flag = 0;
	return 0;
}
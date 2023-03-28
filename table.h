#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>
#define MAX 75
#define Cachesize 256
int Cachenow;
typedef struct Trienode {
	struct Trienode* next[MAX];
	int flag;
	uint32_t ip;
}Trienode;
typedef struct Cachenode {
	struct Cachenode* pre;
	char name[256];
	uint32_t ip;
	time_t empire_time;
	struct Cachenode* next;
}Cachenode;
void cache_insert(Cachenode* head, char name[], uint32_t ip, uint32_t ttl);
void cache_delete(Cachenode* head, char name[]);
uint32_t cache_search(Cachenode* head, char name[256], int* flag, uint32_t* ttl);
void insert(Trienode* root, char s[],uint32_t ip);
uint32_t search(Trienode* root, char s[], int* flag);

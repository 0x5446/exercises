#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GRAPH_V_NODE_NUM 256
#define MAX_NAME_SIZE 32 
#define HASH_TABLE_SIZE 512

typedef struct e_node_s {
	int idx;
	struct e_node_s *next;
} e_node_t;

typedef struct v_node_s {
	char name[MAX_NAME_SIZE];
	e_node_t *first_e_node;
	e_node_t *last_e_node;
} v_node_t;

typedef struct graph_s {
	v_node_t *v_node_list[MAX_GRAPH_V_NODE_NUM];
	int v_node_num;
	int e_node_num;
} graph_t;

typedef struct htable_node_s {
	unsigned int hash;
	char key[MAX_NAME_SIZE];
	int val;
	struct htable_node_s *next;
} htable_node_t;

typedef struct htable_s {
	htable_node_t *table[HASH_TABLE_SIZE];
	int size;
} htable_t;

unsigned int BKDRHash(char *str);	//BKDRHash
htable_node_t *htable_node_init(unsigned int hash, char *key, int val);
htable_t *htable_init();
htable_node_t *htable_get(char *key, htable_t *htable);
int htable_set(char *key, int val, htable_t *htable);

e_node_t *e_node_init(int idx);
v_node_t *v_node_init(char *name);
graph_t *graph_init();
int graph_add_e_node(e_node_t *e, int idx, graph_t *g);
int graph_add_v_node(v_node_t *v, graph_t *g);

e_node_t *e_node_init(int idx) {
	e_node_t *e = malloc(sizeof(e_node_t));
	e->idx = idx;
	e->next = NULL;
	return e;
}

v_node_t *v_node_init(char *name) {
	v_node_t *v = malloc(sizeof(v_node_t));
	snprintf(v->name, MAX_NAME_SIZE, "%s", name);
	v->first_e_node = NULL;
	v->last_e_node = NULL;
	return v;
}

graph_t *graph_init() {
	graph_t *g = malloc(sizeof(graph_t));
	g->v_node_num = 0;
	g->e_node_num = 1;
	return g;
}

int graph_add_v_node(v_node_t *v, graph_t *g) {
	g->v_node_list[g->v_node_num] = v;
	g->v_node_num ++;
	return g->v_node_num - 1;
}

int graph_add_e_node(e_node_t *e, int idx, graph_t *g) {
	v_node_t *v = g->v_node_list[idx];
	if (v == NULL) {
		perror("vex is NULL!");
		return 1;
	}
	if (v->first_e_node == NULL) {
		v->first_e_node = e;
		v->last_e_node = e;
	} else {
		v->last_e_node->next = e;
		v->last_e_node = e;
	}
	g->e_node_num ++;
	return 0;
}


htable_node_t *htable_node_init(unsigned int hash, char *key, int val) {
	htable_node_t *node = malloc(sizeof(htable_node_t));
	node->hash = hash;
	snprintf(node->key, MAX_NAME_SIZE, "%s", key);
	node->val = val;
	node->next = NULL;
	return node;
}

htable_t *htable_init() {
	htable_t *h = malloc(sizeof(htable_t));
	h->size = 0;
	return h;
}

int htable_set(char *key, int val, htable_t *htable) {
	unsigned int hash = BKDRHash(key);
	int idx = hash % HASH_TABLE_SIZE;
	htable_node_t *prev;
	htable_node_t *node = htable->table[idx];
	if (node == NULL) {
		htable->table[idx] = htable_node_init(hash, key, val);
		htable->size ++;
	} else {
 		while (strcmp(node->key, key) != 0) {
			if (node->next != NULL) {
				node = node->next;
			} else {// scan to the end
				node->next = htable_node_init(hash, key, val);
				htable->size ++;
				break;
			}
		}
	}
	return 0;
}

htable_node_t *htable_get(char *key, htable_t *htable) {
	unsigned int hash = BKDRHash(key);
	int idx = hash % HASH_TABLE_SIZE;
	htable_node_t *node = htable->table[idx];
	while (node != NULL && strcmp(node->key, key) != 0) {
		node = node->next;
	}
	return node;
}


int main() {

	char *name = malloc(sizeof(char) * MAX_NAME_SIZE);
	char *jname = malloc(sizeof(char) * MAX_NAME_SIZE);
	char line[1024];
	int i, j, cnt, jcnt, idx;
	graph_t *g = graph_init();
	htable_t *ht = htable_init();

	printf("输入图的顶点数量：\n"); 
	scanf("%d", &cnt);
	for (i = 0; i < cnt; i ++) {//cnt 个顶点
		printf("输入第%d个顶点名字：\n", i);
		scanf("%s", name);
		if (0 == graph_add_v_node(v_node_init(name), g)) {
			printf("第%d个顶点%s已生成", i, name);
		}
		printf("输入顶点#%d:%s的邻接点数量：\n", i, name); 
		scanf("%d", &jcnt);
		for (j = 0; j < jcnt; j ++) {//jcnt个邻接点
			printf("输入#%d:%s的第%d个邻接点名字：\n", i, name, j);
			scanf("%s", jname);
			htable_node_t *node = htable_get(jname, ht);
			if (node == NULL) {
				idx = graph_add_v_node(v_node_init(jname), g);
				htable_set(jname, idx, ht);
			} else {
				idx = node->val;
			}
			if (0 == graph_add_e_node(e_node_init(idx), i, g)) {
				printf("%d:%s的第%d个邻接点%s已生成\n", i, name, j, jname);
			} else {	//incredable!
				exit(1);
			}
		}
	}

	printf("图生成完毕！\n");

	return 0;
}

// BKDR Hash Function
unsigned int BKDRHash(char *str) {
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

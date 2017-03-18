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
	int known;
	int dist;
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
	int count;
} htable_t;

typedef struct ele_s {
	int val;
	struct ele_s *next;
} ele_t;

typedef struct queue_s {
	ele_t *head;
	ele_t *tail;
	int count;
} queue_t;

ele_t *ele_init(int val);
queue_t *queue_init();
ele_t *dequeue(queue_t *q);
int enqueue(ele_t *e, queue_t *q);

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
v_node_t *graph_get_v_node(int idx, graph_t *g);
int graph_set_v_node(v_node_t *v, int idx, graph_t *g);

int graph_get_shortest_path(char *vname, htable_t *htable, graph_t *g);

int graph_print(graph_t *g);

e_node_t *e_node_init(int idx) {
	e_node_t *e = malloc(sizeof(e_node_t));
	e->idx = idx;
	e->next = NULL;
	return e;
}

v_node_t *v_node_init(char *name) {
	v_node_t *v = malloc(sizeof(v_node_t));
	snprintf(v->name, MAX_NAME_SIZE, "%s", name);
	v->known = 0;
	v->dist = 0;
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

v_node_t *graph_get_v_node(int idx, graph_t *g) {
	return g->v_node_list[idx];
}

int graph_set_v_node(v_node_t *v, int idx, graph_t *g) {
	g->v_node_list[idx] = v;
	return 0;
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
	h->count = 0;
	return h;
}

int htable_set(char *key, int val, htable_t *htable) {
	unsigned int hash = BKDRHash(key);
	int idx = hash % HASH_TABLE_SIZE;
	htable_node_t *prev;
	htable_node_t *node = htable->table[idx];
	if (node == NULL) {
		htable->table[idx] = htable_node_init(hash, key, val);
		htable->count ++;
	} else {
 		while (strcmp(node->key, key) != 0) {
			if (node->next != NULL) {
				node = node->next;
			} else {// scan to the end
				node->next = htable_node_init(hash, key, val);
				htable->count ++;
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

ele_t *ele_init(int val) {
	ele_t *e = malloc(sizeof(ele_t));
	e->val = val;
	e->next = NULL;
	return e;
}

queue_t *queue_init() {
	queue_t *q = malloc(sizeof(queue_t));
	q->head = q->tail = NULL;
	q->count = 0;
	return q;
}

int enqueue(ele_t *e, queue_t *q) {
	if (q->tail == NULL) {
		q->head = e;
		q->tail = e;
		q->count = 1;
	} else {
		q->tail->next = e;
		q->tail = e;
		q->count ++;
	}
	return 0;
}

ele_t *dequeue(queue_t *q) {
	if (q->count <= 0) {
		return NULL;
	}
	ele_t *e = q->head;
	q->head = q->head->next;
	q->count --;
	if (q->count == 0) {
		q->tail = NULL;
	}
	return e;
}

int graph_print(graph_t *g) {
	int j, i = 0;
	v_node_t *v;
	while (NULL != (v = g->v_node_list[i])) {
		e_node_t *e = v->first_e_node;
		j = 0;
		while (e != NULL) {
			printf("顶点[%d]%s的第%d个邻接点：[%d]%s\n", i+1, v->name,  j+1, e->idx+1, g->v_node_list[e->idx]->name);
			e = e->next;
			j ++;
		}
		i ++;
	}
	return 0;
}

int graph_get_shortest_path(char *vname, htable_t *htable, graph_t *g) {

	htable_node_t *node = htable_get(vname, htable);
	int idx = node->val;

	queue_t *q = queue_init();
	g->v_node_list[idx]->known = 1;
	g->v_node_list[idx]->dist = 0;
	enqueue(ele_init(idx), q);

	ele_t *ele;

	while (NULL != (ele = dequeue(q))) {
		v_node_t *v = g->v_node_list[ele->val];
		e_node_t *e = v->first_e_node;
		while (e != NULL) {
			if (g->v_node_list[e->idx]->known == 0) {
				g->v_node_list[e->idx]->known = 1;
				g->v_node_list[e->idx]->dist = v->dist + 1;
				enqueue(ele_init(e->idx), q);
				printf("点[%d]%s距离点[%d]%s：%d\n", e->idx, g->v_node_list[e->idx]->name, idx, vname, g->v_node_list[e->idx]->dist);
			}
			e = e->next;
		}
	}
	return 0;
}

int main() {

	char *name = malloc(sizeof(char) * MAX_NAME_SIZE);
	char *jname = malloc(sizeof(char) * MAX_NAME_SIZE);
	char line[1024];
	int i, j, cnt, jcnt, idx, eidx;
	graph_t *g = graph_init();
	htable_t *ht = htable_init();

	printf("输入图的顶点数量：\n"); 
	scanf("%d", &cnt);
	for (i = 0; i < cnt; i ++) {//cnt 个顶点
		printf("输入第%d个顶点名字：\n", i+1);
		scanf("%s", name);
		htable_node_t *node = htable_get(name, ht);
		if (node == NULL) {
			idx = graph_add_v_node(v_node_init(name), g);
			htable_set(name, idx, ht);
			printf("第%d个顶点[%d]%s已生成\n", i+1, idx, name);
		} else {
			idx = node->val;
			printf("第%d个顶点%s已存在,槽位：%d\n", i+1, name, idx);
		}
		printf("输入第%d个顶点[%d]%s的邻接点数量：\n", i+1, idx, name); 
		scanf("%d", &jcnt);
		for (j = 0; j < jcnt; j ++) {//jcnt个邻接点
			printf("输入第%d个顶点[%d]%s的第%d个邻接点名字：\n", i+1, idx, name, j+1);
			scanf("%s", jname);
			htable_node_t *node = htable_get(jname, ht);
			if (node == NULL) {
				eidx = graph_add_v_node(v_node_init(jname), g);
				htable_set(jname, eidx, ht);
			} else {
				eidx = node->val;
			}
			if (0 == graph_add_e_node(e_node_init(eidx), idx, g)) {
				printf("第%d个顶点[%d]%s的第%d个邻接点%s已生成\n", i+1, idx, name, j+1, jname);
			} else {	//incredable!
				exit(1);
			}
		}
	}

	printf("图生成完毕！\n");

	printf("开始打印图——\n");
	graph_print(g);

	printf("准备计算最短路径，请输入起始点：\n");
	char vname[256];
	scanf("%s", vname);
	printf("开始计算最短路径——\n");
	graph_get_shortest_path(vname, ht, g);

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

//set nu ts=4 sw=4 cindent

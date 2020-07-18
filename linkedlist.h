/*
 * linkedlist.h
 */

#ifndef __LINKEDKLIST_H__
#define __LINKEDKLIST_H__

#include <stdint.h>

struct Node_ {
    uint64_t d;
    struct Node_ *n;
    struct Node_ *p;
};
typedef struct Node_ Node;

typedef struct {
    Node *head;
    Node *tail;
    uint32_t cnt;

    uint32_t mallocCnt;
    uint32_t freeCnt;
} LL;


LL* llNew(void);
int llDelete(LL *ll);
int llStats(LL *ll, char *msg);
int llDump(LL *ll);
int llInsertAt(LL *ll, uint64_t val, uint32_t ind);
int llAppend(LL *ll, uint64_t val);
int llPop(LL *ll, uint64_t *val);
int llPush(LL *ll, uint64_t val);
int llDequeue(LL *ll, uint64_t *val);
int llEnqueue(LL *ll, uint64_t val);
int lltest_main(int argc, char* argv[]);

#endif /* __LINKEDKLIST_H__ */

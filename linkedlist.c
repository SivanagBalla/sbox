/**
 * linkedlist.c 
 * Written by Sivanag Balla
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "util.h"
#include "linkedlist.h"

LL* llNew(void) {
    LL *ll;
    
    ll = (LL*)calloc(1, sizeof(LL));
    if (!ll) {
        return NULL;
    }
    return ll;
}

int llDelete(LL *ll) {
    uint64_t d;

    if (!ll) {
        return FALSE;
    }

    while (ll->cnt) {
        llPop(ll, &d);
    }

    free(ll);
    return TRUE;
}

int llStats(LL *ll, char *msg) {
    if (!ll) {
        return FALSE;
    }

    if (msg) {
        printf("%s\n", msg);
    } else {
        printf("Stats: \n");
    }

    printf("  root      : %-16p  cnt       : %d\n", ll, ll->cnt);
    printf("  head      : %-16p  mallocCnt : %d\n", ll->head, ll->mallocCnt);
    printf("  tail      : %-16p  freeCnt   : %d\n", ll->tail, ll->freeCnt);
    return FALSE;
}

int llDump(LL *ll) {
    if (!ll) {
        return FALSE;
    }
    Node *tmp = ll->head;

    llStats(ll, "Dump of LL: ");
    printf("Node Dump\n");
    if (!ll->cnt) {
        printf("  No nodes\n");
        return FALSE;
    }
    while (tmp) {
        printf("  val:%016"PRIx64" %16p %16p %16p \n", tmp->d, tmp->p, tmp, tmp->n);
        tmp = tmp->n;
    }

    return FALSE;
}

int llInsertAt(LL *ll, uint64_t val, uint32_t ind) {
    Node *nn;
    Node *pn;
    Node *node;
    uint32_t tind = ind;

    if (!ll || ind > ll->cnt) {
        return FALSE;
    }

    node = (Node*)malloc(sizeof(Node));
    if (!node) {
        return FALSE;
    }
    ll->mallocCnt++;
    node->d = val;

    if (ll->cnt/2 >= ind-1) {
        /* Move forward */
        nn = ll->head;
        pn = NULL;
        while (tind) {
            pn = nn;
            nn = nn->n;
            tind--;
        }
    } else {
        /* Move backward */
        nn = NULL;
        pn = ll->tail;
        tind = ll->cnt - ind;
        while (tind) {
            nn = pn;
            pn = pn->p;
            tind --;
        }
    }

    node->n = nn;
    node->p = pn;
    if (pn) {
        pn->n = node;
    }
    if (nn) {
        nn->p = node;
    }
    if (ind == 0) {
        ll->head = node;
    }
    if (ind == ll->cnt) {
        ll->tail = node;
    }

    ll->cnt++;

    return TRUE;
}

int llRemoveAt(LL *ll, uint64_t *val, uint32_t ind) {
    Node *node;
    uint32_t tind = ind;

    if (!ll || ll->cnt == 0 || ind >= ll->cnt) {
        *val = 0;
        return FALSE;
    }

    if (tind <= ll->cnt/2) {
        node = ll->head;
        while(tind) {
            node = node->n;
            tind--;
        }
    } else {
        tind = ll->cnt - ind - 1;
        node = ll->tail;
        while(tind) {
            node = node->p;
            tind--;
        }
    }

    *val = node->d;
    if (node->p) {
        node->p->n = node->n;
    }
    if (node->n) {
        node->n->p = node->p;
    }
    if (ind == 0)  {
        ll->head = node->n;
    }
    if (ind == ll->cnt-1) {
        ll->tail = node->p;
    }
    free(node);
    ll->freeCnt++;
    ll->cnt--;

    return TRUE;
}

int llPop(LL *ll, uint64_t *val) {
    return llRemoveAt(ll, val, ll->cnt-1);
}

int llPush(LL *ll, uint64_t val) {
    return llInsertAt(ll, val, ll->cnt);
}

int llDequeue(LL *ll, uint64_t *val) {
    return llRemoveAt(ll, val, 0);
}

int llEnqueue(LL *ll, uint64_t val) {
    return llInsertAt(ll, val, ll->cnt);
}

int llAppend(LL *ll, uint64_t val) {
    return llInsertAt(ll, val, ll->cnt);
}

int lltest_main(int argc, char* argv[]) {
    uint64_t val;
    uint64_t pos;
    int len;
    char buf[1024];
    int quit = FALSE;
    LL *ll = NULL;

    printf("Starting LL test\n");
    ll = llNew();
    if (!ll) {
        printf("ll init failed\n");
        return 1;
    }

    while (quit == FALSE) {
        buf[0] = '\0';
        printf(">> ");
        len = getInput(buf, 1024);
        tokenize(buf, len);

        if (tokCnt == 0) {
            continue;
        } else if (!strcmp("q", tokens[0])) {
            quit = TRUE;
        } else if (!strcmp("i", tokens[0])) {
            if (tokCnt != 3) {
                printf("Usage: i <val> <pos>\n");
                continue;
            }
            if (!parseNumericArg(tokens[1], &val)){
                printf("Failed parsing value\n");
                continue;
            }
            if (!parseNumericArg(tokens[2], &pos)){
                printf("Failed parsing value\n");
                continue;
            }
            if (llInsertAt(ll, val, pos) == FALSE){
                printf("Insert Failed\n");
            }
        } else if (!strcmp("pop", tokens[0])) {
            if (llPop(ll, &val) == FALSE){
                printf("Pop Failed\n");
            } else {
                printf("%"PRIx64"\n", val);
            }
        } else if (!strcmp("push", tokens[0])) {
            if (tokCnt != 2) {
                printf("Usage: push <val>\n");
                continue;
            }
            if (!parseNumericArg(tokens[1], &val)){
                printf("Failed parsing value\n");
                continue;
            }
            if (llPush(ll, val) == FALSE){
                printf("Push Failed\n");
            }
        } else if (!strcmp("enq", tokens[0])) {
            if (tokCnt != 2) {
                printf("Usage: enqueue <val>\n");
                continue;
            }
            if (!parseNumericArg(tokens[1], &val)){
                printf("Failed parsing value\n");
                continue;
            }
            if (llEnqueue(ll, val) == FALSE){
                printf("Enqueue Failed\n");
            }
        } else if (!strcmp("deq", tokens[0])) {
            if (llDequeue(ll, &val) == FALSE){
                printf("Dequeue Failed\n");
            } else {
                printf("%"PRIx64"\n", val);
            }
        } else if (!strcmp("append", tokens[0])) {
            if (tokCnt != 2) {
                printf("Usage: append <val>\n");
                continue;
            }
            if (!parseNumericArg(tokens[1], &val)){
                printf("Failed parsing value\n");
                continue;
            }
            if (llAppend(ll, val) == FALSE){
                printf("Append Failed\n");
            }
        } else if (!strcmp("d", tokens[0])) {
            llDump(ll);
        } else if (!strcmp("s", tokens[0])) {
            llStats(ll, NULL);
        } else if (!strcmp("delete", tokens[0])) {
            llDelete(ll);
        } else {
            printf("Unknown Cmd: %s\n", tokens[0]);
        }
    }

    printf("LL test done\n");
    return 0;
}
 

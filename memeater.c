/*
 * memeater utility (SivanagBalla@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <inttypes.h>

#include "util.h"

int memeater_main(int argc, char* argv[]) {
    int quit = FALSE;
    char buf[1024];

    int sbrk_c = 0;
    int sbrk_t = 0;
    int mmap_c = 0;
    int mmap_t = 0;
    pid_t pid = 0;
    int fd = -1;

    uint64_t len;

    void* brk_pos;
    void* cur_brk_pos;

    printf("Memory Eater\n");
    brk_pos = sbrk(0);
    pid = getpid();
    printf("sbrk: %p pid: %d\n", brk_pos, pid);

    while (quit == FALSE) {
        buf[0] = '\0';
        printf(">> ");
        len = getInput(buf, 1024);
        tokenize(buf, len);

        if (tokCnt == 0) {
            continue;
        } else if (!strcmp("q", tokens[0])) {
            quit = TRUE;
        } else if (!strcmp("eat", tokens[0])) {
            uint64_t size, cnt, echo;
            void *ptr;
            int i;
            if (tokCnt != 4) {
                printf("Usage: eat <size> <cnt> <echo>\n");
                continue;
            }
            if(parseNumericArg(tokens[1], &size) == FALSE ||
                    parseNumericArg(tokens[2], &cnt) == FALSE ||
                    parseNumericArg(tokens[3], &echo) == FALSE) {
                printf("Parsing error: Usage: eat <size> <cnt> <echo>\n");
                continue;
            }
            printf("Eating %"PRId64" byte chunks %"PRId64" times\n", size, cnt);
            sbrk_c = 0;
            mmap_c = 0;
            for(i=0; i < cnt; i++) {
                ptr = malloc(size);

                if (!ptr) {
                    printf("Alloc failed\n");
                    break;
                }
                memset(ptr, 123, size); 

                cur_brk_pos = sbrk(0);
                if ( ptr < cur_brk_pos) {
                    brk_pos = cur_brk_pos;
                    sbrk_c++;
                    sbrk_t++;
                    if (echo)
                        printf("%09d: SBRK ptr=%p sbrk=%p \n",i, ptr, sbrk(0));
                } else {
                    mmap_c++;
                    mmap_t++;
                    if (echo)
                        printf("%09d: MMAP ptr=%p sbrk=%p \n", i, ptr, sbrk(0));

                }
            }
            printf("%09d: last allocation ptr=%p sbrk=%p \n", i, ptr, sbrk(0));
            printf("================\n");
            printf("sbrk_c:%08d sbrk_t:%08d mmap_c:%08d mmap_t:%08d\n",
                    sbrk_c, sbrk_t, mmap_c, mmap_t);
            printf("================\n");
        } else if (!strcmp("system", tokens[0])) {
            if (tokCnt < 2 ) {
                printf("Usage: system <cmd>\n");
                continue;
            }
        } else if (!strcmp("open", tokens[0])) {
            int flags;

            if (fd != -1) {
                printf("Error: fd already open\n");
                continue;
            }
            if (tokCnt != 3){
                printf("Usage: open <file> <mode>\n");
                continue;
            }

            if (!strcmp(tokens[2], "r")) {
                flags = O_RDONLY;
            } else if (!strcmp(tokens[2], "w")) {
                flags = O_WRONLY;
            } else if (!strcmp(tokens[2], "rw")) {
                flags = O_RDWR;
            } else {
                printf("Unknown mode: %s\n", tokens[2]);
                continue;
            }

            fd = open(tokens[1], flags);
            if ( fd == -1 ) {
                printf("Failed to  open %s: %d: %s\n", tokens[1],
                        errno, strerror(errno));
            }
        } else if (!strcmp("close", tokens[0])) {
            if (fd == -1) {
                printf("Nothing open\n");
                continue;
            }
            close(fd);
            printf("fd closed\n");
        } else {
            printf("Unknown Cmd: %s\n", tokens[0]);
        }
            
    }
    return EXIT_SUCCESS;
}

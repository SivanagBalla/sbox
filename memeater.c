/*
 * memeater utility (SivanagBalla@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h> 
#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "util.h"

#define MAX_PTR 1024

uint64_t getTotalSystemMemory() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

void help() {

}



int memeater_main(int argc, char* argv[]) {
    uint64_t totMemory;
    int exit;
    char input[256];
    char cmd;
    char buf[256];
    uint64_t size;
    char *ptr[MAX_PTR];
    int ptr_ind = 0;
    void *sbrk_pos;
    int delms, cnt;

    totMemory = getTotalSystemMemory();
    sbrk_pos = sbrk(0);
    printf("PID: %d\n", getpid());
    printf("Total Memory: %"PRIu64" bytes (%s)\n", totMemory, 
            toHRSize(totMemory));
    printf("sbrk: %p\n", sbrk_pos);

    exit = FALSE;
    while (!exit) {
        printf("hgry > ");
        fgets(input, 256, stdin);
        sscanf(input, "%c", &cmd);

        switch(cmd) {
            case 'q': 
                exit = TRUE;
                break;

            case 'h':
                help();
                break;

            case  'e':
                sscanf(input, "%*c %s", buf);
                if(parseNumericArg(buf, &size))
                    continue;
                ptr[ptr_ind] = (char*) malloc(size);
                if (!ptr[ptr_ind]) {
                    printf("Didn't get memory\n");
                    continue;
                }
                sbrk_pos = sbrk(0);
                printf("Got memory at %p, sbrk:%p, Eating now... ", 
                            ptr[ptr_ind], sbrk_pos);
                fflush(stdout);
                memset(ptr[ptr_ind], '0', size);
                printf("done. Still hungry\n");
                ptr_ind++;
                if (ptr_ind >= MAX_PTR) {
                    ptr_ind = 0;
                }
                break; 
            case 'p':
                delms = 0;
                cnt = 0;
                sscanf(input, "%*c %s %d %d", buf, &delms, &cnt);
                if (delms == 0) {
                    printf("usage: p <size> <interval in ms> <count>\n");
                    printf("Periodic eat: interval cannot be zero\n");
                    continue;
                }
                if (cnt == 0) {
                    printf("usage: p <size> <interval in ms> <count>\n");
                    printf("Periodic eat: count cannot be zero\n");
                    continue;
                }
                if(parseNumericArg(buf, &size)){
                    printf("usage: p <size> <interval in ms> <count>\n");
                    continue;
                }

                printf("Eating %s periodically every %dms for %d times\n",
                        toHRSize(size), delms, cnt);
                while (cnt > 0) {
                    cnt--;
                    ptr[ptr_ind] = (char*) malloc(size);
                    if (!ptr[ptr_ind]) {
                        printf("Didn't get memory\n");
                        break;
                    }
                    sbrk_pos = sbrk(0);
                    printf("Got memory at %p, sbrk:%p, Eating now... ", 
                            ptr[ptr_ind], sbrk_pos);
                    fflush(stdout);
                    memset(ptr[ptr_ind], '0', size);
                    printf("done.\n");
                    ptr_ind++;
                    if (ptr_ind >= MAX_PTR) {
                        ptr_ind = 0;
                    }

                    usleep(delms*1000);
                }
                printf("Still hungry\n");
                break;

            case 'f':
                system("free -h");
                break;

            case 'v':
                ptr_ind--;
                if(ptr_ind < 0) {
                    ptr_ind = 0;
                    printf("Wrap around error");
                    break;
                }
                printf("Vomiting mem at %p\n", ptr[ptr_ind]);
                free(ptr[ptr_ind]);
                printf("Done, srbk:%p\n", sbrk_pos);
                break;
                
            default:
                printf("Invalid command: %c\n", cmd);
                break;
        }
    }

    return EXIT_SUCCESS;
}

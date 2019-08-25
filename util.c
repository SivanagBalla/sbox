/**
 * util.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "util.h"

/**
 * parseNumericArg()
 */
int parseNumericArg(const char *optarg, uint64_t *val) {
#define pprintf if(p_debug) printf
    int p_debug = 0;
    int len = strlen(optarg);
    char arg[256] = {0};
    uint64_t scale = 1;
    char *end;

    if (len == 0) {
        pprintf("Zero null arg: optarg\n");
        return 1;
    }

    if (val == NULL) {
        pprintf("Null arg: val\n");
        return 1;
    }

    strncpy(arg, optarg, 255);
    pprintf("arg: %s\n", arg);

    if (arg[0] == '0') {
        char *lead = arg;
        char *trail = arg;

        while (*lead !='\0') {
            if (*lead != '_') {
                *trail = *lead;
                trail++;
            }
            lead++;
        }
        *trail = '\0';
        pprintf("Stripped _ : %s\n", arg);

        if (len >= 2 && (arg[1] == 'x' || arg[1] == 'X')) {
            pprintf("Hex argument\n");
            *val = strtoul(arg+2, &end, 16);
            if (*end != '\0') {
                printf("Hex format error\n");
                return 1;
            }
            pprintf("val = %"PRIu64"\n", *val);
            return 0;
        } else if (len >= 2 && (arg[1] == 'b' || arg[1] == 'B')) {
            pprintf("Bin argument\n");
            *val = strtoul(arg+2, &end, 2);
            if (*end != '\0') {
                printf("Binary format error\n");
                return 1;
            }
            pprintf("val = %"PRIu64"\n", *val);
            return 0;
        }
        pprintf("Octal argument\n");

        *val = strtoul(arg+1, &end, 8);
        if (*end != '\0') {
            printf("Octal format error\n");
            return 1;
        }
        pprintf("val = %"PRIu64"\n", *val);
        return 0;
    }

    switch (arg[len-1]) {
        case 'K':
            scale = 1024;
            arg[len-1] = '\0';
            break;
        case 'M':
            scale = 1024*1024;
            arg[len-1] = '\0';
            break;
        case 'G':
            scale = 1024UL*1024*1024;
            arg[len-1] = '\0';
            break;
        case 'T':
            scale = 1024UL*1024*1024*1024;
            arg[len-1] = '\0';
            break;
    }
    pprintf("scale = %"PRIu64"\n", scale);
    pprintf("arg = %s\n", arg);

    *val = strtoul(arg, &end, 10);
    if (*end != '\0') {
        printf("Numeric Arg error\n");
        return 1;
    }
    *val *= scale;

    pprintf("val = %"PRIu64"\n", *val);
    return 0;
}

char hrBuf[256];
char* toHRSize(uint64_t size) {
    hrBuf[0] = '\0';
    if (size >> 60)
        snprintf(hrBuf, 256, "%dE", (int)(size >> 60));
    else if (size >> 50)
        snprintf(hrBuf, 256, "%dP", (int)(size >> 50));
    else if (size >> 40)
        snprintf(hrBuf, 256, "%dT", (int)(size >> 40));
    else if (size >> 30)
        snprintf(hrBuf, 256, "%dG", (int)(size >> 30));
    else if (size >> 20)
        snprintf(hrBuf, 256, "%dM", (int)(size >> 20));
    else if (size >> 10)
        snprintf(hrBuf, 256, "%dK", (int)(size >> 10));
    else
        snprintf(hrBuf, 256, "%dB", (int)size);
    return hrBuf;
}

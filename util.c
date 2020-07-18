/**
 * util.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
        return FALSE;
    }

    if (val == NULL) {
        pprintf("Null arg: val\n");
        return FALSE;
    }

    strncpy(arg, optarg, 255);
    pprintf("arg: %s\n", arg);

    if(arg[0] == '0') {
        char *lead = arg;
        char *trail = arg;

        while( *lead !='\0' ) {
            if ( *lead != '_' ) {
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
                return FALSE;
            }
            pprintf("val = %"PRIx64"\n", *val);
            return TRUE;
        }else if (len >= 2 && (arg[1] == 'b' || arg[1] == 'B')) {
            pprintf("Bin argument\n");
            *val = strtoul(arg+2, &end, 2);
            if (*end != '\0') {
                printf("Binary format error\n");
                return FALSE;
            }
            pprintf("val = %"PRIx64"\n", *val);
            return TRUE;
        }
        pprintf("Octal argument\n");

        *val = strtoul(arg+1, &end, 8);
        if (*end != '\0') {
            printf("Octal format error\n");
            return FALSE;
        }
        pprintf("val = %"PRIx64"\n", *val);
        return TRUE;
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
    pprintf("scale = %"PRIx64"\n", scale);
    pprintf("arg = %s\n", arg);

    *val = strtoul(arg, &end, 10);
    if (*end != '\0') {
        pprintf("Decimal error\n");
        return FALSE;
    }
    *val *= scale;

    pprintf("val = %"PRIx64"\n", *val);
    return TRUE;
}

/**
 * getinput
 */
int getInput(char *s, int len) {
    //https://stackoverflow.com/questions/3276546/how-to-implement-getch-function-of-c-in-linux
    fgets(s, len, stdin);
    return strlen(s);
}

/**
 * tokenize
 */
#define MAX_TOKENS 256
char *tokens[MAX_TOKENS];
int tokCnt = 0;
int tokenize(char* buf, uint32_t len) {
    int quoted = FALSE; 
    int quoteChar;
    int token = FALSE;
    int ind = 0;

    stripStr(buf, len);
    len = strlen(buf);
    tokCnt = 0;

    while(ind < len && tokCnt < MAX_TOKENS) {
        if (buf[ind] == '\0') {
            break;
        } else if (quoted == TRUE) {
            if (quoteChar == buf[ind]) {
                quoted = FALSE;
            } else {
                ind++;
                continue;
            }
        } else if (buf[ind] == '"' || buf[ind] == '\'') {
            quoted = TRUE;
            quoteChar = buf[ind];
            if (token == FALSE) {
                token = TRUE;
                tokens[tokCnt++] = buf+ind;
            }
        } else {
            if (isspace(buf[ind])) {
                buf[ind] = '\0';
                token = FALSE;
            } else if (token == FALSE){
                token = TRUE;
                tokens[tokCnt++] = buf+ind;
            }
        }
        ind++;
    }
    if (quoted) {
        return FALSE;
    }
    return TRUE;
}

char* stripStr(char *s, int len) {
    char *sptr = s;
    char *eptr = s + len - 1;
    int i;

    if (!s || len == 0) {
        return s;
    }

    while (sptr < eptr && *sptr != '\0' && isspace(*sptr)) {
        sptr++;
    }
    
    while (eptr > sptr && (*eptr == '\0' || isspace(*eptr))) {
        eptr--;
    }

    i = 0;
    while (sptr <= eptr) {
        s[i++] = *(sptr++);
    }
    s[i] = '\0';
    return s;
}

int util_main(int argc, char* argv[]) {
    char buf[1024];
    int quit = FALSE;
    int len;

    while (quit == FALSE) {
        buf[0] = '\0';
        printf(">> ");
        len = getInput(buf, 1024);
        stripStr(buf, len);

        if (buf[0] == 'q') {
            quit = TRUE;
        } else if (!strncmp(buf, "token", sizeof("token"))) {
            int ret;
            int i;
            printf("token >> ");
            len = getInput(buf, 1024);
            ret = tokenize(buf, len);
            printf("Ret: %d\n", ret);
            printf("Cnt: %d\n", tokCnt);
            if (ret == TRUE) {
                for (i=0; i < tokCnt; i++){
                    printf("  %3d: %s\n", i, tokens[i]);
                }
            }
        }
    }
    return 0;
}

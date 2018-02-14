/*
 * sbox.h
 */

#ifndef __SBOX_H__
#define __SBOX_H__

#define MAX_UTIL_NAME 16

typedef struct {
    char utilName[MAX_UTIL_NAME];
    int (*utilMain)(int argc, char* arg[]);
} utils_t;

int symlinks_main(int argc, char* argv[]);

#endif /* __SBOX_H__ */

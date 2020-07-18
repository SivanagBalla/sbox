/*
 * genmain.c
 * Generic main function for building individual utilities
 */

#ifndef UTIL
    #error UTIL not defined
#endif

#include "util.h"
#include "sbox.h"

int main(int argc , char* argv[]) {
    return UTIL(argc, argv);
}

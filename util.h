/**
 * util.h
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

#define dprintf if(debug_en) printf

#define TRUE 1
#define FALSE 0

extern int debug_en;
int parseNumericArg(const char *optarg, uint64_t *val);
char* toHRSize(uint64_t size);

#endif /* __UTIL_H__ */

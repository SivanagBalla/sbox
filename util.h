/**
 * util.h
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

#define dprintf if(debug_en) printf
#define TRUE  1
#define FALSE 0

extern int debug_en;
extern char *tokens[256];
extern int tokCnt;

int parseNumericArg(const char *optarg, uint64_t *val);
int getInput(char *s, int len);

int tokenize(char* buf, uint32_t len);
char* stripStr(char *s, int len);
int util_main(int argc, char* argv[]);

#endif /* __UTIL_H__ */

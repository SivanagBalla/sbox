/**
 * util.h
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#define dprintf if(debug_en) printf

extern int debug_en;
int parseNumericArg(const char *optarg, uint64_t *val);

#endif /* __UTIL_H__ */

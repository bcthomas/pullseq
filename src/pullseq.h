#ifndef __PULLSEQ_H__
#define __PULLSEQ_H__
#endif

#define _POSIX_C_SOURCE 200809L

#ifdef DEBUG
#define DEBUGP(x, args...) fprintf(stderr, " [%s(), %s:%u]\n" x, __FUNCTION__, __FILE__,__LINE__, ## args)
#else
#define DEBUGP(x, args...)
#endif

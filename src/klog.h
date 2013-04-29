#ifndef __KLOG_H__
#define __KLOG_H__ 1

#include <stdio.h>
#include "kmacro.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

extern FILE* gLogFd;

#define MOBUG_MODE 0
#if MOBUG_MODE
	#define DBG_ENABLE	1
	#define WARN_ENABLE	1
#else
	#define DBG_ENABLE	0
	#define WARN_ENABLE	0
#endif
#define INFO_ENABLE	1
#define ERR_ENABLE	1

#if DBG_ENABLE
	#define KDBG(fmt,...) { KLOG(gLogFd, "DEBUG", fmt, ##__VA_ARGS__); }
	#define KHEX(TAG,DATA,LEN) {KLOG_LINESTAMP(gLogFd, "HEXDUMP"); hexDump(TAG,DATA,LEN); }
	#define KLINESTAMP() { KLOG_LINESTAMP(gLogFd, "LINESTAMP"); fprintf(gLogFd, "\n"); }
#else
	#define KDBG(fmt,...) {};
	#define KHEX(TAG,DATA,LEN) {};
	#define KLINESTAMP() {};
#endif

#if WARN_ENABLE
	#define KWARN(fmt,...) KLOG(gLogFd, "WARNING", fmt, ##__VA_ARGS__);
#else
	#define KWARN(fmt,...) {};
#endif

#if ERR_ENABLE
	#define KERR(fmt,...)  KLOG(gLogFd, "ERROR", fmt, ##__VA_ARGS__);
#else
	#define KERR(fmt,...) {};
#endif

#if INFO_ENABLE
	#define KINFO(fmt,...)  KLOG(gLogFd, "INFO", fmt, ##__VA_ARGS__);
#else
	#define KINFO(fmt,...) {};
#endif
#define KDUMP(...) { fprintf(gLogFd, ##__VA_ARGS__); fprintf(gLogFd, "\n"); }

void clear_log(void);
int init_log(const char* fileName);
void hexDump(const char *pTagName, char *pHexData, int nLen);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__KLOG_H__*/



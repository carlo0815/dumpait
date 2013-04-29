#ifndef __KMACRO_H__
#define __KMACRO_H__ 1

#include <time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define TIMESTAMP(b) \
	do { \
		time_t c; \
		struct tm* p; \
		c = time(NULL); \
		p = localtime(&c); \
		strftime(b, sizeof(b), "%Y.%m.%d %H:%M:%S", p); \
	} while(0)

#define MAKE_LOGFILE_NAME(buff,fmt,...) \
	do { \
		char timestamp[15] = {0}; \
		TIMESTAMP(timestamp); \
		sprintf(buff, fmt"_%s", ##__VA_ARGS__, timestamp);\
	} while(0)

#define KLOG(fd, level, fmt, ...) \
	do { \
		if(!fd) break; \
		char bb[20] = {0}; TIMESTAMP(bb); \
		fprintf(fd, "[%s] [" level "]\t" fmt " (%s:%d - %s)\n", bb, ##__VA_ARGS__, __FUNCTION__, __LINE__, __FILE__); \
	} while (0)

#define KLOG_LINESTAMP(fd, level) \
	do { \
		if(!fd) break; \
		char bb[20] = {0}; TIMESTAMP(bb); \
		fprintf(fd, "[%s] [" level "]\t(%s:%d - %s)", bb, __FUNCTION__, __LINE__, __FILE__); \
	} while (0)

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__KMACRO_H__*/


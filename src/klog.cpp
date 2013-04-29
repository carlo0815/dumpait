#include <stdio.h>
#include <ctype.h>

#include "klog.h"
#include "define.h"

#define UNITTEST 0

FILE* gLogFd;

int init_log(const char* filename)
{
	if(filename == NULL) {
		gLogFd = stdout;
		return TRUE;
	} else {
		if(!(gLogFd = fopen(filename, "a+"))) {
			gLogFd = NULL;
			return FALSE;
		}
	}
	return TRUE;
}

void clear_log(void)
{
	if(gLogFd && (gLogFd != stdout))
		fclose(gLogFd);
	gLogFd = NULL;
}

void hexDump(const char *pTagName, char *pHexData, int nLen)
{
	unsigned char 	*ptr1=(unsigned char *)pHexData, *ptr2=(unsigned char *)pHexData;
	int		i, j;
	FILE	*fd = gLogFd;

	fprintf(fd, "\t%s [%d]\n", pTagName, nLen);
	fprintf(fd, "================== BINARY HEX ================= ==== ASCII =====\n");
	for(i = 0; i < nLen; i++ ) {
		fprintf(fd, "%02x ", (unsigned int)*(ptr1+i));
		if(((i+1)%16 == 0) && (i > 0)) {
			for(j = 0; j < 16; j++) {
				if(isprint(*(ptr2+j)) && (iscntrl(*(ptr2+j)) == 0) && *(ptr2+j) != '^')
					fprintf(fd, "%c", *(ptr2+j));
				else
					fprintf(fd, ".");
			}
			fprintf(fd, "\n");
			fflush(fd);
			ptr2 = ptr1+i+1;
		}
	}

	if((((i+1)%16) <= 15) && ((i+1)%16 != 1)) {
		if((i+1)%16 == 0) 
			fprintf(fd, "   ");
		else 
			for(j = ((i+1)%16)-1; j < 16; j++)
				fprintf(fd, "   ");

		if((i+1)%16 == 0) 
			for(j = 0; j < 15; j++) {
				if(isprint(*(ptr2+j)) && (iscntrl(*(ptr2+j)) == 0) && *(ptr2+j) != '^')
					fprintf(fd, "%c", *(ptr2+j));
				else
					fprintf(fd, ".");
			}
		else
			for(j = 0; j < (i+1)%16-1; j++) {
				if(isprint(*(ptr2+j)) && (iscntrl(*(ptr2+j)) == 0) && *(ptr2+j) != '^')
					fprintf(fd, "%c", *(ptr2+j));
				else
					fprintf(fd, ".");
			}
		fprintf(fd, "\n");
		fflush(fd);
	}
	fprintf(fd, "----------------------------------------------- ----------------\n");
	fflush(fd);
}


#if UNITTEST
int main(void)
{
	if(initLog(NULL) == FALSE) {
		perror("init log fail!!");
		return -1;
	}

	char* data = (char*) "This is test!!!";
	KDBG("TEST!! %d", 12);
	KHEX("TAG", data, strlen(data));

	clearLog();
	return 0;
}
#endif

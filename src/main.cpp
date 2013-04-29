#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <getopt.h> 

#include "klog.h"
#include "kutils.h"
#include "define.h"
#include "dvb_scanner.h"

#define usage() { printf("Usage: dumpait --demux={HEXA} --pmtid={HEXA} --serviceid={HEXA}\n\n"); }

static int checkBox(void)
{
	int  nn, fd = 0;
	char nm[32] = {0};

	nn = ((fd=open("/proc/stb/info/vumodel", O_RDONLY)) >= 0) ? read(fd, nm, 32) : 0;
	if(fd >= 0) close(fd);
	return ((strncmp(nm, "duo", 3) == 0) || (strncmp(nm, "solo", 4) == 0) || (strncmp(nm, "uno", 3) == 0) || (strncmp(nm, "ultimo", 6) == 0));
}

/* ./dumpait --demux=0 --pmtid=13ec --serviceid=283d */
int main(int argc, char** argv)
{
	int option	= 0;

	int demuxid	= -1;
	int pmtid	= -1;
	int serviceid	= -1;

	if(!checkBox()) {
		return -1;
	}

	if(argc < 4) {
		usage();
		return -1;
	}

	/* Parse Arguments */
	static struct option long_options[] = { 
		{"help",	0, NULL, 'h'}, 
		{"demux",	1, NULL, 'd'}, 
		{"pmtid",	1, NULL, 'p'}, 
		{"serviceid",	1, NULL, 's'}, 
		{0, 0, 0, 0} 
	}; 

	if(init_log(NULL) == FALSE) {
		perror("init log fail!!");
		return -1;
	}

	do {
		/* ":" -> need value behind option. */
		option = getopt_long(argc, argv, "d:p:s:h", long_options, 0);
		switch (option) {
		case 'd':
			if(is_hex_str(optarg))
				demuxid = htoi(optarg);
			KDBG("D : %s, %X", optarg, demuxid);
			break;
		case 'p': 
			if(is_hex_str(optarg))
				pmtid = htoi(optarg);
			KDBG("P : %s, %X", optarg, pmtid);
			break; 
		case 's':
			if(is_hex_str(optarg))
				serviceid = htoi(optarg);
			KDBG("S : %s, %X", optarg, serviceid);
			break;
		case 'h':
			clear_log();
			return 0;
		}
	} while(option != -1);

	KDBG("DemuxId : %d, PmtId : %X, ServiceId : %X", demuxid, pmtid, serviceid);

	if(demuxid < 0) {
		KERR("DemuxId is not seted!!");
		clear_log();
		return -1;
	}
#if 1
	if(pmtid < 0) {
		KERR("PmtId is not seted!!");
		clear_log();
		return -1;
	}
	if(serviceid < 0) {
		KERR("ServiceId is not seted!!");
		clear_log();
		return -1;
	}
#endif
	eAITScanner().doScan(demuxid, pmtid, serviceid);

	clear_log();
	return 0;
}


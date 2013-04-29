#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>   
#include <sys/types.h>   
#include <sys/time.h>

#include <iostream>

#include "define.h"

#include "klog.h"

#include "dvb_specs.h"
#include "dvb_demux.h"

eDemux::eDemux() : mDemuxFd(0)
{
	memset(mBuffer, 0, SECTION_BUFF_SIZE);
}

eDemux::~eDemux()
{
	close();
}

int eDemux::isOpened(void) 
{
	return mDemuxFd;
}

int eDemux::setFilter(const eDVBSectionFilterMask &mask)
{
	int res;
	if (mDemuxFd < 0)
		return -ENODEV;

	dmx_sct_filter_params sct;

	sct.pid		= mask.pid;
	sct.timeout	= 0;
	sct.flags	= DMX_IMMEDIATE_START;

	memcpy(sct.filter.filter, mask.data, DMX_FILTER_SIZE);
	memcpy(sct.filter.mask, mask.mask, DMX_FILTER_SIZE);
	memcpy(sct.filter.mode, mask.mode, DMX_FILTER_SIZE);
	::ioctl(mDemuxFd, DMX_SET_BUFFER_SIZE, 8192*8);
	return ::ioctl(mDemuxFd, DMX_SET_FILTER, &sct);
}

eDVBSectionFilterMask& eDemux::filterToMask(const eDVBTableSpec &table)
{
	static eDVBSectionFilterMask mask;

	memset(&mask, 0, sizeof(mask));
	mask.pid   = table.pid;
	mask.flags = 0;

	if (table.flags & eDVBTableSpec::tfCheckCRC)
		mask.flags |= eDVBSectionFilterMask::rfCRC;

	if (table.flags & eDVBTableSpec::tfHaveTID) {
		mask.data[0] = table.tid;
		if (table.flags & eDVBTableSpec::tfHaveTIDMask)
			mask.mask[0] = table.tid_mask;
		else
			mask.mask[0] = 0xFF;
	}

	if (table.flags & eDVBTableSpec::tfHaveTIDExt) {
		mask.data[1] = table.tidext >> 8;
		mask.data[2] = table.tidext;
		if (table.flags & eDVBTableSpec::tfHaveTIDExtMask) {
			mask.mask[1] = table.tidext_mask >> 8;
			mask.mask[2] = table.tidext_mask;
		} else {
			mask.mask[1] = 0xFF;
			mask.mask[2] = 0xFF;
		}
	}

	if (!(table.flags & eDVBTableSpec::tfAnyVersion)) {
		mask.data[3] |= (table.version << 1)|1;
		mask.mask[3] |= 0x3f;
		if (!(table.flags & eDVBTableSpec::tfThisVersion))
			mask.mode[3] |= 0x3e;
	} 
	return mask;
}

int eDemux::open(int demuxId, const eDVBTableSpec &table) 
{
	int rc = 0;
	char filePath[256] = {0};
	sprintf(filePath, "/dev/dvb/adapter0/demux%d", demuxId);
	KDBG("Open Demux : %s", filePath);

	mDemuxFd = ::open(filePath, O_RDWR);
	if(mDemuxFd <= 0) {
		KERR("Fail to open Demux-%d", demuxId);
		mDemuxFd = 0;
		return NULL;
	}

	KDBG("Success to open Demux-%d", demuxId);
	rc = setFilter(filterToMask(table));
	return mDemuxFd;
}

unsigned char* eDemux::read(int &nBytesRead) 
{
	if(!mDemuxFd) {
		return NULL;
	}

#if 1
	memset(mBuffer, 0, SECTION_BUFF_SIZE);
	nBytesRead = ::read(mDemuxFd, mBuffer, SECTION_BUFF_SIZE);
	if(!nBytesRead) {
		return NULL;
	}
	return mBuffer;	
#else
	fd_set readfds, writefds, exceptfds;
	FD_ZERO(&readfds);
	FD_SET(mDemuxFd, &readfds);

	FD_ZERO(&writefds);
	FD_SET(mDemuxFd, &writefds);

	FD_ZERO(&exceptfds);
	FD_SET(mDemuxFd, &exceptfds);

	struct timeval tv = {5,0};

	int rc = select(1, &readfds, &writefds, &exceptfds, &tv);
	KDBG("select() : %d", rc);

	if(rc > 0) {
		if(FD_ISSET(mDemuxFd, &readfds)) {
			memset(mBuffer, 0, SECTION_BUFF_SIZE);
			nBytesRead = ::read(mDemuxFd, mBuffer, SECTION_BUFF_SIZE);
			if(!nBytesRead) {
				return NULL;
			}
			return mBuffer;	
		}
		if(FD_ISSET(mDemuxFd, &writefds)) {
			KLINESTAMP();
		}
		if(FD_ISSET(mDemuxFd, &exceptfds)) {
			KLINESTAMP();
		}
	}

	return NULL;
#endif
}

void eDemux::close(void)
{ 
	memset(mBuffer, 0, SECTION_BUFF_SIZE);
	if(mDemuxFd) {
		::close(mDemuxFd);
	}
	mDemuxFd = NULL;
}



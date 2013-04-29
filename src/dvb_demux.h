#ifndef __DVB_DEMUX_H__
#define __DVB_DEMUX_H__ 1

#include "define.h"
#include "dvb_specs.h"

class eDemux
{
private:
	int mDemuxFd;
	unsigned char mBuffer[SECTION_BUFF_SIZE];
protected:
	int isOpened(void);
	int setFilter(const eDVBSectionFilterMask &mask);
	eDVBSectionFilterMask& filterToMask(const eDVBTableSpec &table);
public:
	eDemux();
	~eDemux();

	int open(int demuxId, const eDVBTableSpec &table);
	unsigned char* read(int &readCount);
	void close(void);
};

#endif /*__DVB_DEMUX_H__*/

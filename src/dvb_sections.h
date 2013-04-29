#ifndef __DVB_SECTIONS_H__
#define __DVB_SECTIONS_H__ 1

#include <list>
#include <stdio.h>

#include "define.h"
#include "dvb_specs.h"
#include "dvb_demux.h"

namespace SectionType {
	typedef enum {
		NONE=0x00
		, PAT
		, PMT
		, AIT
		, DSMCC
	} Type;
};

class eSectionData: public LongCrcSection
{
private:
	int mLength;
	unsigned char* mData;
public:
	eSectionData(unsigned char* buffer)
		: LongCrcSection(buffer)
	{
		mLength = getSectionLength();
		mData = (unsigned char*) calloc(1, mLength);
		if(mData) {
			memcpy(mData, buffer, mLength);
		}
	}
	~eSectionData()
	{
		if(mData) {
			free(mData);
		}
		mData = 0;
	}
	unsigned char* getData(void)
	{
		return mData;
	}
	int getLength(void)
	{
		return mLength;
	}
};
typedef std::list<eSectionData *> eSectionDataList;
typedef eSectionDataList::iterator eSectionDataListIterator;
typedef eSectionDataList::const_iterator eSectionDataListListConstIterator;


class eSectionReader : public eDemux
{
protected:
	void push(SectionType::Type type, void *sectionList, unsigned char* buffer);
public:
	eSectionReader();
	~eSectionReader();
	BOOL scan(int demuxId, SectionType::Type type, void *list, void *param);
};

#endif /*__DVB_SECTIONS_H__*/


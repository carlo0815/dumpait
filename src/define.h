#ifndef __KDEFINE_H__
#define __KDEFINE_H__ 1

#ifndef TRUE
	#define TRUE	1
	#define FALSE	0
	#define BOOL	int
#endif

#ifndef NULL
	#define NULL	0
#endif

#ifndef SECTION_BUFF_SIZE
	#define SECTION_BUFF_SIZE 4096
#endif

#ifndef DMX_FILTER_SIZE
	#define DMX_FILTER_SIZE   16
#endif

#include <list>
#include <string.h>

class eAITData
{
public:
	int mAitId;
	int mBufferLen;
	unsigned char mBuffer[SECTION_BUFF_SIZE];
public:
	eAITData(int aitid) 
		: mAitId(aitid), mBufferLen(0)
	{
		memset(mBuffer, 0, SECTION_BUFF_SIZE);
	}
	void set(unsigned char *data, int length) 
	{
		mBufferLen = length;
		memcpy(mBuffer, data, length);
	}
};


class ePMTData
{
public:
	int mPmtId;
	int mServiceId;
public:
	ePMTData(int pmtid, int sid) 
		: mPmtId(pmtid), mServiceId(sid)
	{}
};
typedef std::list<ePMTData *> ePMTDataList;
typedef ePMTDataList::iterator ePMTDataListIterator;
typedef ePMTDataList::const_iterator ePMTDataListConstIterator;


template <class T, class TIterator>
class eStorageManager
{
private:
	std::list<T *> mList;
public:
	eStorageManager() {}
	~eStorageManager(){ clear(); }

	void clear(void)
	{
		if(mList.size() == 0)
			return;

		TIterator pp = mList.begin();
		for(; pp != mList.end(); ++pp)
			delete (*pp);

		mList.clear();
	}
	void push(T *data)
	{
		mList.push_back(data);
	}
	std::list<T *>* get(void)
	{
		return &mList;
	}
};

#endif /*_KDEFINE_H__*/


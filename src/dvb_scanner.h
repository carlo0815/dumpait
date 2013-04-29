#ifndef __DVB_SCANNER_H__
#define __DVB_SCANNER_H__ 1

#include <dvbsi++/descriptor_tag.h>

#include "dvb_sections.h"

#include <fstream>
#include <iostream>
using namespace std;

static std::wstring str2wstr(std::string str) {
	std::wstring wstr(str.length(), L' ');
	copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}

class eApplicationInfo
{
public:
	int mOrgId;
	int mAppId;
	int mCtrlCode;
	short mProfCode;
	std::string mUrl;
	std::string mName;
public:
	eApplicationInfo(int ctrlcode, int orgid, int appid, std::string url, std::string appname, int profcode)
		: mCtrlCode(ctrlcode), mUrl(url), mOrgId(orgid), mAppId(appid), mProfCode(profcode)
	{
		for (string::iterator it = appname.begin(); it != appname.end(); ++it) {
			if(*it > 0 && *it < 32) continue;
			mName += *it;
		}

	}
	void savedump(std::ofstream &ofst)
	{
		ofst << "<application>"		<< endl;
		ofst << "       <control>"	<< mCtrlCode 	<< "</control>"	<< endl;
		ofst << "       <orgid>"	<< mOrgId 	<< "</orgid>"	<< endl;
		ofst << "       <appid>"	<< mAppId 	<< "</appid>"	<< endl;
		ofst << "       <name>"		<< mName	<< "</name>"	<< endl;
		ofst << "       <url>"		<< mUrl		<< "</url>"		<< endl;
		ofst << "       <profile>"	<< mProfCode 	<< "</profile>"	<< endl;
		ofst << "</application>"	<< endl;
	}
	void dump() 
	{
		cout << "<application>\n";
		cout << "	<control>" << mCtrlCode << "</control>\n";
		cout << "	<orgid>" << mOrgId << "</orgid>\n";
		cout << "	<appid>" << mAppId << "</appid>\n";
		cout << "	<name>" << mName << "</name>\n";
		cout << "	<url>" << mUrl << "</url>\n";
		cout << "	<profile>" << mProfCode << "</profile>\n";
		cout << "</application>\n";
	}
};
typedef std::list<eApplicationInfo *> eApplicationInfoList;
typedef eApplicationInfoList::iterator eApplicationInfoListIterator;
typedef eApplicationInfoList::const_iterator eApplicationInfoListConstIterator;

class eApplicationContainer
{
private:
	eApplicationInfoList mApplicationList;
protected:
	int getOrgId()
	{
		eApplicationInfoListConstIterator it = mApplicationList.begin();
		return (*it)->mOrgId;
	}
public:
	void append(int ctrlcode, int orgid, int appid, std::string url, std::string appname, int profcode) {
		append(new eApplicationInfo(ctrlcode, orgid, appid, url, appname, profcode));
	}
	void append(eApplicationInfo* info) {
		mApplicationList.push_back(info);
	}
	void dump() {
#if 0
		std::ofstream ofst;
		ofst.open("/tmp/app.xml");
		ofst << "<applications>\n";
		eApplicationInfoListConstIterator it = mApplicationList.begin();
		for(; it != mApplicationList.end() ; ++it) {
			(*it)->savedump(ofst);
		}
		ofst << "</applications>\n";
		ofst.close();
#else
		cout << "<applications>\n";
		eApplicationInfoListConstIterator it = mApplicationList.begin();
		for(; it != mApplicationList.end() ; ++it) {
			(*it)->dump();
		}
		cout << "</applications>\n";
#endif
	}
};

typedef eStorageManager<ePMTData, ePMTDataListConstIterator> ePMTStorageManager;

class eAITScanner
{
private:
	eApplicationContainer mAppContainer;
private:
	void dumpAITData(eAITData &ait);
	void saveSectionData(char* filename, eAITData &ait);
protected:
	void doScanPAT(int demuxId, ePMTStorageManager &esm);
	void doScanAIT(int demuxId, ProgramMapSectionList &pmts);
	void doScanPMT(int demuxId, ePMTStorageManager &esm, ProgramMapSectionList *list, ePMTData &cond);
public:
	void doScan(int demuxid, int pmtid, int serviceid);
};

#endif /*__DVB_SCANNER_H__*/


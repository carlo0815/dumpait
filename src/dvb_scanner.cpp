#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <string>

#include "klog.h"
#include "define.h"
#include "dvb_scanner.h"

#include <dvbsi++/program_map_section.h>
#include <dvbsi++/service_description_section.h>
#include <dvbsi++/network_information_section.h>
#include <dvbsi++/bouquet_association_section.h>
#include <dvbsi++/program_association_section.h>
#include <dvbsi++/event_information_section.h>
#include <dvbsi++/application_information_section.h>
#include <dvbsi++/application_profile.h>
#include <dvbsi++/application_descriptor.h>
#include <dvbsi++/simple_application_location_descriptor.h>
#include <dvbsi++/simple_application_boundary_descriptor.h>
#include <dvbsi++/transport_protocol_descriptor.h>
#include <dvbsi++/application_name_descriptor.h>
#include <dvbsi++/simple_application_boundary_descriptor.h>

#define PACK_VERSION(major,minor,micro) (((major) << 16) + ((minor) << 8) + (micro))
#define UNPACK_VERSION(version,major,minor,micro) { \
	major = (version)&0xff; \
	minor = (version>>8)&0xff; \
	micro = (version>>16)&0xff; \
}

using namespace std;

#define FILENAME_TEMPLETE "/tmp/ait.%d"

void eAITScanner::saveSectionData(char* filename, eAITData &ait)
{
	int nWriteCount, nAitFd;
	nAitFd = open(filename, O_RDWR|O_CREAT|O_TRUNC);
	if(nAitFd < 0) {
		KERR("Fail to open file. (%s)", filename);
		return;
	}
	nWriteCount = write(nAitFd, ait.mBuffer, ait.mBufferLen);
	close(nAitFd);

	//KINFO("Saved %d bytes to %s", nWriteCount, filename);
	//KDUMP("Saved %d bytes to %s", nWriteCount, filename);
}

void eAITScanner::dumpAITData(eAITData &ait)
{
	char filename[256] = {0};

	std:string applicaionName = "", hbbtvUrl = "", boundaryExtension = "";
	int profileCode = 0, orgId = 0, appId = 0, controlCode = 0, profileVersion = 0;

	ApplicationInformationSection ais = ApplicationInformationSection(ait.mBuffer);
	std::list<ApplicationInformation *>::const_iterator pp = ais.getApplicationInformation()->begin();
	for (; pp != ais.getApplicationInformation()->end(); ++pp) {
		controlCode = (*pp)->getApplicationControlCode();

		const ApplicationIdentifier* applicationIdentifier = (*pp)->getApplicationIdentifier();
		orgId = applicationIdentifier->getOrganisationId();
		appId = applicationIdentifier->getApplicationId();
		//KDBG("controlCode=%d, orgId=%d, appId=%d", controlCode, orgId, appId);
		if(controlCode == 1 || controlCode == 2) {
			if(controlCode == 1) {
				sprintf(filename, FILENAME_TEMPLETE, orgId);
			}

			DescriptorConstIterator desc = (*pp)->getDescriptors()->begin();
			//KDBG("(*pp)->getDescriptors()->size() : %d", (*pp)->getDescriptors()->size());

			for(; desc != (*pp)->getDescriptors()->end(); ++desc) {
				switch ((*desc)->getTag())
				{
				case APPLICATION_DESCRIPTOR: {
						ApplicationDescriptor* applicationDescriptor = (ApplicationDescriptor*)(*desc);
						const ApplicationProfileList* applicationProfiles = applicationDescriptor->getApplicationProfiles();
						ApplicationProfileConstIterator interactionit = applicationProfiles->begin();
						for(; interactionit != applicationProfiles->end(); ++interactionit) {
							profileCode = (*interactionit)->getApplicationProfile();
							profileVersion = PACK_VERSION(
								(*interactionit)->getVersionMajor(),
								(*interactionit)->getVersionMinor(),
								(*interactionit)->getVersionMicro()
							);
							//KDBG("profileCode=%d, profileVersion=%d", profileCode, profileVersion);
						}
						break;
					}
				case APPLICATION_NAME_DESCRIPTOR: {
						ApplicationNameDescriptor *nameDescriptor  = (ApplicationNameDescriptor*)(*desc);
						ApplicationNameConstIterator interactionit = nameDescriptor->getApplicationNames()->begin();
						for(; interactionit != nameDescriptor->getApplicationNames()->end(); ++interactionit) {
							applicaionName = (*interactionit)->getApplicationName();
							//KDBG("applicaionName=%s", applicaionName.c_str());
							break;
						}
						break;
					}
				case TRANSPORT_PROTOCOL_DESCRIPTOR: {
						TransportProtocolDescriptor *transport = (TransportProtocolDescriptor*)(*desc);
						switch (transport->getProtocolId())
						{
						case 1: /* object carousel */
							break;
						case 2: /* ip */
							break;
						case 3: { /* interaction */
								InterActionTransportConstIterator interactionit = transport->getInteractionTransports()->begin();
								for(; interactionit != transport->getInteractionTransports()->end(); ++interactionit) {
									hbbtvUrl = (*interactionit)->getUrlBase()->getUrl();
									//KDBG("TRANSPORT_PROTOCOL_DESCRIPTOR : hbbtvUrl=%s", hbbtvUrl.c_str());
									break;
								}
								break;
							}
						}
						break;
					}
				case GRAPHICS_CONSTRAINTS_DESCRIPTOR:
					break;
				case SIMPLE_APPLICATION_LOCATION_DESCRIPTOR: {
						SimpleApplicationLocationDescriptor *applicationlocation = (SimpleApplicationLocationDescriptor*)(*desc);
						hbbtvUrl += applicationlocation->getInitialPath();
						//KDBG("SIMPLE_APPLICATION_LOCATION_DESCRIPTOR : hbbtvUrl=%s", hbbtvUrl.c_str());
						break;
					}
				case APPLICATION_USAGE_DESCRIPTOR:
					break;
				case SIMPLE_APPLICATION_BOUNDARY_DESCRIPTOR: {
						SimpleApplicationBoundaryDescriptor *boundaryDescriptor  = (SimpleApplicationBoundaryDescriptor*)(*desc);
						const BoundaryExtensionList* boundaryList = boundaryDescriptor->getBoundaryExtensions();
						BoundaryExtensionConstIterator interactionit = boundaryList->begin();
						for(; interactionit != boundaryList->end(); ++interactionit) {
							boundaryExtension = (*interactionit)->getBoundaryExtension();
							//KDBG("boundaryExtension=%s", boundaryExtension.c_str());
							if(!boundaryExtension.empty()) {
								break;
							}
						}
						break;
					}
				}
			}
		}

#if 0
		KDBG("applicaionName=%s\nm_ApplicationName=%s\nhbbtvUrl=%s\nboundaryExtension=%s\nprofileCode=%d\norgId=%d\nappId=%d\ncontrolCode=%d\nprofileVersion=%d", 
			applicaionName.c_str(), applicaionName.c_str(), hbbtvUrl.c_str(), boundaryExtension.c_str(), profileCode, orgId, appId, controlCode, profileVersion);
#endif

		if(!hbbtvUrl.empty())
		{
			const char* uu = hbbtvUrl.c_str();
			if(!strncmp(uu, "http://", 7) || !strncmp(uu, "dvb://", 6) || !strncmp(uu, "https://", 8))
			{
				switch(profileVersion)
				{
				case 65793:
				case 66049:
					mAppContainer.append(controlCode, orgId, appId, hbbtvUrl, applicaionName, profileCode);
					break;
				case 1280:
				case 65538:
				default:
					mAppContainer.append((-1)*controlCode, orgId, appId, hbbtvUrl, applicaionName, profileCode);
					break;
				}
			}
			else if (!boundaryExtension.empty()) {
				if(boundaryExtension.at(boundaryExtension.length()-1) != '/') {
					boundaryExtension += "/";
				}
				boundaryExtension += hbbtvUrl;
				switch(profileVersion)
				{
				case 65793:
				case 66049:
					mAppContainer.append(controlCode, orgId, appId, boundaryExtension, applicaionName, profileCode);
					break;
				case 1280:
				case 65538:
				default:
					mAppContainer.append((-1)*controlCode, orgId, appId, boundaryExtension, applicaionName, profileCode);
					break;
				}
			}
		}
	}

	KDBG("filename=%s", filename);

	mAppContainer.dump();

	if(filename[0]) {
#if MOBUG_MODE
		char tag[256] = {0};
		sprintf(tag, "AIT-0x%X", ait.mAitId);
		KHEX(tag, (char*)ait.mBuffer, ait.mBufferLen);
#endif
		KDBG("AIT File Name : %s", filename);
		saveSectionData(filename, ait);
	}
}

void eAITScanner::doScanAIT(int demuxId, ProgramMapSectionList &pmts)
{
	ProgramMapSectionConstIterator ii = pmts.begin();
	for(; ii != pmts.end() ; ++ii)
	{
		ElementaryStreamInfoConstIterator es = (*ii)->getEsInfo()->begin();
		for(; es != (*ii)->getEsInfo()->end(); ++es)
		{
			if((*es)->getType() != 0x05)
				continue;
			DescriptorConstIterator desc = (*es)->getDescriptors()->begin();
			for(; desc != (*es)->getDescriptors()->end(); ++desc)
			{
				switch ((*desc)->getTag())
				{
				case APPLICATION_SIGNALLING_DESCRIPTOR:
					eAITData ait = eAITData((*es)->getPid());
					eSectionReader().scan(demuxId, SectionType::AIT, NULL, &ait);
					if(ait.mBufferLen) {
						dumpAITData(ait);
					}
					break;
				}
			}
		}
	}
}

void eAITScanner::doScanPAT(int demuxId, ePMTStorageManager &esm)
{
	ProgramAssociationSectionList list;
	eSectionReader().scan(demuxId, SectionType::PAT, &list, NULL);
	KDBG("PAT Size : %d", list.size());

	ProgramAssociationSectionConstIterator ii = list.begin();
	for(; ii != list.end() ; ++ii)
	{
		ProgramAssociationConstIterator pp = (*ii)->getPrograms()->begin();
		for(; pp != (*ii)->getPrograms()->end(); ++pp)
		{
			int pmtid = (*pp)->getProgramMapPid();
			int sid   = (*pp)->getProgramNumber();

			esm.push(new ePMTData(pmtid, sid));
			KDBG("PMTID : %x, SID : %x", pmtid, sid);
		}
	}
}

void eAITScanner::doScanPMT(int demuxId, ePMTStorageManager &esm, ProgramMapSectionList *list, ePMTData &cond)
{
	ePMTDataList* pmts = esm.get();
	KDBG("PAT List Size : %d", pmts->size());
	for(ePMTDataListConstIterator pp = pmts->begin() ; pp != pmts->end() ; ++pp) {
		ePMTData* pmtdata = *pp;
		KDBG("PMT Id : %x, Service Id : %x, %x,%x", pmtdata->mPmtId, pmtdata->mServiceId, cond.mPmtId, cond.mServiceId);
#if 1
		if((pmtdata->mPmtId == cond.mPmtId) && (pmtdata->mServiceId == cond.mServiceId)) {
#else
		{
#endif
			eSectionReader().scan(demuxId, SectionType::PMT, list, pmtdata);
			break;
		}
	}
}

void eAITScanner::doScan(int demuxid, int pmtid, int serviceid)
{
	/* PAT Scan */
	ePMTStorageManager esm;
	doScanPAT(demuxid, esm);

	/* PMT Scan */
	ProgramMapSectionList pmts;
	ePMTData cond = ePMTData(pmtid, serviceid);
	doScanPMT(demuxid, esm, &pmts, cond);
	KDBG("PMT Size : %d", pmts.size());
#if 1
	/* AIT Scan */
	doScanAIT(demuxid, pmts);
#endif
}



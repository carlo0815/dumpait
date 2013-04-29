#include "klog.h"
#include "define.h"
#include "dvb_sections.h"

eSectionReader::eSectionReader()
{
}

eSectionReader::~eSectionReader()
{
}

void eSectionReader::push(SectionType::Type type, void *list, unsigned char* buffer)
{
	switch(type)
	{
	case SectionType::PAT: 
		((ProgramAssociationSectionList*)list)->push_back(new ProgramAssociationSection(buffer));
		break;
	case SectionType::PMT: 
		((ProgramMapSectionList*)list)->push_back(new ProgramMapSection(buffer));
		break;
	case SectionType::AIT:
		((ApplicationInformationSectionList*)list)->push_back(new ApplicationInformationSection(buffer));
		break;
	case SectionType::DSMCC:
		break;
	}
}

BOOL eSectionReader::scan(int demuxId, SectionType::Type type, void *list, void *param)
{
	switch(type)
	{
	case SectionType::PAT: 
		if(open(demuxId, eDVBPATSpec()) == 0) 
			return FALSE;
		break;
	case SectionType::PMT: 
		if(open(demuxId, eDVBPMTSpec(((ePMTData*)param)->mPmtId, ((ePMTData*)param)->mServiceId)) == 0) 
			return FALSE;
		break;
	case SectionType::AIT:
		if(open(demuxId, eDVBAITSpec(((eAITData*)param)->mAitId)) == 0)
			return FALSE;
		break;
	case SectionType::DSMCC:
		break;
	default:return FALSE;
	}

	for(int m_tries = 0; ; m_tries++) {
		//KDBG("try to read : %d", m_tries);
		int len = 0;
		unsigned char* buffer = read(len);
		if(buffer == NULL) {
			if(m_tries == 5)
				return FALSE;
			continue;
		}
		unsigned int section_number = buffer[6];
		unsigned int last_section_number = buffer[7];
		if (m_tries > 5*(last_section_number+1))
			break;
		if(!len || !buffer)
			break;
#if 1
		KHEX("SECTION", (char*)buffer, len);
#endif
		if(type == SectionType::AIT)
			((eAITData*)param)->set(buffer, len);
		else push(type, list, buffer);

		if(last_section_number == 0 && section_number == 0)
			break;
	}

	return TRUE;
}



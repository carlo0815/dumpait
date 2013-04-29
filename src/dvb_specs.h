#ifndef __DVB_SPECS_H__
#define __DVB_SPECS_H__ 1

#include <stdint.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>
#include <dvbsi++/program_map_section.h>
#include <dvbsi++/service_description_section.h>
#include <dvbsi++/network_information_section.h>
#include <dvbsi++/bouquet_association_section.h>
#include <dvbsi++/program_association_section.h>
#include <dvbsi++/event_information_section.h>
#include <dvbsi++/application_information_section.h>

struct eDVBSectionFilterMask
{
	int pid;
	/* mode is 0 for positive, 1 for negative filtering */
	__u8 data[DMX_FILTER_SIZE], mask[DMX_FILTER_SIZE], mode[DMX_FILTER_SIZE];
	enum {
		rfCRC=1,
		rfNoAbort=2
	};
	int flags;
};

struct eDVBTableSpec
{
	int pid, tid, tidext, tid_mask, tidext_mask;
	int version;
	int timeout;        /* timeout in ms */
	enum
	{
		tfInOrder=1,
		/*
			tfAnyVersion      filter ANY version
			0                 filter all EXCEPT given version (negative filtering)
			tfThisVersion     filter only THIS version
		*/
		tfAnyVersion=2,
		tfThisVersion=4,
		tfHaveTID=8,
		tfHaveTIDExt=16,
		tfCheckCRC=32,
		tfHaveTimeout=64,
		tfHaveTIDMask=128,
		tfHaveTIDExtMask=256
	};
	int flags;
};

struct eDVBDSMCCDLDataSpec
{
	eDVBTableSpec m_spec;
public:
	eDVBDSMCCDLDataSpec(int pid)
	{
		m_spec.pid     = pid;
		m_spec.tid     = TID_DSMCC_DL_DATA;
		m_spec.timeout = 20000;
		m_spec.flags   = eDVBTableSpec::tfAnyVersion |
			eDVBTableSpec::tfHaveTID | eDVBTableSpec::tfCheckCRC |
			eDVBTableSpec::tfHaveTimeout;
	}
	operator eDVBTableSpec &()
	{
		return m_spec;
	}
};

struct eDVBPMTSpec
{
	eDVBTableSpec m_spec;
public:
	eDVBPMTSpec(int pid, int sid, int timeout = 20000)
	{
		m_spec.pid     = pid;
		m_spec.tid     = ProgramMapSection::TID;
		m_spec.tidext  = sid;
		m_spec.timeout = timeout; // ProgramMapSection::TIMEOUT;
		m_spec.flags   = eDVBTableSpec::tfAnyVersion | 
			eDVBTableSpec::tfHaveTID | eDVBTableSpec::tfHaveTIDExt | 
			eDVBTableSpec::tfCheckCRC | eDVBTableSpec::tfHaveTimeout;
	}
	operator eDVBTableSpec &()
	{
		return m_spec;
	}
};

struct eDVBPATSpec
{
	eDVBTableSpec m_spec;
public:
	eDVBPATSpec(int timeout=20000)
	{
		m_spec.pid     = ProgramAssociationSection::PID;
		m_spec.tid     = ProgramAssociationSection::TID;
		m_spec.timeout = timeout; // ProgramAssociationSection::TIMEOUT;
		m_spec.flags   = eDVBTableSpec::tfAnyVersion |
			eDVBTableSpec::tfHaveTID | eDVBTableSpec::tfCheckCRC |
			eDVBTableSpec::tfHaveTimeout;
	}
	operator eDVBTableSpec &()
	{
		return m_spec;
	}
};

struct eDVBAITSpec
{
	eDVBTableSpec m_spec;
public:
	eDVBAITSpec(int pid)
	{
		m_spec.pid     = pid;
		m_spec.tid     = ApplicationInformationSection::TID;
		m_spec.timeout = ApplicationInformationSection::TIMEOUT;
		m_spec.flags   = eDVBTableSpec::tfAnyVersion |
			eDVBTableSpec::tfHaveTID | eDVBTableSpec::tfCheckCRC |
			eDVBTableSpec::tfHaveTimeout;
	}
	operator eDVBTableSpec &()
	{
		return m_spec;
	}
};

#endif /*__DVB_SPECS_H__*/


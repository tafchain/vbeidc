#ifndef _VBE_IDC_CC_ACTION_MSG_H_01237027697202
#define _VBE_IDC_CC_ACTION_MSG_H_01237027697202

#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"

#include "dsc/codec/dsc_codec/dsc_codec.h"


namespace VBE_IDC
{
	class CVbeIdcCcActionTransferMsg
	{
	public:
		DSC_BIND_ATTR(m_nTotalCoin, m_sponsorUserKey, m_lstUserKey, m_lstCoin);

	public:
		ACE_UINT32 m_nTotalCoin;
		DSC::CDscShortBlob m_sponsorUserKey;// vbe user key
		DSC::CDscShortList<DSC::CDscShortBlob> m_lstUserKey;	// vbe user key
		DSC::CDscShortList<ACE_UINT32> m_lstCoin;
	};

	class CVbeIdcCcActionUpdateMsg
	{
	public:
		DSC_BIND_ATTR(m_nUpdateType, m_sponsorUserKey, m_userInfo);

	public:
		ACE_UINT32 m_nUpdateType;
		DSC::CDscShortBlob m_sponsorUserKey;// vbe user key
		DSC::CDscShortBlob m_userInfo;
	};
}










#endif
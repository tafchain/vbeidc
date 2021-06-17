#include "vbe_idc_common_msg_wrapper.h"

namespace VBE_IDC
{
	CRegisterUserCltRegReqDataWrapper::CRegisterUserCltRegReqDataWrapper(ACE_INT32& nChannelID, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userInfo, DSC::CDscShortBlob& serverCryptKey)
		:m_nChannelID(nChannelID), m_nonce(nonce), m_userInfo(userInfo), m_serverCryptKey(serverCryptKey)
	{
	}

	CLoginCltVbeReqDataWrapper::CLoginCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce)
		: m_nonce(nonce)
	{
	}

	CLoginVbeCltRspDataWrapper::CLoginVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, ACE_UINT32& token)
		: m_nonce(nonce), m_token(token)
	{
	}

	CRegisterUserRegCltRspDataWrapper::CRegisterUserRegCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userKey)
		:m_nonce(nonce), m_userKey(userKey)
	{
	}

	CGetUserInfoCltVbeReqDataWrapper::CGetUserInfoCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce)
		: m_nonce(nonce)
	{
	}

	CGetUserInfoVbeCltRspDataWrapper::CGetUserInfoVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userinfo)
		: m_nonce(nonce), m_userinfo(userinfo)
	{
	}

	CUpdateCltVbeReqDataWrapper::CUpdateCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userInfo, ACE_UINT32& nActionID, ACE_UINT32& nUpdateType)
		: m_nonce(nonce), m_userInfo(userInfo), m_nActionID(nActionID), m_nUpdateType(nUpdateType)
	{
	}

	CUpdateVbeCltRspDataWrapper::CUpdateVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& transKey)
		: m_nonce(nonce), m_transKey(transKey)
	{
	}
	
	CIdcInterfaceCltVbeReqDataWrapper::CIdcInterfaceCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userInfo, ACE_UINT32& nInterfaceType)
		: m_nonce(nonce), m_userInfo(userInfo), m_nInterfaceType(nInterfaceType)
	{
	}

	CIdcInterfaceVbeCltRspDataWrapper::CIdcInterfaceVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscBlob& info)
		: m_nonce(nonce), m_info(info)
	{
	}

	CTransferCltVbeReqDataWrapper::CTransferCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce, ACE_UINT32& nTotalCoin, DSC::CDscShortList<DSC::CDscShortBlob>& userKeyList, DSC::CDscShortList<ACE_UINT32>& coinList)
		:m_nonce(nonce), m_nTotalCoin(nTotalCoin), m_userKeyList(userKeyList), m_coinList(coinList)
	{
	}

	CTransferVbeCltRspDataWrapper::CTransferVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& transKey)
		:m_nonce(nonce), m_transKey(transKey)
	{
	}
}

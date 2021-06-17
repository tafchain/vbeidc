#ifndef _VBE_IDC_COMMON_MSG_WRAPPER_H_10365718768910717
#define _VBE_IDC_COMMON_MSG_WRAPPER_H_10365718768910717

#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"
#include "vbh/common/vbh_encrypt_lib.h"
#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/container/dsc_array.h"


namespace VBE_IDC {
	template<typename WARPPER_TYPE>
	ACE_INT32 EncryptWrapperMsg(WARPPER_TYPE& warpperData, const DSC::CDscShortBlob& envolopeKey, char*& outBuf, ACE_UINT32& outLen);
	template<typename WARPPER_TYPE>
	char* DecryptWrapperMsg(WARPPER_TYPE& warpperData, const DSC::CDscShortBlob& envolopeKey, char* data, ACE_UINT32 dataLen);

	class   CLoginCltVbeReqDataWrapper
	{
	public:
		
	VBE_IDC_EXPORT	CLoginCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce);

	public:
		DSC_BIND_ATTR(m_nonce);

	public:
		DSC::CDscShortBlob& m_nonce;
	};

	class CLoginVbeCltRspDataWrapper
	{
	public:
		VBE_IDC_EXPORT CLoginVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, ACE_UINT32& token);

	public:
		DSC_BIND_ATTR(m_nonce, m_token);

	public:
		DSC::CDscShortBlob& m_nonce;
		ACE_UINT32& m_token;
	};

	class CRegisterUserCltRegReqDataWrapper
	{
	public:
		VBE_IDC_EXPORT CRegisterUserCltRegReqDataWrapper(ACE_INT32& nChannelID, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userInfo, DSC::CDscShortBlob& m_serverCryptKey);

	public:
		DSC_BIND_ATTR(m_nChannelID, m_nonce, m_userInfo, m_serverCryptKey);

	public:
		ACE_INT32 m_nChannelID;
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_userInfo;
		DSC::CDscShortBlob& m_serverCryptKey;
	};

	class CRegisterUserRegCltRspDataWrapper
	{
	public:
		VBE_IDC_EXPORT CRegisterUserRegCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userKey);

	public:
		DSC_BIND_ATTR(m_nonce, m_userKey);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_userKey;

	};

	class CUpdateCltVbeReqDataWrapper
	{
	public:
		VBE_IDC_EXPORT CUpdateCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userInfo, ACE_UINT32&  nActionID, ACE_UINT32& nUpdateType);

	public:
		DSC_BIND_ATTR(m_nonce, m_userInfo, m_nActionID, m_nUpdateType);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_userInfo;
		ACE_UINT32& m_nActionID;
		ACE_UINT32& m_nUpdateType;
	};

	class CUpdateVbeCltRspDataWrapper
	{
	public:
		VBE_IDC_EXPORT CUpdateVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& transKey);

	public:
		DSC_BIND_ATTR(m_nonce, m_transKey);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_transKey;
	};

	class CIdcInterfaceCltVbeReqDataWrapper
	{
	public:
		VBE_IDC_EXPORT CIdcInterfaceCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userInfo, ACE_UINT32& nInterfaceType);

	public:
		DSC_BIND_ATTR(m_nonce, m_userInfo, m_nInterfaceType);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_userInfo;
		ACE_UINT32& m_nInterfaceType;
	};

	class CIdcInterfaceVbeCltRspDataWrapper
	{
	public:
		VBE_IDC_EXPORT CIdcInterfaceVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscBlob& info);

	public:
		DSC_BIND_ATTR(m_nonce, m_info);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscBlob& m_info;
	};

	class CTransferCltVbeReqDataWrapper
	{
	public:
		VBE_IDC_EXPORT CTransferCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce, ACE_UINT32& nTotalCoin, DSC::CDscShortList<DSC::CDscShortBlob>& userKeyList,
			DSC::CDscShortList<ACE_UINT32>& coinList);
		
	public:
		DSC_BIND_ATTR(m_nonce, m_nTotalCoin, m_userKeyList, m_coinList);

	public:
		DSC::CDscShortBlob& m_nonce;
		ACE_UINT32 &m_nTotalCoin;
		DSC::CDscShortList<DSC::CDscShortBlob> &m_userKeyList;
		DSC::CDscShortList<ACE_UINT32> &m_coinList;
	};

	class CTransferVbeCltRspDataWrapper
	{
	public:
		VBE_IDC_EXPORT CTransferVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& transKey);

	public:
		DSC_BIND_ATTR(m_nonce, m_transKey);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_transKey;
	};

	class CGetUserInfoCltVbeReqDataWrapper
	{
	public:
		VBE_IDC_EXPORT CGetUserInfoCltVbeReqDataWrapper(DSC::CDscShortBlob& nonce);

	public:
		DSC_BIND_ATTR(m_nonce);

	public:
		DSC::CDscShortBlob& m_nonce;
	};

	class CGetUserInfoVbeCltRspDataWrapper
	{
	public:
		VBE_IDC_EXPORT CGetUserInfoVbeCltRspDataWrapper(DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userinfo);

	public:
		DSC_BIND_ATTR(m_nonce, m_userinfo);

	public:
		DSC::CDscShortBlob& m_nonce;
		DSC::CDscShortBlob& m_userinfo;
	};

#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg_wrapper.inl"
}

#endif

#ifndef _VBE_IDC_CLIENT_SDK_H_83107513465091471
#define _VBE_IDC_CLIENT_SDK_H_83107513465091471

#include "vbe_idc_client_sdk_export.h"

#include "ace/Basic_Types.h"
#include "dsc/dsc_export.h"
#include "dsc/codec/codec_base/dsc_codec_base.h"

namespace VBE_IDC_SDK
{
	class CTransferReceiver
	{
	public:
		ACE_UINT32 m_nCoin;
		DSC::CDscShortBlob m_userKey;
	};

	class IClientSdkMsgCallback
	{
	public:
		virtual void OnReady(void) {}
		virtual void OnAbnormal(void) {}
		virtual void OnExit(void) {}
	public:
		virtual void OnRegisterUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userKey = nullptr) {}
		virtual void OnLoginResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 token = 0){}
		virtual void OnGetUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nQueryType, DSC::CDscShortBlob* userinfo = nullptr) {}
		virtual void OnUpdateUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nUpdateType, DSC::CDscShortBlob* userinfo = nullptr) {}
		virtual void OnCommonResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nInterfaceType, DSC::CDscShortBlob* userinfo = nullptr) {}
		//virtual void OnGetTransInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userinfo = nullptr) {}
		virtual void OnTransferResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* transKey = nullptr){}
	};

	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 CreateIdcCryptKey(char*& pszCltKey, size_t& nCltKeyLen, char*& pszSvrKey, size_t& nSvrKeyLen);

	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 Init(const ACE_INT16 nAppID, IClientSdkMsgCallback* pCallBack);
	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 Deinit();

	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 RegisterUser(ACE_UINT32 nReqID, const char* userInfo, ACE_INT64 nLen, const char* serverCryptKey, ACE_INT64 nKeyLen);
	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 Login(ACE_UINT32 nReqID, const char* userKey, ACE_INT64 nUserKeyLen, const char* cltKey, ACE_INT64 nCltKeyLen);
	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 GetUserInfo(ACE_UINT32 nReqID, ACE_UINT32 nToken, ACE_UINT32 nQueryType);
	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 UpdateIdcUserInfo(ACE_UINT32 nReqID, ACE_UINT32 nToken, ACE_UINT32 nActionID, ACE_UINT32 nUpdateType, const char* userInfo, ACE_INT64 nLen);
	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 OperationIdcInterface(ACE_UINT32 nReqID, ACE_UINT32 nToken, ACE_UINT32 nUpdateType, const char* userInfo, ACE_INT64 nLen);
	VBE_IDC_CLIENT_SDK_EXPORT ACE_INT32 Transfer(ACE_UINT32 nReqID, ACE_UINT32 nToken, DSC::CDscShortList<CTransferReceiver> &lstReceiver);
}


#endif

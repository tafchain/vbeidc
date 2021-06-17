#include <cstddef>

#include "vbe_idc/vbe_idc_client_sdk/vbe_idc_client_sdk.h"
#include "vbe_idc/vbe_idc_client_c_sdk/vbe_idc_client_c_api.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg.h"

class CWrapperCallback : public VBE_IDC_SDK::IClientSdkMsgCallback
{

public:
	virtual void OnReady(void);
	virtual void OnAbnormal(void);
	virtual void OnExit(void); 
	virtual void OnRegisterUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userKey = nullptr);
	virtual void OnLoginResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 token = 0);
	virtual void OnGetUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nQueryType, DSC::CDscShortBlob* userinfo = nullptr);
	virtual void OnUpdateUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nUpdateType, DSC::CDscShortBlob* userinfo = nullptr);
	virtual void OnCommonResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nInterfaceType, DSC::CDscShortBlob* userinfo = nullptr);
	virtual void OnTransferResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* transKey = nullptr);
	//virtual void OnGetTransInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userinfo = nullptr) {}

public:
	on_ready m_pOnReady;
	on_abnormal m_pOnAbnormal;
	on_exit m_pOnExit;
	on_regist_idc_user_rsp m_pOnRegistUserRsp;
	on_login_idc_rsp m_pOnLoginIdcRsp;
	on_query_idc_info_rsp m_pOnQueryUserInfoRsp;
	on_update_user_info_rsp m_pOnUpdateRsp;
	on_common_rsp m_pOnCommonRsp;
};

int create_crypt_key(char** ppCltKey, unsigned int* pnCltKeyLen, char** ppSvrKey, unsigned int* pnSvrKeyLen)
{
	char* pCltKey;
	size_t nTempCltKeyLen;
	char* pSvrKey;
	size_t nTempSvrKeyLen;

	int32_t nRet = VBE_IDC_SDK::CreateIdcCryptKey(pCltKey, nTempCltKeyLen, pSvrKey, nTempSvrKeyLen);
	
	*ppCltKey = pCltKey;
	*pnCltKeyLen = nTempCltKeyLen;
	*ppSvrKey = pSvrKey;
	*pnSvrKeyLen = nTempSvrKeyLen;

	return nRet;
}


//初始化
int init_vbe_idc_c_sdk(
	on_ready pOnReady,
	on_abnormal pOnAbnormal,
	on_exit pOnExit,
	on_regist_idc_user_rsp pOnRegistUserRsp,
	on_login_idc_rsp pOnLoginIdcRsp,
	on_query_idc_info_rsp pOnQueryUserInfoRsp,
	on_update_user_info_rsp pOnUpdateRsp,
	on_common_rsp pOnCommonRsp)
{
	if (pOnReady && pOnAbnormal && pOnExit &&
		pOnRegistUserRsp && pOnLoginIdcRsp && pOnQueryUserInfoRsp && 
		pOnUpdateRsp && pOnCommonRsp)

	{
		CWrapperCallback* pWrapperCallback = new CWrapperCallback;

		pWrapperCallback->m_pOnReady = pOnReady;
		pWrapperCallback->m_pOnAbnormal = pOnAbnormal;
		pWrapperCallback->m_pOnExit = pOnExit;
		pWrapperCallback->m_pOnRegistUserRsp = pOnRegistUserRsp;
		pWrapperCallback->m_pOnLoginIdcRsp = pOnLoginIdcRsp;
		pWrapperCallback->m_pOnQueryUserInfoRsp = pOnQueryUserInfoRsp;
		pWrapperCallback->m_pOnUpdateRsp = pOnUpdateRsp;
		pWrapperCallback->m_pOnCommonRsp = pOnCommonRsp;

		return Init(9527, pWrapperCallback);
	}
	else
	{
		return -1;
	}
}

//注册用户
int reg_idc_user(const unsigned int nRequestID,
	char* pUserInfo, const unsigned int nUserInfoLen,
	char* pServerCryptKey, const unsigned int nServerCryptLen)
{
	return VBE_IDC_SDK::RegisterUser(nRequestID, pUserInfo, nUserInfoLen, pServerCryptKey, nServerCryptLen);
}

int Login(const unsigned int nReqID, const char* userKey,
	unsigned long long  nUserKeyLen, const char* cltKey, unsigned long long nCltKeyLen)
{
	return VBE_IDC_SDK::Login(nReqID, userKey, nUserKeyLen, cltKey, nCltKeyLen);
}

int query_idc_user(const unsigned int nRequestID, const unsigned int nToken)
{
	return VBE_IDC_SDK::GetUserInfo(nRequestID, nToken, VBE_IDC::EN_IDC_QUERY_USER_INFO_TYPE);
}

int auth_idc_user(const unsigned int nRequestID, const unsigned int nToken)
{
	return VBE_IDC_SDK::GetUserInfo(nRequestID, nToken, VBE_IDC::EN_IDC_AUTH_USER_TYPE);
}

int update_idc_user_info(const unsigned int nRequestID, const unsigned int nToken,
	char* pUserInfo, const unsigned int nUserInfoLen)
{
	return VBE_IDC_SDK::UpdateIdcUserInfo(nRequestID, nToken, 1, VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, pUserInfo, nUserInfoLen);
}

int idcQueryUserAsset(const unsigned int nRequestID, const unsigned int nToken)
{
	return VBE_IDC_SDK::GetUserInfo(nRequestID, nToken, VBE_IDC::EN_IDC_QUERY_USER_ASSET_TYPE);
}

int idcReceiveBonus(const unsigned int nRequestID, const unsigned int nToken, double dNumBonus)
{
	if (dNumBonus < 0.0001)
	{
		//转账不能为负数
		return -2;
	}
	double realNums = floor(dNumBonus * 10000.00 + 0.5) / 10000;

	char info[20] = { 0 };
	sprintf(info, "%lf", realNums);
	return VBE_IDC_SDK::UpdateIdcUserInfo(nRequestID, nToken, 1, VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE, info, strlen(info));
}

int idcAddForce(const unsigned int nRequestID, const unsigned int nToken, int nForce, int nType)
{
	if (nForce <= 0)
	{
		return -2;
	}

	VBE_IDC::CIDCAddForce addForce;
	addForce.m_nForce = nForce;
	addForce.m_nType = nType;

	char* pProposal = NULL;
	size_t nProposalLen;

	DSC::Encode(addForce, pProposal, nProposalLen);
	if (pProposal)
	{
		int ret = VBE_IDC_SDK::UpdateIdcUserInfo(nRequestID, nToken, 1, VBE_IDC::EN_IDC_ADD_FORCE_TYPE, pProposal, nProposalLen);
		DSC_THREAD_SIZE_FREE(pProposal, nProposalLen);
		return ret;
	}
	else
	{
		DSC_RUN_LOG_INFO("idcAddForce  Encode pProposal failed");
		return -1;
	}
}

int idcBet2EarlyBird(const unsigned int nRequestID, const unsigned int nToken)
{
	return VBE_IDC_SDK::UpdateIdcUserInfo(nRequestID, nToken, 1, VBE_IDC::EN_IDC_ADD_TO_EARLY_BIRD_TYPE, NULL, 0);
}

VBE_IDC_CLIENT_C_SDK_EXPORT int idcTransferIDM(const unsigned int nRequestID, const unsigned int nToken,
	char* pTargetKey, const unsigned int nTargetKeyLen, double dIDMNum)
{
	if (dIDMNum < 0.0001)
	{
		//转账不能为负数
		return -2;
	}
	double realNums = floor(dIDMNum * 10000.00 + 0.5) / 10000;

	VBE_IDC::CTransInfoCltIdc transInfo;
	transInfo.m_dIDMNum = realNums;
	transInfo.m_strTargetKey.assign(pTargetKey, nTargetKeyLen);

	char* pProposal = NULL;
	size_t nProposalLen;

	DSC::Encode(transInfo, pProposal, nProposalLen);
	if (pProposal)
	{
		int ret = VBE_IDC_SDK::UpdateIdcUserInfo(nRequestID, nToken, 2, VBE_IDC::EN_IDC_TRANS_TYPE, pProposal, nProposalLen);
		DSC_THREAD_SIZE_FREE(pProposal, nProposalLen);
		return ret;
	}
	else
	{
		DSC_RUN_LOG_INFO("idcAddForce  Encode pProposal failed");
		return -1;
	}
}

int iDomShareInfo(const unsigned int nRequestID, const unsigned int nToken,
	char* pSharerSign, const unsigned int nSharerSignLen, char* pShareInfo, const unsigned int nShareInfoLen)
{
	VBE_IDC::CShareInfoReqComm req;
	req.m_sharerSign.Set(pSharerSign, nSharerSignLen);
	req.m_shareInfo.Set(pShareInfo, nShareInfoLen);

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_SHARE_INFO, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomDownloadShareInfo(const unsigned int nRequestID, const unsigned int nToken,
	char* pViewerSign, const unsigned int nViewerSignLen, char* pShareInfoUrl, const unsigned int nShareInfoUrlLen)
{
	VBE_IDC::CDownLoadShareInfoReq req;

	req.m_viewerSign.Set(pViewerSign, nViewerSignLen);
	req.m_shareInfoUrl = atoi(pShareInfoUrl);

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_DOWNLOAD_SHARE_INFO, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

//删除分享信息
int iDomDeleteShareInfo(const unsigned int nRequestID, const unsigned int nToken,
	char* pShareInfoUrl, const unsigned int nShareInfoUrlLen)
{
	VBE_IDC::CDeleteShareInfoReq req;

	req.m_shareInfoUrl = atoi(pShareInfoUrl);

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_DELETE_SHARE_INFO, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomQueryShareHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
	int nQueryType, int nStartIndex, int nEndIndex)
{
	VBE_IDC::CQueryShareHistoryInfoReq req;

	req.m_nStartIndex = nStartIndex;
	req.m_nEndIndex = nEndIndex;
	req.m_nQueryType = nQueryType;

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_QUERY_SHARE_HISTORY, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomGetAssetHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
	int nStartIndex, int nEndIndex, int nCoinType, int nRecordType)
{
	VBE_IDC::CGetAssetHistoryInfoReq req;

	req.m_nStartIndex = nStartIndex;
	req.m_nEndIndex = nEndIndex;
	req.m_nCoinType = nCoinType;
	req.m_nRecordType = nRecordType;

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_GET_ASSET_HISTORY, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomGetForceHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
	int nStartIndex, int nEndIndex)
{
	VBE_IDC::CGetForceHistoryInfoReq req;

	req.m_nStartIndex = nStartIndex;
	req.m_nEndIndex = nEndIndex;

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_GET_FORCE_HISTORY, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomEarlyBirdSign(const unsigned int nRequestID, const unsigned int nToken)
{
	VBE_IDC::CEarlyBirdSignReq req;

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_EARLY_BIRD_SIGN, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomGetEarlyBirdHistory(const unsigned int nRequestID, const unsigned int nToken,
	char* pQueryTime, const unsigned int nQueryTimeLen)
{
	VBE_IDC::CGetEarlyBirdHistoryReq req;

	req.m_queryTime.Set(pQueryTime, nQueryTimeLen);

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_GET_EARLY_BIRD_HISTORY, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomGetEarlyBirdPeopleCount(const unsigned int nRequestID, const unsigned int nToken,
	char* pQueryTime, const unsigned int nQueryTimeLen)
{
	VBE_IDC::CGetEarlyBirdPeopleCountReq req;

	req.m_queryTime.Set(pQueryTime, nQueryTimeLen);

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_GET_EARLY_BIRD_PEOPLE_COUNT, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomGetEarlyBirdRankList(const unsigned int nRequestID, const unsigned int nToken,
	char* pQueryTime, const unsigned int nQueryTimeLen, const unsigned int nNum)
{
	VBE_IDC::CGetEarlyBirdRankListReq req;

	req.m_queryTime.Set(pQueryTime, nQueryTimeLen);
	req.m_nNum = nNum;

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_GET_EARLY_BIRD_RANK_LIST, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomIfEarlyBirdSign(const unsigned int nRequestID, const unsigned int nToken)
{
	VBE_IDC::CIfEarlyBirdSignReq req;

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_IF_EARLY_BIRD, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

int iDomGetSignHistoryByMonth(const unsigned int nRequestID, const unsigned int nToken,
	char* pQueryTime, const unsigned int nQueryTimeLen)
{
	VBE_IDC::CGetSignHistoryReq req;

	req.m_queryMonth.Set(pQueryTime, nQueryTimeLen);

	char* info = NULL;
	size_t infoLen = 0;

	DSC::Encode(req, info, infoLen);

	if (info)
	{
		int ret = VBE_IDC_SDK::OperationIdcInterface(nRequestID, nToken, VBE_IDC::EN_IDC_INTERFACE_GET_SIGN_HISTORY, info, infoLen);

		DSC_THREAD_SIZE_FREE(info, infoLen);

		return ret;
	}
	else
	{
		return -1;
	}
}

//int query_vbh_trans(const uint32_t nChannelID, const uint32_t nRequestID,
//	char* pTransKey, const uint32_t nTransKeyLen, /*要查询的trans-key*/ 
//	char* pUserKey, const uint32_t nUserKeyLen, /*用户key*/ 
//	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/)
//{
//	if (pUserCryptKey && nUserCryptKeyLen > 0
//		&& pTransKey && nTransKeyLen > 0)
//	{
//		VBH_SDK::CGetTransactionInfoReq req;
//
//		req.m_pUserCryptKey = pUserCryptKey;
//		req.m_nUserCryptKeyLen = nUserCryptKeyLen;
//
//		req.m_pTransKey = pTransKey;
//		req.m_nTransKeyLen = nTransKeyLen;
//
//		return VBH_SDK::QueryTransInfo(req);
//	}
//	else
//	{
//		return -1;
//	}
//}

void CWrapperCallback::OnReady(void)
{
	this->m_pOnReady();
}

void CWrapperCallback::OnAbnormal(void)
{
	this->m_pOnAbnormal();
}

void CWrapperCallback::OnExit(void)
{
	this->m_pOnExit();
}

void CWrapperCallback::OnRegisterUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userKey)
{
	this->m_pOnRegistUserRsp(nReturnCode, nRequestID, userKey->GetBuffer(), userKey->GetSize());
}

void CWrapperCallback::OnLoginResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 token)
{
	this->m_pOnLoginIdcRsp(nReturnCode, nRequestID, token);
}

void CWrapperCallback::OnGetUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nQueryType, DSC::CDscShortBlob* userinfo)
{
	this->m_pOnQueryUserInfoRsp(nReturnCode, nRequestID, nQueryType, userinfo->GetBuffer(), userinfo->GetSize());
}

void CWrapperCallback::OnUpdateUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nUpdateType, DSC::CDscShortBlob* userinfo)
{
	this->m_pOnUpdateRsp(nReturnCode, nRequestID, nUpdateType, userinfo->GetBuffer(), userinfo->GetSize());
}

void CWrapperCallback::OnCommonResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nInterfaceType, DSC::CDscShortBlob* userinfo)
{
	this->m_pOnCommonRsp(nReturnCode, nRequestID, nInterfaceType, userinfo->GetBuffer(), userinfo->GetSize());
}

void CWrapperCallback::OnTransferResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* transKey)
{

}

//virtual void OnGetTransInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userinfo = nullptr) {}

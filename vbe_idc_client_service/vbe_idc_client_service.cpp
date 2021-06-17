
#include "vbe_idc/vbe_idc_client_service/vbe_idc_client_service.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg_wrapper.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_user_util.h"

#include "vbh/common/vbh_comm_func.h"
#include "vbh/common/vbh_comm_error_code.h"

#include "dsc/mem_mng/dsc_allocator.h"
#include "dsc/configure/dsc_configure.h"


ACE_INT32 CVbeIdcClientService::OnInit(void)
{
	if (CDscHtsClientService::OnInit())
	{
		DSC_RUN_LOG_ERROR("bc user client service init failed!");

		return -1;
	}

	if (this->LoadPeerInfo())
	{
		DSC_RUN_LOG_WARNING("load peer info failed");

		return -1;
	}

	CDscString key;
	//读取peer的公钥，用于非对称加密通信
	if (VBH::GetVbhProfileString("PEER_ENVELOPE_KEY", key))
	{
		DSC_RUN_LOG_WARNING("cann't read 'PEER_ENVELOPE_KEY' configure item value");

		return -1;
	}
	if (key.empty())
	{
		DSC_RUN_LOG_WARNING("'PEER_ENVELOPE_KEY' cann't be empty");

		return -1;
	}

	m_peerEnvelopeKey.AllocBuffer(key.size());
	memcpy(m_peerEnvelopeKey.c_str(), key.data(), key.size());

	ACE_OS::srand(ACE_OS::time(0));
	m_seed = ACE_OS::time(0);

	m_bReady = true;

	return 0;
}

ACE_INT32 CVbeIdcClientService::OnExit(void)
{
	return 0;
}



void CVbeIdcClientService::OnDscMsg(VBE_IDC::CInitSdk& rInitSdk, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	if (m_pCallback)
	{
		DSC_RUN_LOG_WARNING("repleat init regist user msg");
	}
	else
	{
		m_pCallback = (VBE_IDC_SDK::IClientSdkMsgCallback*)rInitSdk.m_pCallBack;

		if (m_bReady)
		{
			m_pCallback->OnReady();
		}
	}
}


ACE_INT32 CVbeIdcClientService::OnHtsMsg(VBE_IDC::CRegisterUserRegCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler)
{
	auto pSession = (CRegisterUserSession*)EraseSession(m_mapRegisterUserSession, rRegistUserRsp.m_nSrcRequestID);

	if (pSession)
	{
		ACE_INT32 nReturnCode = rRegistUserRsp.m_nReturnCode;
		DSC::CDscShortBlob userKey;

		if (rRegistUserRsp.m_nReturnCode == VBH::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CRegisterUserRegCltRspDataWrapper wrapper(nonce, userKey);

			auto decryptBuf = DecryptWrapperMsg(wrapper, m_peerEnvelopeKey, rRegistUserRsp.m_data.c_str(), rRegistUserRsp.m_data.size());

			if (decryptBuf)
			{
				if (nonce == pSession->m_nonce)
				{
				}
				else
				{
					nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
				}
			}
			else
			{
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
			}
		}
		
		OnRegisterUserResponse(nReturnCode, pSession->m_nSrcSessionID, &userKey);
		DSC_THREAD_TYPE_DELETE(pSession);

		return 0;
	}

	return -1;
}

ACE_INT32 CVbeIdcClientService::OnHtsMsg(VBE_IDC::CLoginVbeCltRsp& rLoginRsp, CMcpHandler* pMcpHandler)
{
	auto pSession = (CLoginSession*)EraseSession(m_mapLoginSession, rLoginRsp.m_nSrcRequestID);
	
	if (pSession)
	{
		ACE_INT32 nReturnCode = rLoginRsp.m_nReturnCode;
		ACE_UINT32 nToken = 0;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CLoginVbeCltRspDataWrapper wrapper(nonce, nToken);

			auto decryptBuf = DecryptWrapperMsg(wrapper, pSession->m_cryptUserKey, rLoginRsp.m_data.c_str(), rLoginRsp.m_data.size());

			if (decryptBuf)
			{
				if (nonce == pSession->m_nonce)
				{
					auto onlineSession = DSC_THREAD_TYPE_NEW(COnlineUser) COnlineUser(*this);

					onlineSession->m_nVbeToken = nToken;
					onlineSession->m_nToken = NewToken();
					onlineSession->m_cryptUserKey.Clone(pSession->m_cryptUserKey);
					onlineSession->m_userKey.Clone(pSession->m_userKey);
					onlineSession->m_VbeServer = pSession->m_VbeServer;

					m_mapOnlineUsers.Insert(onlineSession->m_nToken, onlineSession);

					nToken = onlineSession->m_nToken;
				}
				else
				{
					nReturnCode = VBH::EN_NONCE_VERIFY_ERROR_TYPE;
				}

				DSC_THREAD_FREE(decryptBuf);
			}
			else
			{
				nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
			}
		}

		OnLoginResponse(nReturnCode, pSession->m_nSrcSessionID, nToken);
		DSC_THREAD_TYPE_DELETE(pSession);
	}

	return 0;
}

ACE_INT32 CVbeIdcClientService::OnHtsMsg(VBE_IDC::CTransferVbeCltRsp& rTransferRsp, CMcpHandler* pMcpHandler)
{
	auto pTransferSession = (CTransferSession*)EraseSession(m_mapTransferSession, rTransferRsp.m_nSrcRequestID);
	ACE_INT32 nReturnCode = rTransferRsp.m_nReturnCode;

	if (pTransferSession)
	{
		DSC::CDscShortBlob transKey;
		char* decryptBuf = nullptr;

		if (rTransferRsp.m_nReturnCode == VBE_IDC::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CTransferVbeCltRspDataWrapper wrapper(nonce, transKey);

			decryptBuf = DecryptWrapperMsg(wrapper, pTransferSession->m_pOnlineUser->m_cryptUserKey, rTransferRsp.m_data.c_str(), rTransferRsp.m_data.size());
			if (decryptBuf)
			{
				if (nonce == pTransferSession->m_nonce)
				{
				}
				else
				{
					nReturnCode = VBE_IDC::EN_NONCE_ERROR_TYPE;
				}
			}
			else
			{
				nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
			}
		}

		OnTransferResponse(nReturnCode, rTransferRsp.m_nSrcRequestID, &transKey);
		
		if (decryptBuf)
		{
			DSC_THREAD_FREE(decryptBuf);
		}

		DSC_THREAD_TYPE_DELETE(pTransferSession);
	}

	return 0;
}

void CVbeIdcClientService::OnDscMsg(VBE_IDC::CRegisterUserApiCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	
	auto pRegisterUserSession = DSC_THREAD_TYPE_NEW(CRegisterUserSession) CRegisterUserSession(*this);

	pRegisterUserSession->m_nChannelID = rRegistUserReq.m_nChannelID;
	pRegisterUserSession->m_nSrcSessionID = rRegistUserReq.m_nSrcRequestID;
	pRegisterUserSession->m_waitingSend = true;
	pRegisterUserSession->m_userInfo.Clone(rRegistUserReq.m_userInfo);
	pRegisterUserSession->m_serverCryptKey.Clone(rRegistUserReq.m_serverCryptKey);

	InsertSession(m_mapRegisterUserSession, pRegisterUserSession);

	if (m_pRegServer->m_bConnected)
	{
		SendRequest(pRegisterUserSession);
	}
	else
	{
		ConnectRegServer();
	}
}

void CVbeIdcClientService::OnDscMsg(VBE_IDC::CLoginApiCltReq& rLoginReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	DSC::CDscShortBlob vbhUserKey;
	ACE_UINT32 nChannelID;

	if (VBE_IDC::CVbeUserUtil::DecodeVbeUserKey(vbhUserKey, nChannelID, rLoginReq.m_userKey))
	{
		OnLoginResponse(VBH::EN_DECODE_ERROR_TYPE, rLoginReq.m_nSrcRequestID);

		return;
	}

	auto vbeServer = m_mapVbePeers.Find(nChannelID);

	if (!vbeServer)
	{
		OnLoginResponse(VBH::EN_SYSTEM_ERROR_TYPE, rLoginReq.m_nSrcRequestID);

		return;
	}

	auto pSession = DSC_THREAD_TYPE_NEW(CLoginSession) CLoginSession(*this);

	pSession->m_nSrcSessionID = rLoginReq.m_nSrcRequestID;
	pSession->m_waitingSend = true;
	pSession->m_userKey.Clone(rLoginReq.m_userKey);
	pSession->m_cryptUserKey.Clone(rLoginReq.m_cryptUserKey);
	pSession->m_VbeServer = vbeServer;

	InsertSession(m_mapLoginSession, pSession);

	if (vbeServer->m_bConnected)
	{
		SendRequest(pSession);
	}
	else
	{
		ConnectVbeServer(vbeServer);
	}
	
}

ACE_INT32 CVbeIdcClientService::OnHtsMsg(VBE_IDC::CGetUserInfoVbeCltRsp& rGetUserInfoRsp, CMcpHandler* pMcpHandler)
{
	auto pGetUserInfoSession = (CGetUserInfoSession*)EraseSession(m_mapGetUserInfoSession, rGetUserInfoRsp.m_nSrcRequestID);
	ACE_INT32 nReturnCode = rGetUserInfoRsp.m_nReturnCode;

	if (pGetUserInfoSession)
	{
		DSC::CDscShortBlob userinfo;
		char* decryptBuf = nullptr;

		if (rGetUserInfoRsp.m_nReturnCode == VBE_IDC::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CGetUserInfoVbeCltRspDataWrapper wrapper(nonce, userinfo);

			decryptBuf = DecryptWrapperMsg(wrapper, pGetUserInfoSession->m_pOnlineUser->m_cryptUserKey, rGetUserInfoRsp.m_data.c_str(), rGetUserInfoRsp.m_data.size());
			if (decryptBuf)
			{
				if (nonce == pGetUserInfoSession->m_nonce)
				{
				}
				else
				{
					nReturnCode = VBE_IDC::EN_NONCE_ERROR_TYPE;
				}
			}
			else
			{
				nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
			}
		}

		OnGetUserInfoResponse(nReturnCode, rGetUserInfoRsp.m_nSrcRequestID, pGetUserInfoSession->m_nQueryType, &userinfo);

		if (decryptBuf)
		{
			DSC_THREAD_FREE(decryptBuf);
		}

		DSC_THREAD_TYPE_DELETE(pGetUserInfoSession);
	}
	return 0;
}

ACE_INT32 CVbeIdcClientService::OnHtsMsg(VBE_IDC::CIdcInterfaceVbeCltRsp& rIdcInterfaceVbeCltRsp, CMcpHandler* pMcpHandler)
{
	auto pIdcInterfaceSession = (CIdcInterfaceSession*)EraseSession(m_mapIdcInterfaceSession, rIdcInterfaceVbeCltRsp.m_nSrcRequestID);
	ACE_INT32 nReturnCode = rIdcInterfaceVbeCltRsp.m_nReturnCode;

	if (pIdcInterfaceSession)
	{
		DSC::CDscShortBlob updateKey;
		char* decryptBuf = nullptr;

		if (rIdcInterfaceVbeCltRsp.m_nReturnCode == VBE_IDC::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CUpdateVbeCltRspDataWrapper wrapper(nonce, updateKey);

			decryptBuf = DecryptWrapperMsg(wrapper, pIdcInterfaceSession->m_pOnlineUser->m_cryptUserKey, rIdcInterfaceVbeCltRsp.m_data.c_str(), rIdcInterfaceVbeCltRsp.m_data.size());
			if (decryptBuf)
			{
				if (nonce == pIdcInterfaceSession->m_nonce)
				{
				}
				else
				{
					nReturnCode = VBE_IDC::EN_NONCE_ERROR_TYPE;
				}
			}
			else
			{
				nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
			}
		}

		OnUpdateResponse(nReturnCode, rIdcInterfaceVbeCltRsp.m_nSrcRequestID, pIdcInterfaceSession->m_nInterfaceType, &updateKey);

		if (decryptBuf)
		{
			DSC_THREAD_FREE(decryptBuf);
		}

		DSC_THREAD_TYPE_DELETE(pIdcInterfaceSession);
	}
	return 0;
}

ACE_INT32 CVbeIdcClientService::OnHtsMsg(VBE_IDC::CUpdateVbeCltRsp& rUpdateVbeCltRsp, CMcpHandler* pMcpHandler)
{
	auto pUpdateSession = (CUpdateSession*)EraseSession(m_mapUpdateSession, rUpdateVbeCltRsp.m_nSrcRequestID);
	ACE_INT32 nReturnCode = rUpdateVbeCltRsp.m_nReturnCode;

	if (pUpdateSession)
	{
		DSC::CDscShortBlob updateKey;
		char* decryptBuf = nullptr;

		if (rUpdateVbeCltRsp.m_nReturnCode == VBE_IDC::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CUpdateVbeCltRspDataWrapper wrapper(nonce, updateKey);

			decryptBuf = DecryptWrapperMsg(wrapper, pUpdateSession->m_pOnlineUser->m_cryptUserKey, rUpdateVbeCltRsp.m_data.c_str(), rUpdateVbeCltRsp.m_data.size());
			if (decryptBuf)
			{
				if (nonce == pUpdateSession->m_nonce)
				{
				}
				else
				{
					nReturnCode = VBE_IDC::EN_NONCE_ERROR_TYPE;
				}
			}
			else
			{
				nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
			}
		}

		OnUpdateResponse(nReturnCode, rUpdateVbeCltRsp.m_nSrcRequestID, pUpdateSession->m_nUpdateType, &updateKey);

		if (decryptBuf)
		{
			DSC_THREAD_FREE(decryptBuf);
		}

		DSC_THREAD_TYPE_DELETE(pUpdateSession);
	}
	return 0;
}

void CVbeIdcClientService::OnDscMsg(VBE_IDC::CIDCCommonApiCltReq& rIDCCommonApiCltReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto onlineUser = m_mapOnlineUsers.Find(rIDCCommonApiCltReq.m_nToken);

	if (onlineUser)
	{
		auto idcInterfaceSession = DSC_THREAD_TYPE_NEW(CIdcInterfaceSession) CIdcInterfaceSession(*this);
		VBE_IDC::CIdcInterfaceCltVbeReqDataWrapper wrapper(idcInterfaceSession->m_nonce, rIDCCommonApiCltReq.m_userInfo, rIDCCommonApiCltReq.m_nReqType);
		VBE_IDC::CIdcInterfaceCltVbeReq req;

		idcInterfaceSession->m_pOnlineUser = onlineUser;
		idcInterfaceSession->m_nInterfaceType = rIDCCommonApiCltReq.m_nReqType;
		this->MakeNonce(idcInterfaceSession->m_nonce);
		req.m_nSrcRequestID = rIDCCommonApiCltReq.m_nReqID;
		req.m_nToken = onlineUser->m_nVbeToken;

		auto nReturnCode = EncryptSendRequest(req, wrapper, onlineUser->m_cryptUserKey, onlineUser->m_VbeServer->m_nHandleID);

		if (nReturnCode)
		{
			OnIdcInterfaceResponse(nReturnCode, rIDCCommonApiCltReq.m_nReqType,
				rIDCCommonApiCltReq.m_nReqType);
		}
		else
		{
			InsertSession(m_mapIdcInterfaceSession, idcInterfaceSession);
		}
	}
	else
	{
		OnIdcInterfaceResponse(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rIDCCommonApiCltReq.m_nReqID, rIDCCommonApiCltReq.m_nReqType);
	}
}

void CVbeIdcClientService::OnDscMsg(VBE_IDC::CUpdateUserInfoApiCltReq& rUpdateUserInfoApiCltReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto onlineUser = m_mapOnlineUsers.Find(rUpdateUserInfoApiCltReq.m_nToken);

	if (onlineUser)
	{
		auto updateSession = DSC_THREAD_TYPE_NEW(CUpdateSession) CUpdateSession(*this);
		VBE_IDC::CUpdateCltVbeReqDataWrapper wrapper(updateSession->m_nonce, rUpdateUserInfoApiCltReq.m_userInfo, 
			rUpdateUserInfoApiCltReq.m_nActionID, rUpdateUserInfoApiCltReq.m_nUpdateType);
		VBE_IDC::CUpdateCltVbeReq req;

		updateSession->m_pOnlineUser = onlineUser;
		updateSession->m_nUpdateType = rUpdateUserInfoApiCltReq.m_nUpdateType;
		this->MakeNonce(updateSession->m_nonce);
		req.m_nSrcRequestID = rUpdateUserInfoApiCltReq.m_nSrcRequestID;
		req.m_nToken = onlineUser->m_nVbeToken;

		auto nReturnCode = EncryptSendRequest(req, wrapper, onlineUser->m_cryptUserKey, onlineUser->m_VbeServer->m_nHandleID);

		if (nReturnCode)
		{
			OnUpdateResponse(nReturnCode, rUpdateUserInfoApiCltReq.m_nSrcRequestID, 
				rUpdateUserInfoApiCltReq.m_nUpdateType);
		}
		else
		{
			InsertSession(m_mapUpdateSession, updateSession);
		}
	}
	else
	{
		OnUpdateResponse(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rUpdateUserInfoApiCltReq.m_nSrcRequestID, rUpdateUserInfoApiCltReq.m_nUpdateType);
	}
}

void CVbeIdcClientService::OnDscMsg(VBE_IDC::CTransferApiCltReq& rTransferReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto onlineUser = m_mapOnlineUsers.Find(rTransferReq.m_nToken);

	if (onlineUser)
	{
		auto transferSession = DSC_THREAD_TYPE_NEW(CTransferSession) CTransferSession(*this);
		VBE_IDC::CTransferCltVbeReqDataWrapper wrapper(transferSession->m_nonce, rTransferReq.m_nTotalCoin, rTransferReq.m_userKeyList, rTransferReq.m_coinList);
		VBE_IDC::CTransferCltVbeReq req;

		transferSession->m_pOnlineUser = onlineUser;
		this->MakeNonce(transferSession->m_nonce);
		req.m_nSrcRequestID = rTransferReq.m_nSrcRequestID;
		req.m_nToken = onlineUser->m_nVbeToken;
		
		auto nReturnCode = EncryptSendRequest(req, wrapper, onlineUser->m_cryptUserKey, onlineUser->m_VbeServer->m_nHandleID);

		if (nReturnCode)
		{
			OnTransferResponse(nReturnCode, rTransferReq.m_nSrcRequestID);
		}
		else
		{
			InsertSession(m_mapTransferSession, transferSession);
		}
	}
	else
	{
		OnTransferResponse(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rTransferReq.m_nSrcRequestID);
	}
}

void CVbeIdcClientService::OnDscMsg(VBE_IDC::CGetUserInfoApiCltReq& rGetUserInfoReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto onlineUser = m_mapOnlineUsers.Find(rGetUserInfoReq.m_nToken);

	if (onlineUser)
	{
		auto pGetUserInfoSession = DSC_THREAD_TYPE_NEW(CGetUserInfoSession) CGetUserInfoSession(*this);
		VBE_IDC::CGetUserInfoCltVbeReqDataWrapper wrapper(pGetUserInfoSession->m_nonce);
		VBE_IDC::CGetUserInfoCltVbeReq req;

		this->MakeNonce(pGetUserInfoSession->m_nonce);
		pGetUserInfoSession->m_nQueryType = rGetUserInfoReq.m_nQueryType;
		req.m_nSrcRequestID = rGetUserInfoReq.m_nSrcRequestID;
		req.m_nToken = rGetUserInfoReq.m_nToken;
		req.m_nQueryType = rGetUserInfoReq.m_nQueryType;

		auto nReturnCode = EncryptSendRequest(req, wrapper, onlineUser->m_cryptUserKey, onlineUser->m_VbeServer->m_nHandleID);

		if (nReturnCode)
		{
			OnGetUserInfoResponse(nReturnCode, rGetUserInfoReq.m_nQueryType, rGetUserInfoReq.m_nSrcRequestID);
		}
		else
		{
			InsertSession(m_mapGetUserInfoSession, pGetUserInfoSession);
		}
	}
	else
	{
		OnGetUserInfoResponse(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rGetUserInfoReq.m_nQueryType, rGetUserInfoReq.m_nSrcRequestID);
	}
}

ACE_INT32 CVbeIdcClientService::OnConnectedNodify(CMcpClientHandler* pMcpClientHandler)
{
	DSC_RUN_LOG_FINE("OnConnectedNodify:%d", pMcpClientHandler->GetHandleID());

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;
	
	if (m_pRegServer && m_pRegServer->m_nHandleID == pMcpClientHandler->GetHandleID())
	{
		m_pRegServer->m_bConnected = true;

		for (auto it = m_mapRegisterUserSession.begin(); it != m_mapRegisterUserSession.end(); ++it)
		{
			if (it.second->m_waitingSend)
			{
				SendRequest(it.second);
			}
		}
		return 0;
	}

	for (auto it = m_lstVbePeers.begin(); it != m_lstVbePeers.end(); it++)
	{
		if ((*it)->m_nHandleID == pMcpClientHandler->GetHandleID())
		{
			for (auto login = m_mapLoginSession.begin(); login != m_mapLoginSession.end(); m_mapLoginSession.Next(login))
			{
				if (login.second->m_waitingSend && login.second->m_VbeServer == (*it))
				{
					SendRequest(login.second);
				}
			}
			break;
		}
	}

	return 0;
}

void CVbeIdcClientService::OnNetworkError(CMcpHandler* pMcpHandler)
{

}

ACE_INT32 CVbeIdcClientService::OnConnectFailedNodify(PROT_COMM::CDscIpAddr& remoteAddr, const ACE_UINT32 nHandleID)
{
	if (m_pRegServer && m_pRegServer->m_nHandleID == nHandleID)
	{
		//
		return 0;
	}

	return 0;
}

void CVbeIdcClientService::OnRegisterUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* userKey)
{
	m_pCallback ? m_pCallback->OnRegisterUserResponse(nReturnCode, nRequestID, userKey) : void();
}

void CVbeIdcClientService::OnLoginResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 token)
{
	m_pCallback ? m_pCallback->OnLoginResponse(nReturnCode, nRequestID, token) : void();
}

void CVbeIdcClientService::OnGetUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nQueryType, DSC::CDscShortBlob* userinfo)
{
	m_pCallback ? m_pCallback->OnGetUserInfoResponse(nReturnCode, nRequestID, nQueryType, userinfo) : void();
}

void CVbeIdcClientService::OnUpdateResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nUpdateType, DSC::CDscShortBlob* userinfo)
{
	m_pCallback ? m_pCallback->OnUpdateUserResponse(nReturnCode, nRequestID, nUpdateType, userinfo) : void();
}

void CVbeIdcClientService::OnIdcInterfaceResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nInterfaceType, DSC::CDscShortBlob* userinfo)
{
	m_pCallback ? m_pCallback->OnCommonResponse(nReturnCode, nRequestID, nInterfaceType, userinfo) : void();
}

void CVbeIdcClientService::OnTransferResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* transKey)
{
	m_pCallback ? m_pCallback->OnTransferResponse(nReturnCode, nRequestID, transKey) : void();
}

class CVbeAddr
{
public:
	CVbeAddr()
		: m_IpAddr("IP_ADDR")
		, m_port("PORT")
		, m_peerID("PEER_ID")
		, m_channels("NEAR_CHANNELS")
	{
	}

public:
	PER_BIND_ATTR(m_IpAddr, m_port, m_peerID, m_channels);

public:
	CColumnWrapper< CDscString > m_IpAddr;
	CColumnWrapper< ACE_INT32 > m_port;
	CColumnWrapper< ACE_INT32 > m_peerID;
	CColumnWrapper< CDscString > m_channels;
};
class CVbeRegAddr
{
public:
	CVbeRegAddr()
		: m_IpAddr("IP_ADDR")
		, m_port("PORT")
		, m_peerID("PEER_ID")
	{
	}

public:
	PER_BIND_ATTR(m_IpAddr, m_port, m_peerID);

public:
	CColumnWrapper< CDscString > m_IpAddr;
	CColumnWrapper< ACE_INT32 > m_port;
	CColumnWrapper< ACE_INT32 > m_peerID;
};

class CVbeAddrCriterion : public CSelectCriterion
{
public:
	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.OrderByAsc("PEER_ID");
	}
};

ACE_INT32 CVbeIdcClientService::SendRequest(CRegisterUserSession* pSession)
{
	this->MakeNonce(pSession->m_nonce);

	VBE_IDC::CRegisterUserCltRegReqDataWrapper wrapper(pSession->m_nChannelID, pSession->m_nonce, pSession->m_userInfo, pSession->m_serverCryptKey);
	VBE_IDC::CRegisterUserCltRegReq req;

	req.m_nSrcRequestID = pSession->m_nCltSessionID;

	auto nReturnCode = EncryptSendRequest(req, wrapper, m_peerEnvelopeKey, m_pRegServer->m_nHandleID);

	if (nReturnCode)
	{ 
		OnRegisterUserResponse(nReturnCode, pSession->m_nSrcSessionID);
		EraseSession(m_mapRegisterUserSession, pSession->m_nCltSessionID);
		DSC_THREAD_TYPE_DELETE(pSession);

		return -1;
	}
	else
	{
		pSession->m_waitingSend = false;
	}
	return 0;
}

ACE_INT32 CVbeIdcClientService::SendRequest(CLoginSession* pSession)
{
	this->MakeNonce(pSession->m_nonce);

	VBE_IDC::CLoginCltVbeReqDataWrapper wrapper(pSession->m_nonce);
	VBE_IDC::CLoginCltVbeReq req;

	req.m_nSrcRequestID = pSession->m_nCltSessionID;
	req.m_userKey = pSession->m_userKey;

	auto nReturnCode = EncryptSendRequest(req, wrapper, pSession->m_cryptUserKey, pSession->m_VbeServer->m_nHandleID);

	if (nReturnCode)
	{
		OnRegisterUserResponse(nReturnCode, pSession->m_nSrcSessionID);
		EraseSession(m_mapLoginSession, pSession->m_nCltSessionID);
		DSC_THREAD_TYPE_DELETE(pSession);
		
		return -1;
	}
	else
	{
		pSession->m_waitingSend = false;
	}
	return 0;
}

//ACE_INT32 CVbeIdcClientService::SendRequest(CUpdateSession* pSession)
//{
//	return 0;
//}

ACE_INT32 CVbeIdcClientService::SendRequest(CTransferSession* pSession)
{
	return 0;
}

void CVbeIdcClientService::MakeNonce(DSC::CDscShortBlob& rNonce)
{
	int nonce = ACE_OS::rand_r(&m_seed);

	rNonce.AllocBuffer(sizeof(nonce));
	memcpy(rNonce.c_str(), &nonce, sizeof(nonce));
}

ACE_UINT32 CVbeIdcClientService::NewToken()
{
	while (true)
	{
		if (m_mapOnlineUsers.Find(m_nTokenSeq) == nullptr)
		{
			return m_nTokenSeq++;
		}
		m_nTokenSeq++;
	}
	return 0;

}

ACE_INT32 CVbeIdcClientService::LoadPeerInfo(void)
{
	CDscDatabase database;
	CDBConnection dbConnection;

	if (CDscDatabaseFactoryDemon::instance()->CreateDatabase(database, dbConnection))
	{
		DSC_RUN_LOG_ERROR("connect database failed.");

		return -1;
	}

	CTableWrapper< CCollectWrapper<CVbeAddr> > lstVbeAddr("VBE_ADDR_CFG");
	CVbeAddrCriterion criterion;

	if (::PerSelect(lstVbeAddr, database, dbConnection, &criterion))
	{
		DSC_RUN_LOG_ERROR("select from VBE_ADDR_CFG failed");

		return -1;
	}

	auto exploreChannels = [&](CVbePeer* peer, CDscString & strChannels) {
		char* p;

		if (strChannels.size() == 0) {
			return;
		}
		DSC_RUN_LOG_FINE("channels = %s", strChannels.c_str());
		p = strChannels.data();
		while (p && *p != '\0' && (*p < '0' || *p > '9'))
		{
			p++;
		}

		while (p && *p)
		{
			peer->m_channels.push_back(ACE_OS::atoi(p));
			while (*p >= '0' && *p <= '9')
			{
				p++;
			}
		
			while (*p != '\0' && (*p < '0' || *p > '9'))
			{
				p++;
			}
		}
	};
	ACE_INT32 nPeerID = std::numeric_limits<ACE_INT32>::min();

	for (auto it = lstVbeAddr->begin(); it != lstVbeAddr->end(); ++it)
	{
		if (nPeerID != *it->m_peerID)
		{
			auto vbePeer = DSC_THREAD_TYPE_NEW(CVbePeer) CVbePeer;

			m_lstVbePeers.push_back(vbePeer);
			exploreChannels(vbePeer, *it->m_channels);
			vbePeer->m_strIpAddr = *it->m_IpAddr;
			vbePeer->m_nPort = *it->m_port;
			vbePeer->m_nHandleID = this->AllocHandleID();

			for (auto chit = vbePeer->m_channels.begin(); chit != vbePeer->m_channels.end(); chit++)
			{
				m_mapVbePeers.Insert(*chit, vbePeer);
			}
			nPeerID = *it->m_peerID;
		}
	}

	if (m_lstVbePeers.size() > 0)
	{
		return 0;
	}

	return -1;
}

ACE_INT32 CVbeIdcClientService::ConnectRegServer(void)
{
	if (!m_pRegServer)
	{
		CDscDatabase database;
		CDBConnection dbConnection;

		if (CDscDatabaseFactoryDemon::instance()->CreateDatabase(database, dbConnection))
		{
			DSC_RUN_LOG_ERROR("connect database failed.");

			return -1;
		}

		// 获取注册服务的地址
		CTableWrapper< CCollectWrapper<CVbeRegAddr> > lstVbeRegAddr("VBE_REG_ADDR_CFG");
		CVbeAddrCriterion criterion;

		if (::PerSelect(lstVbeRegAddr, database, dbConnection, &criterion))
		{
			DSC_RUN_LOG_ERROR("select from VBE_REG_ADDR_CFG failed");

			return -1;
		}

		for (auto it = lstVbeRegAddr->begin(); it != lstVbeRegAddr->end(); ++it)
		{
			m_pRegServer = DSC_THREAD_TYPE_NEW(CVbePeer) CVbePeer;

			m_pRegServer->m_strIpAddr = *it->m_IpAddr;
			m_pRegServer->m_nPort = *it->m_port;
			m_pRegServer->m_nHandleID = this->AllocHandleID();

			break;
		}
	}

	if (m_pRegServer)
	{
		PROT_COMM::CDscIpAddr remoteAddr(m_pRegServer->m_strIpAddr, m_pRegServer->m_nPort);

		DSC_RUN_LOG_FINE("connect to reg server %s:%d", m_pRegServer->m_strIpAddr, m_pRegServer->m_nPort);

		return this->DoConnect(remoteAddr, NULL, m_pRegServer->m_nHandleID);
	}

	DSC_RUN_LOG_ERROR("not found reg server addr");
	return -1;
}

ACE_INT32 CVbeIdcClientService::ConnectVbeServer(CVbeIdcClientService::CVbePeer *vbeServer)
{
	PROT_COMM::CDscIpAddr remoteAddr(vbeServer->m_strIpAddr, vbeServer->m_nPort);

	DSC_RUN_LOG_FINE("connect to reg server %s:%d", vbeServer->m_strIpAddr, vbeServer->m_nPort);

	return this->DoConnect(remoteAddr, NULL, vbeServer->m_nHandleID);
}


void CVbeIdcClientService::OnTimeOut(CVbeIdcClientService::CRegisterUserSession* pRegistUserSession)
{
	OnRegisterUserResponse(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pRegistUserSession->m_nSrcSessionID);
	EraseSession(m_mapRegisterUserSession, pRegistUserSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pRegistUserSession);
}

void CVbeIdcClientService::OnNetError(CVbeIdcClientService::CRegisterUserSession* pRegistUserSession)
{
	OnRegisterUserResponse(VBE_IDC::EN_NETWORK_ERROR_TYPE, pRegistUserSession->m_nSrcSessionID);
	EraseSession(m_mapRegisterUserSession, pRegistUserSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pRegistUserSession);
}


void CVbeIdcClientService::OnTimeOut(CVbeIdcClientService::CLoginSession* pLoginSession)
{
	OnLoginResponse(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pLoginSession->m_nSrcSessionID);
	EraseSession(m_mapLoginSession, pLoginSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pLoginSession);
}

void CVbeIdcClientService::OnNetError(CVbeIdcClientService::CLoginSession* pLoginSession)
{
	OnLoginResponse(VBE_IDC::EN_NETWORK_ERROR_TYPE, pLoginSession->m_nSrcSessionID);
	EraseSession(m_mapLoginSession, pLoginSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pLoginSession);
}

void CVbeIdcClientService::OnTimeOut(CVbeIdcClientService::CGetUserInfoSession* pGetUserInfoSession)
{
	OnGetUserInfoResponse(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pGetUserInfoSession->m_nQueryType, pGetUserInfoSession->m_nSrcSessionID);
	EraseSession(m_mapGetUserInfoSession, pGetUserInfoSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pGetUserInfoSession);
}

void CVbeIdcClientService::OnNetError(CVbeIdcClientService::CGetUserInfoSession* pGetUserInfoSession)
{
	OnGetUserInfoResponse(VBE_IDC::EN_NETWORK_ERROR_TYPE, pGetUserInfoSession->m_nQueryType, pGetUserInfoSession->m_nSrcSessionID);
	EraseSession(m_mapGetUserInfoSession, pGetUserInfoSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pGetUserInfoSession);
}

void CVbeIdcClientService::OnTimeOut(CVbeIdcClientService::CUpdateSession* pUpdateSession)
{
	OnUpdateResponse(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pUpdateSession->m_nSrcSessionID, pUpdateSession->m_nUpdateType);
	EraseSession(m_mapUpdateSession, pUpdateSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pUpdateSession);
}

void CVbeIdcClientService::OnNetError(CVbeIdcClientService::CUpdateSession* pUpdateSession)
{
	OnUpdateResponse(VBE_IDC::EN_NETWORK_ERROR_TYPE, pUpdateSession->m_nSrcSessionID, pUpdateSession->m_nUpdateType);
	EraseSession(m_mapUpdateSession, pUpdateSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pUpdateSession);
}

void CVbeIdcClientService::OnTimeOut(CVbeIdcClientService::CIdcInterfaceSession* rIdcInterfaceSession)
{
	OnIdcInterfaceResponse(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, rIdcInterfaceSession->m_nSrcSessionID, rIdcInterfaceSession->m_nInterfaceType);
	EraseSession(m_mapIdcInterfaceSession, rIdcInterfaceSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(rIdcInterfaceSession);
}

void CVbeIdcClientService::OnNetError(CVbeIdcClientService::CIdcInterfaceSession* rIdcInterfaceSession)
{
	OnIdcInterfaceResponse(VBE_IDC::EN_NETWORK_ERROR_TYPE, rIdcInterfaceSession->m_nSrcSessionID, rIdcInterfaceSession->m_nInterfaceType);
	EraseSession(m_mapIdcInterfaceSession, rIdcInterfaceSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(rIdcInterfaceSession);
}

void CVbeIdcClientService::OnTimeOut(CVbeIdcClientService::CTransferSession* pTransferSession)
{
	OnTransferResponse(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pTransferSession->m_nSrcSessionID);
	EraseSession(m_mapTransferSession, pTransferSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pTransferSession);
}

void CVbeIdcClientService::OnNetError(CVbeIdcClientService::CTransferSession* pTransferSession)
{
	OnTransferResponse(VBE_IDC::EN_NETWORK_ERROR_TYPE, pTransferSession->m_nSrcSessionID);
	EraseSession(m_mapTransferSession, pTransferSession->m_nCltSessionID);
	DSC_THREAD_TYPE_DELETE(pTransferSession);
}

CVbeIdcClientService::IBaseSession::IBaseSession(CVbeIdcClientService& rClientService)
	: m_rClientBaseService(rClientService)
{
}

CVbeIdcClientService::CRegisterUserSession::CRegisterUserSession(CVbeIdcClientService& rClientBaseService)
	: IBaseSession(rClientBaseService)
{
}

CVbeIdcClientService::CRegisterUserSession::~CRegisterUserSession()
{
	m_nonce.FreeBuffer();
	m_userInfo.FreeBuffer();
	m_serverCryptKey.FreeBuffer();
}

CVbeIdcClientService::CLoginSession::CLoginSession(CVbeIdcClientService& rClientBaseService)
	: IBaseSession(rClientBaseService)
{
}
CVbeIdcClientService::CLoginSession::~CLoginSession()
{
	m_nonce.FreeBuffer();
	m_userKey.FreeBuffer();
	m_cryptUserKey.FreeBuffer();
}

CVbeIdcClientService::CGetUserInfoSession::CGetUserInfoSession(CVbeIdcClientService& rClientBaseService)
	:IBaseSession(rClientBaseService)
{
}

CVbeIdcClientService::CGetUserInfoSession::~CGetUserInfoSession()
{
	m_nonce.FreeBuffer();
}

CVbeIdcClientService::CUpdateSession::CUpdateSession(CVbeIdcClientService& rClientBaseService)
	:IBaseSession(rClientBaseService)
{
}

CVbeIdcClientService::CUpdateSession::~CUpdateSession()
{
	m_nonce.FreeBuffer();
}

CVbeIdcClientService::CIdcInterfaceSession::CIdcInterfaceSession(CVbeIdcClientService& rClientBaseService)
	:IBaseSession(rClientBaseService)
{
}

CVbeIdcClientService::CIdcInterfaceSession::~CIdcInterfaceSession()
{
	m_nonce.FreeBuffer();
}

CVbeIdcClientService::CTransferSession::CTransferSession(CVbeIdcClientService& rClientBaseService)
	:IBaseSession(rClientBaseService)
{
}

CVbeIdcClientService::CTransferSession::~CTransferSession()
{
	m_nonce.FreeBuffer();
}

void CVbeIdcClientService::CRegisterUserSession::OnTimer(void)
{
	m_rClientBaseService.OnTimeOut(this);
}

void CVbeIdcClientService::CRegisterUserSession::OnNetError(void)
{
	m_rClientBaseService.OnNetError(this);
}

void CVbeIdcClientService::CLoginSession::OnTimer(void)
{
	m_rClientBaseService.OnTimeOut(this);
}

void CVbeIdcClientService::CLoginSession::OnNetError(void)
{
	m_rClientBaseService.OnNetError(this);
}

void CVbeIdcClientService::CGetUserInfoSession::OnTimer(void)
{
	m_rClientBaseService.OnTimeOut(this);
}

void CVbeIdcClientService::CGetUserInfoSession::OnNetError(void)
{
	m_rClientBaseService.OnNetError(this);
}

void CVbeIdcClientService::CUpdateSession::OnTimer(void)
{
	m_rClientBaseService.OnTimeOut(this);
}

void CVbeIdcClientService::CUpdateSession::OnNetError(void)
{
	m_rClientBaseService.OnNetError(this);
}

void CVbeIdcClientService::CIdcInterfaceSession::OnTimer(void)
{
	m_rClientBaseService.OnTimeOut(this);
}

void CVbeIdcClientService::CIdcInterfaceSession::OnNetError(void)
{
	m_rClientBaseService.OnNetError(this);
}

void CVbeIdcClientService::CTransferSession::OnTimer(void)
{
	m_rClientBaseService.OnTimeOut(this);
}

void CVbeIdcClientService::CTransferSession::OnNetError(void)
{
	m_rClientBaseService.OnNetError(this);
}

CVbeIdcClientService::COnlineUser::~COnlineUser()
{
	m_userKey.FreeBuffer();
	m_cryptUserKey.FreeBuffer();
}
CVbeIdcClientService::COnlineUser::COnlineUser(CVbeIdcClientService& srv)
	:m_clientService(srv)
{
}

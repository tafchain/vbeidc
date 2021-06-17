#include "vbe_idc/vbe_idc_reg_service/vbe_idc_reg_service.h"
#include "vbe_idc/vbe_idc_reg_service/vbe_idc_rr_channel_allocator.h"

#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg_wrapper.h"

#include "vbh/common/vbh_comm_func.h"
#include "vbh/common/vbh_comm_error_code.h"

#include "openssl/rand.h"
#include "openssl/err.h"

#include "dsc/db/per/persistence.h"
#include "dsc/dsc_database_factory.h"


CVbeIdcRegService::CVbeIdcRegService(const CDscString& strIpAddr, const ACE_INT32 nPort)
	: m_strIpAddr(strIpAddr)
	, m_nPort(nPort)
{
}

ACE_INT32 CVbeIdcRegService::OnInit(void)
{
	if (CDscHtsServerService::OnInit())
	{
		DSC_RUN_LOG_ERROR("bc endorser service init failed!");

		return -1;
	}

	// 使用轮询分配器
	m_channelAllocator = DSC_THREAD_TYPE_NEW(CVbeIdcRrChannelAllocator) CVbeIdcRrChannelAllocator;

	if (m_channelAllocator->Init())
	{
		DSC_RUN_LOG_ERROR("hls service router init failed.");
		return -1;
	}

	m_pAcceptor = DSC_THREAD_TYPE_NEW(CMcpAsynchAcceptor<CVbeIdcRegService>) CMcpAsynchAcceptor<CVbeIdcRegService>(*this);
	if (m_pAcceptor->Open(m_nPort, m_strIpAddr.c_str()))
	{
		DSC_THREAD_TYPE_DEALLOCATE(m_pAcceptor);
		m_pAcceptor = NULL;
		DSC_RUN_LOG_ERROR("acceptor failed, ip addr:%s, port:%d", m_strIpAddr.c_str(), m_nPort);

		return -1;
	}
	else
	{
		this->RegistHandler(m_pAcceptor, ACE_Event_Handler::ACCEPT_MASK);
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


	DSC_RUN_LOG_INFO("vbe service init succeed!");

	return 0;
}

ACE_INT32 CVbeIdcRegService::OnExit(void)
{
	return 0;
}

void CVbeIdcRegService::OnTimeOut(CVbeIdcRegService::CRegisterUserSession* pRegSession)
{
	this->OnRegisterUserResponse(pRegSession->m_pRegServiceHandler->GetHandleID(), VBH::EN_TIMEOUT_ERROR_TYPE, pRegSession->m_nCltSessionID, pRegSession->m_nonce);
	this->EraseSession(m_mapRegSession, pRegSession->m_nRegSessionID);
	DSC_THREAD_TYPE_DELETE(pRegSession);
}

void CVbeIdcRegService::OnNetworkError(CVbeIdcRegService::CRegisterUserSession* pRegSession)
{
	this->EraseSession(m_mapRegSession, pRegSession->m_nRegSessionID);
	DSC_THREAD_TYPE_DELETE(pRegSession);
}

void CVbeIdcRegService::OnRegisterUserResponse(ACE_INT32 nHandleID, ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob & nonce, DSC::CDscShortBlob* userKey)
{
	VBE_IDC::CRegisterUserRegCltRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	char* encryptBuf = nullptr;
	ACE_UINT32 encryptBufLen;

	if (nReturnCode == VBH::EN_OK_TYPE)
	{
		VBE_IDC::CRegisterUserRegCltRspDataWrapper wrapper(nonce, *userKey);
		

		if (VBE_IDC::EncryptWrapperMsg(wrapper, m_peerEnvelopeKey, encryptBuf, encryptBufLen))
		{
			rsp.m_nReturnCode = VBH::EN_ENCRYPT_ERROR_TYPE;
		}
		else
		{
			rsp.m_data.Set(encryptBuf, encryptBufLen);
		}
	}

	SendHtsMsg(rsp, nHandleID);

	if (encryptBuf)
	{
		DSC_THREAD_SIZE_FREE(encryptBuf, encryptBufLen);
	}
}

ACE_UINT32 CVbeIdcRegService::AllocChannel()
{
	return m_channelAllocator->AllocChannel();
}

CMcpServerHandler* CVbeIdcRegService::AllocMcpHandler(ACE_HANDLE handle)
{
	return DSC_THREAD_DYNAMIC_TYPE_NEW(CVbeRegServiceHandler) CVbeRegServiceHandler(*this, handle, this->AllocHandleID());
}


ACE_INT32 CVbeIdcRegService::OnHtsMsg(VBE_IDC::CRegisterUserCltRegReq& rRegistUser, CMcpHandler* pMcpHandler)
{
	DSC_RUN_LOG_WARNING("CRegisterUserCltVbeReq %s", rRegistUser.m_data.c_str());

	ACE_INT32 nReturnCode = VBH::EN_OK_TYPE;
	DSC::CDscShortBlob nonce;
	DSC::CDscShortBlob userInfo;
	DSC::CDscShortBlob serverCryptKey;
	ACE_INT32 nChannelID;
	VBE_IDC::CRegisterUserCltRegReqDataWrapper wrapper(nChannelID, nonce, userInfo, serverCryptKey);

	char* decryptBuf = VBE_IDC::DecryptWrapperMsg(wrapper, m_peerEnvelopeKey, rRegistUser.m_data.c_str(), rRegistUser.m_data.size());

	if (decryptBuf)
	{
		CDscMsg::CDscMsgAddr addr;

		if (m_channelAllocator->GetHlsAddr(addr, AllocChannel()))
		{
			nReturnCode = VBH::EN_SYSTEM_ERROR_TYPE;
		}
		else
		{
			VBE_IDC::CRegisterUserRegHlsReq req;

			req.m_nChannelID = nChannelID;
			req.m_nSrcRequestID = m_nSessionID;
			req.m_userInfo = userInfo;
			req.m_serverCryptKey = serverCryptKey;

			if (this->SendDscMessage(req, addr))
			{
				nReturnCode = VBH::EN_NETWORK_ERROR_TYPE;
			}
			else
			{
				auto pSession = DSC_THREAD_TYPE_NEW(CRegisterUserSession) CRegisterUserSession(*this);

				pSession->m_nCltSessionID = rRegistUser.m_nSrcRequestID;
				pSession->m_nonce.Clone(nonce);
				InsertSession(m_mapRegSession, pSession, pMcpHandler);
			}
			
		}
	}
	else
	{
		nReturnCode = VBH::EN_DECRYPT_ERROR_TYPE;
	}

	if (nReturnCode != VBH::EN_OK_TYPE)
	{
		OnRegisterUserResponse(pMcpHandler->GetHandleID(), nReturnCode, rRegistUser.m_nSrcRequestID, nonce);
	}

	return 0;
}

void CVbeIdcRegService::OnDscMsg(VBE_IDC::CRegisterUserHlsRegRsp& rRegistUserRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pSession = (CRegisterUserSession*)EraseSession(m_mapRegSession, rRegistUserRsp.m_nSrcRequestID);

	if (pSession)
	{
		OnRegisterUserResponse(pSession->m_pRegServiceHandler->GetHandleID(), rRegistUserRsp.m_nReturnCode, pSession->m_nCltSessionID, pSession->m_nonce, &rRegistUserRsp.m_userKey);
		
		DSC_THREAD_TYPE_DELETE(pSession);
	}
}

CVbeIdcRegService::CVbeRegServiceHandler::CVbeRegServiceHandler(CMcpServerService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID)
	: CMcpServerHandler(rService, handle, nHandleID)
{
}

CVbeIdcRegService::IUserSession::IUserSession(CVbeIdcRegService& rRegService)
	:m_rRegService(rRegService)
{
}

CVbeIdcRegService::CRegisterUserSession::CRegisterUserSession(CVbeIdcRegService& rRegService)
	: IUserSession(rRegService)
{
}

CVbeIdcRegService::CRegisterUserSession::~CRegisterUserSession()
{
	m_nonce.FreeBuffer();
}

void CVbeIdcRegService::CRegisterUserSession::OnTimer()
{
	m_rRegService.OnTimeOut(this);
}

void CVbeIdcRegService::CRegisterUserSession::OnNetError()
{
	m_rRegService.OnNetworkError(this);
}

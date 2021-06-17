#include "vbe_idc/vbe_idc_hls/vbe_idc_hls_service.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_user_util.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_cc_action_msg.h"

#include "openssl/rand.h"
#include "openssl/err.h"

#include "dsc/db/per/persistence.h"
#include "dsc/dsc_database_factory.h"

#include "common/vbh_comm_msg_def.h"
#include "common/sdk_comm_if.h"

CVbeIdcHlsService::CVbeIdcHlsService(const ACE_UINT32 nChannelID)
	:m_nChannelID(nChannelID)
{
}

ACE_INT32 CVbeIdcHlsService::OnInit(void)
{
	if (CVbhAdapterBase::OnInit())
	{
		DSC_RUN_LOG_ERROR("hls init failed!");

		return -1;
	}

	if (m_userManager.Init())
	{
		CVbhAdapterBase::OnExit();

		return -1;
	}

	if (m_proposeManager.Init())
	{
		CVbhAdapterBase::OnExit();

		return -1;
	}

	return 0;
}

ACE_INT32 CVbeIdcHlsService::OnExit(void)
{
	return 0;
}

void CVbeIdcHlsService::OnRegistUserResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userKey)
{
	auto regSession = (CHlsRegisterUserSession*)FindSession(m_mapRegisterUser, nRequestID);

	if (regSession)
	{
		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			regSession->m_userKey.Clone(userKey);
		}
		else
		{
			VBE_IDC::CRegisterUserHlsRegRsp rsp;

			rsp.m_nReturnCode = nReturnCode;
			rsp.m_nSrcRequestID = regSession->m_nCltSessionID;

			this->SendDscMessage(rsp, regSession->m_srcMsgAddr);

			EraseSession(m_mapRegisterUser, nRequestID);
			DSC_THREAD_TYPE_DELETE(regSession);
		}
	}
}

void CVbeIdcHlsService::OnRegistUserResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	auto regSession = (CHlsRegisterUserSession*)EraseSession(m_mapRegisterUser, nRequestID);
	DSC::CDscShortBlob vbeUserKey;

	if (regSession)
	{
		VBE_IDC::CRegisterUserHlsRegRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcRequestID = regSession->m_nCltSessionID;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			VBE_IDC::CVbeUserUtil::EncodeVbeUserKey(vbeUserKey, regSession->m_userKey, m_nChannelID);

			rsp.m_userKey = vbeUserKey;
			
			auto nHashKey = VBE_IDC::CVbeUserUtil::GetHashKey(regSession->m_userKey);
			CVbeIdcUserInfo userinfo;

			userinfo.m_nHashKey = nHashKey;
			userinfo.m_bLocked = false;
			userinfo.m_nLockKey = 0;

			m_userManager.PutUserInfo(userinfo, 0);
		}

		this->SendDscMessage(rsp, regSession->m_srcMsgAddr);

		DSC_THREAD_TYPE_DELETE(regSession);
		vbeUserKey.FreeBuffer();
	}
}

void CVbeIdcHlsService::OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& transKey)
{
	auto pTransferSession = (CHlsTransferSession*)FindSession(m_mapTransfer, nRequestID);

	if (pTransferSession)
	{
		if (nReturnCode)
		{
			EraseTimerSession(m_mapTransfer, nRequestID);
			OnProposeFailed(nReturnCode, pTransferSession);
		}
		else
		{
			pTransferSession->m_transKey.Clone(transKey);
		}
	}
}

void CVbeIdcHlsService::OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	auto pTransferSession = (CHlsTransferSession*)EraseTimerSession(m_mapTransfer, nRequestID);

	if (pTransferSession)
	{
		if (nReturnCode)
		{
			OnProposeFailed(nReturnCode, pTransferSession);
		}
		else
		{
			OnProposeSuccess(pTransferSession);
		}
	}
}

void CVbeIdcHlsService::OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo)
{
	auto pQuerySession = (CHlsCheckUserQuerySession*)EraseSession(m_mapCheckUserQuery, nRequestID);

	if (pQuerySession)
	{
		pQuerySession->m_pCheckUserSession->m_arrCheckUserQuery.Erase(pQuerySession);

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			CVbeIdcUserInfo userinfo;
			ACE_UINT32 nVaildCount = 0;

			userinfo.m_nHashKey = pQuerySession->m_nUserHashKey;

			m_userManager.PutUserInfo(userinfo);

			for (auto& it : pQuerySession->m_pCheckUserSession->m_mapUserHashKey)
			{
				if (it.first == pQuerySession->m_nUserHashKey)
				{
					it.second = true;
				}

				if (it.second == true)
				{
					nVaildCount++;
				}
			}

			if (nVaildCount == pQuerySession->m_pCheckUserSession->m_mapUserHashKey.size())
			{
				// 所有用户有效，回复
				ResponseCheckUserReq(VBE_IDC::EN_OK_TYPE, pQuerySession->m_pCheckUserSession);
				ReleaseCheckUserSession(pQuerySession->m_pCheckUserSession);
			}
		}
		else
		{
			// 只要一个用户无效，整个操作失败
			ResponseCheckUserReq(nReturnCode, pQuerySession->m_pCheckUserSession);
			ReleaseCheckUserSession(pQuerySession->m_pCheckUserSession);
		}

		DSC_THREAD_TYPE_DELETE(pQuerySession);
	}
}

void CVbeIdcHlsService::OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo)
{
}

void CVbeIdcHlsService::OnDscMsg(VBE_IDC::CRegisterUserRegHlsReq& rRegReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto nReturnCode = this->RegistUser(m_nHlsSessionID, rRegReq.m_nChannelID, true, rRegReq.m_userInfo, rRegReq.m_serverCryptKey);
	//auto nReturnCode = this->RegistUser(m_nHlsSessionID, m_nChannelID, true, rRegReq.m_userInfo, rRegReq.m_serverCryptKey);

	if (nReturnCode != VBH::EN_OK_TYPE)
	{
		VBE_IDC::CRegisterUserHlsRegRsp rsp;

		rsp.m_nReturnCode = nReturnCode;
		rsp.m_nSrcRequestID = rRegReq.m_nSrcRequestID;

		this->SendDscMessage(rsp, rSrcMsgAddr);
	}
	else
	{
		auto pSession = DSC_THREAD_TYPE_NEW(CHlsRegisterUserSession) CHlsRegisterUserSession;

		pSession->m_nCltSessionID = rRegReq.m_nSrcRequestID;
		InsertSession(m_mapRegisterUser, pSession, rSrcMsgAddr);
	}
}


void CVbeIdcHlsService::OnDscMsg(VBE_IDC::CCheckUsersVbeHlsReq& rCheckUsersReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pCheckUserSession = DSC_THREAD_TYPE_NEW(CHlsCheckUserSession) CHlsCheckUserSession();

	pCheckUserSession->m_nCltSessionID = rCheckUsersReq.m_nSrcRequestID;
	pCheckUserSession->m_srcMsgAddr = rSrcMsgAddr;
	
	auto checkedCount = 0;

	for (auto &user : rCheckUsersReq.m_lstUserKey)
	{
		CVbeIdcUserInfo userinfo;
		auto nHashKey = VBE_IDC::CVbeUserUtil::GetHashKey(user);

		if (m_userManager.GetUserInfo(nHashKey, userinfo))
		{
			pCheckUserSession->m_mapUserHashKey[nHashKey] = false;

			auto pCheckUserQuerySession = DSC_THREAD_TYPE_NEW(CHlsCheckUserQuerySession) CHlsCheckUserQuerySession();

			pCheckUserQuerySession->m_pCheckUserSession = pCheckUserSession;
			pCheckUserQuerySession->m_nUserHashKey = nHashKey;

			auto nReturnCode = QueryUserInfo(m_nHlsSessionID, m_nChannelID, user);
			if (nReturnCode)
			{
				//
				ResponseCheckUserReq(nReturnCode, pCheckUserSession);
				ReleaseCheckUserSession(pCheckUserSession);
			}
			else
			{
				InsertSession(m_mapCheckUserQuery, pCheckUserQuerySession, rSrcMsgAddr);
				pCheckUserSession->m_arrCheckUserQuery.Insert(pCheckUserQuerySession);
			}
		}
		else
		{
			checkedCount++;
			pCheckUserSession->m_mapUserHashKey[nHashKey] = true;
		}
	}

	if (checkedCount == rCheckUsersReq.m_lstUserKey.size())
	{
		ResponseCheckUserReq(VBE_IDC::EN_OK_TYPE, pCheckUserSession);
		DSC_THREAD_TYPE_DELETE(pCheckUserSession);
	}

}


void CVbeIdcHlsService::OnDscMsg(VBE_IDC::CTransferVbeHlsReq& rTransferReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pTransferSession = DSC_THREAD_TYPE_NEW(CHlsTransferSession) CHlsTransferSession(*this);

	if (DecodePropose(pTransferSession, rTransferReq.m_propose))
	{
		DSC::CDscShortBlob transKey;
		ResponseTransferReq(VBE_IDC::EN_PARAM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, rSrcMsgAddr, transKey);
		
		DSC_THREAD_TYPE_DELETE(pTransferSession);

		return;
	}

	pTransferSession->m_dbProposeItem.m_bMain = true;

	if (pTransferSession->m_dbProposeItem.m_lstChannelID.size() == 1)
	{
		pTransferSession->m_dbRecord = nullptr;
	}
	else
	{
		// 存储提案
		SBS::SbsRecord record;

		if (m_proposeManager.InsertPropose(record, pTransferSession->m_dbProposeItem))
		{
			ResponseTransferReq(VBE_IDC::EN_SYSTEM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, rSrcMsgAddr);

			return;
		}

		pTransferSession->m_dbRecord = DSC_THREAD_TYPE_NEW(SBS::SbsRecord) SBS::SbsRecord;
		*pTransferSession->m_dbRecord = record;
	}

	for (auto& nHashKey : pTransferSession->m_lstUserHashKey)
	{
		m_mapUserTransferQueue[nHashKey].push_back(pTransferSession);
	}

	pTransferSession->m_nCltSessionID = rTransferReq.m_nSrcRequestID;
	pTransferSession->m_srcMsgAddr = rSrcMsgAddr;
	pTransferSession->m_nRetryTimes = 0;

	MaybePropose(pTransferSession);
}

void CVbeIdcHlsService::OnDscMsg(VBE_IDC::CTransferHlsHlsReq& rTransferReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pTransferSession = DSC_THREAD_TYPE_NEW(CHlsTransferSession) CHlsTransferSession(*this);

	if (DecodePropose(pTransferSession, rTransferReq.m_propose))
	{
		ResponseTransferHlsReq(VBE_IDC::EN_PARAM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, rSrcMsgAddr);

		DSC_THREAD_TYPE_DELETE(pTransferSession);

		return;
	}

	pTransferSession->m_dbProposeItem.m_bMain = false;
	// 存储提案
	SBS::SbsRecord record;

	if (m_proposeManager.InsertPropose(record, pTransferSession->m_dbProposeItem))
	{
		ResponseTransferHlsReq(VBE_IDC::EN_SYSTEM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, rSrcMsgAddr);

		return;
	}

	// 存下来即成功
	ResponseTransferHlsReq(VBE_IDC::EN_OK_TYPE, rTransferReq.m_nSrcRequestID, rSrcMsgAddr);

	pTransferSession->m_dbRecord = DSC_THREAD_TYPE_NEW(SBS::SbsRecord) SBS::SbsRecord;
	*pTransferSession->m_dbRecord = record;

	for (auto& nHashKey : pTransferSession->m_lstUserHashKey)
	{
		m_mapUserTransferQueue[nHashKey].push_back(pTransferSession);
	}

	pTransferSession->m_nCltSessionID = rTransferReq.m_nSrcRequestID;
	pTransferSession->m_srcMsgAddr = rSrcMsgAddr;
	pTransferSession->m_nRetryTimes = 0;

	MaybePropose(pTransferSession);
}

void CVbeIdcHlsService::OnDscMsg(VBE_IDC::CTransferHlsHlsRsp& rTransferRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pTransferHlsSession = (CHlsTransferToHlsSession*)EraseTimerSession(m_mapTransferToHls, rTransferRsp.m_nSrcRequestID);

	if (pTransferHlsSession)
	{
		pTransferHlsSession->m_pTransferSession->m_arrTransferToHlsSession.Erase(pTransferHlsSession);

		for (auto it = pTransferHlsSession->m_pTransferSession->m_dbProposeItem.m_lstChannelID.begin(); it != pTransferHlsSession->m_pTransferSession->m_dbProposeItem.m_lstChannelID.end(); it++)
		{
			if (*it == pTransferHlsSession->m_nChannelID)
			{
				pTransferHlsSession->m_pTransferSession->m_dbProposeItem.m_lstChannelID.erase(it);
				break;
			}
		}

		if (pTransferHlsSession->m_pTransferSession->m_dbProposeItem.m_lstChannelID.size() == 0)
		{
			m_proposeManager.DeletePropose(*pTransferHlsSession->m_pTransferSession->m_dbRecord);
			ReleaseTransferSession(pTransferHlsSession->m_pTransferSession);
		}

		DSC_THREAD_TYPE_DELETE(pTransferHlsSession);
	}
}

void CVbeIdcHlsService::ReleaseCheckUserSession( CHlsCheckUserSession* pCheckSession)
{
	// 删除和这个请求关联的所有vbh query
	for (auto index = 0; index < pCheckSession->m_arrCheckUserQuery.Size(); index++)
	{
		m_mapCheckUserQuery.Erase(pCheckSession->m_arrCheckUserQuery[index]);
		DSC_THREAD_TYPE_DELETE(pCheckSession->m_arrCheckUserQuery[index]);
	}

	DSC_THREAD_TYPE_DELETE(pCheckSession);
}

void CVbeIdcHlsService::ReleaseTransferSession(CHlsTransferSession* pTransferSession)
{
	for (auto& nHashKey : pTransferSession->m_lstUserHashKey)
	{
		auto it = m_mapUserTransferQueue.find(nHashKey);

		if (it != m_mapUserTransferQueue.end())
		{
			for (auto transIt = it->second.begin(); transIt != it->second.end(); transIt++)
			{
				if (*transIt == pTransferSession)
				{
					it->second.erase(transIt);
					break;
				}
			}
		}
	}

	for (auto& nHashKey : pTransferSession->m_lstUserHashKey)
	{
		auto it = m_mapUserTransferQueue.find(nHashKey);

		if (it != m_mapUserTransferQueue.end())
		{
			MaybePropose(it->second.front());
		}
	}

	for (auto index = 0; index < pTransferSession->m_arrTransferToHlsSession.Size(); index++)
	{
		m_mapTransferToHls.Erase(pTransferSession->m_arrTransferToHlsSession[index]);
		CancelDscTimer(pTransferSession->m_arrTransferToHlsSession[index]);
		DSC_THREAD_TYPE_DELETE(pTransferSession->m_arrTransferToHlsSession[index]);
	}

	DSC_THREAD_TYPE_DELETE(pTransferSession);
}

ACE_INT32 CVbeIdcHlsService::DecodePropose(CHlsTransferSession* pTransferSession, DSC::CDscShortBlob& propose)
{
	VBE_IDC::CVbeIdcCcActionTransferMsg msg;
	DSC::CDscShortBlob vbhUserKey;
	ACE_UINT32 nChannelID;

	if (DSC::Decode(msg, propose.c_str(), propose.size()))
	{
		return -1;
	}

	if (VBE_IDC::CVbeUserUtil::DecodeVbeUserKey(vbhUserKey, nChannelID, msg.m_sponsorUserKey))
	{
		return -1;
	}

	pTransferSession->m_dbProposeItem.m_sponsorUserKey.Clone(vbhUserKey);

	for (auto& userKey : msg.m_lstUserKey)
	{
		if (VBE_IDC::CVbeUserUtil::DecodeVbeUserKey(vbhUserKey, nChannelID, userKey))
		{
			return -1;
		}

		pTransferSession->m_lstUserHashKey.push_back(VBE_IDC::CVbeUserUtil::GetHashKey(vbhUserKey));
		pTransferSession->m_dbProposeItem.m_lstChannelID.push_back(nChannelID);
	}

	pTransferSession->m_dbProposeItem.m_propose.Clone(propose);
	
	return 0;
}

void CVbeIdcHlsService::MaybePropose(CHlsTransferSession* pTransferSession)
{
	// 若已在session map 中说明已发起提案
	if (m_mapTransfer.Find(pTransferSession->m_nHlsSessionID))
	{
		return;
	}

	for (auto& user : pTransferSession->m_lstUserHashKey)
	{
		auto it = m_mapUserTransferQueue.find(user);

		if (it != m_mapUserTransferQueue.end())
		{
			// 没有在队首
			if (it->second.front() != pTransferSession)
			{
				return;
			}
		}
	}

	auto nReturnCode = Propose(m_nHlsSessionID, m_nChannelID, true, 1, pTransferSession->m_dbProposeItem.m_sponsorUserKey, pTransferSession->m_dbProposeItem.m_propose);

	if (nReturnCode)
	{
		OnProposeFailed(nReturnCode, pTransferSession);
	}
	else
	{
		SetDscTimer(pTransferSession, EN_SESSION_TIMEOUT);
		m_mapTransfer.Insert(m_nHlsSessionID++, pTransferSession);
	}
}

bool CVbeIdcHlsService::OnProposeFailed(ACE_INT32 nReturnCode, CHlsTransferSession* pTransferSession)
{
	if (pTransferSession->m_dbProposeItem.m_bMain)
	{
		// 主提案可以失败
		ResponseTransferReq(nReturnCode, pTransferSession->m_nCltSessionID, pTransferSession->m_srcMsgAddr);
		ReleaseTransferSession(pTransferSession);

		return true;
	}
	else
	{
		if (pTransferSession->m_nRetryTimes > EN_MAX_RETRY_PROPOSE_TIMES)
		{
			//TODO 确认失败，人工干预
			DSC_RUN_LOG_ERROR("transfer failed times > %d", EN_MAX_RETRY_PROPOSE_TIMES);

			return true;
		}
		else
		{
			pTransferSession->m_nRetryTimes++;
			// 等待超时后重试
			SetDscTimer(pTransferSession, EN_SESSION_TIMEOUT);
			m_mapTransfer.Insert(m_nHlsSessionID++, pTransferSession);
		}
	}

	return false;
}

void CVbeIdcHlsService::OnProposeSuccess(CHlsTransferSession* pTransferSession)
{
	if (pTransferSession->m_dbProposeItem.m_bMain)
	{
		ResponseTransferReq(VBE_IDC::EN_OK_TYPE, pTransferSession->m_nCltSessionID, pTransferSession->m_srcMsgAddr);

		if (pTransferSession->m_dbRecord == nullptr)
		{
			ReleaseTransferSession(pTransferSession);
		}
		else
		{
			// 向其他hls发请求
			for (auto it = pTransferSession->m_dbProposeItem.m_lstChannelID.begin(); it != pTransferSession->m_dbProposeItem.m_lstChannelID.end();)
			{
				auto curIt = it;

				it++;

				if (*curIt == m_nChannelID)
				{
					pTransferSession->m_dbProposeItem.m_lstChannelID.erase(curIt);
				}
				else
				{
					SendProposeToHls(pTransferSession, *curIt);
				}
			}
		}
	}
	else 
	{
		if (pTransferSession->m_dbRecord)
		{
			m_proposeManager.DeletePropose(*pTransferSession->m_dbRecord);
		}
		ReleaseTransferSession(pTransferSession);
	}

	
}

void CVbeIdcHlsService::MaybeRetryProposeOnFailed(ACE_INT32 nReturnCode, CHlsTransferSession* pTransferSession)
{
	if (pTransferSession->m_dbProposeItem.m_bMain)
	{
		ResponseTransferReq(nReturnCode, pTransferSession->m_nCltSessionID, pTransferSession->m_srcMsgAddr);
		EraseTimerSession(m_mapTransfer, pTransferSession->m_nHlsSessionID);
		ReleaseTransferSession(pTransferSession);
	}
	else
	{
		if (pTransferSession->m_nRetryTimes > EN_MAX_RETRY_PROPOSE_TIMES)
		{
			//TODO 确认失败，人工干预
			DSC_RUN_LOG_ERROR("transfer failed more than %u times", pTransferSession->m_nRetryTimes);
		}
		else
		{
			pTransferSession->m_nRetryTimes++;

			nReturnCode = Propose(m_nHlsSessionID, m_nChannelID, true, 1, pTransferSession->m_dbProposeItem.m_sponsorUserKey, pTransferSession->m_dbProposeItem.m_propose);

			if (nReturnCode)
			{
				if (pTransferSession->m_nRetryTimes > EN_MAX_RETRY_PROPOSE_TIMES)
				{
					//TODO 确认失败，人工干预
					DSC_RUN_LOG_ERROR("transfer failed more than %u times", pTransferSession->m_nRetryTimes);
				}
				else
				{
					// 等超时后重试
					SetDscTimer(pTransferSession, EN_SESSION_TIMEOUT);
					m_mapTransfer.Insert(m_nHlsSessionID++, pTransferSession);
				}
			}
			else
			{
				SetDscTimer(pTransferSession, EN_SESSION_TIMEOUT);
				m_mapTransfer.Insert(m_nHlsSessionID++, pTransferSession);
			}
		}
	}
}

ACE_INT32 CVbeIdcHlsService::SendProposeToHls(CHlsTransferSession* pTransferSession, ACE_UINT32 nChannelID)
{
	CDscMsg::CDscMsgAddr addr;

	if (m_hlsRouter.GetHlsAddr(addr, nChannelID))
	{
		DSC_RUN_LOG_ERROR("not found channel addr:%d", nChannelID);

		return VBE_IDC::EN_SYSTEM_ERROR_TYPE;
	}

	VBE_IDC::CTransferHlsHlsReq req;

	req.m_nSrcRequestID = m_nHlsSessionID;
	req.m_propose = pTransferSession->m_dbProposeItem.m_propose;

	auto pTransferHlsSession = DSC_THREAD_TYPE_NEW(CHlsTransferToHlsSession) CHlsTransferToHlsSession(*this);

	pTransferHlsSession->m_pTransferSession = pTransferSession;
	pTransferHlsSession->m_nChannelID = nChannelID;
	pTransferHlsSession->m_nRetryTimes = 0;
	pTransferSession->m_arrTransferToHlsSession.Insert(pTransferHlsSession);

	SetDscTimer(pTransferHlsSession, EN_SESSION_TIMEOUT);
	m_mapTransferToHls.DirectInsert(m_nHlsSessionID++, pTransferHlsSession);

	SendDscMessage(req, addr);

	return 0;
}

void CVbeIdcHlsService::ResponseCheckUserReq(ACE_INT32 nReturnCode, CHlsCheckUserSession* pCheckSession)
{
	VBE_IDC::CCheckUsersHlsVbeRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = pCheckSession->m_nCltSessionID;

	SendDscMessage(rsp, pCheckSession->m_srcMsgAddr);
}


void CVbeIdcHlsService::ResponseTransferReq(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const CDscMsg::CDscMsgAddr& rSrcMsgAddr, DSC::CDscShortBlob& transKey)
{
	VBE_IDC::CTransferHlsVbeRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;
	rsp.m_nChannelID = m_nChannelID;
	rsp.m_transKey = transKey;

	SendDscMessage(rsp, rSrcMsgAddr);
}

void CVbeIdcHlsService::ResponseTransferReq(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	DSC::CDscShortBlob transKey;

	ResponseTransferReq(nReturnCode, nRequestID, rSrcMsgAddr, transKey);
}

void CVbeIdcHlsService::ResponseTransferHlsReq(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBE_IDC::CTransferHlsHlsRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	SendDscMessage(rsp, rSrcMsgAddr);
}


void CVbeIdcHlsService::OnTimeOut(CHlsTransferToHlsSession* pTransferToHlsSession)
{
	if (pTransferToHlsSession->m_nRetryTimes > EN_MAX_RETRY_PROPOSE_TIMES)
	{
		// TODO 人工干预
		DSC_RUN_LOG_ERROR("CHlsTransferToHlsSession timeout, channel id = %d", pTransferToHlsSession->m_nChannelID);
	}
	else
	{
		pTransferToHlsSession->m_nRetryTimes++;

		CDscMsg::CDscMsgAddr addr;
		VBE_IDC::CTransferHlsHlsReq req;

		m_hlsRouter.GetHlsAddr(addr, pTransferToHlsSession->m_nChannelID);

		req.m_nSrcRequestID = pTransferToHlsSession->m_nHlsSessionID;
		req.m_propose = pTransferToHlsSession->m_pTransferSession->m_dbProposeItem.m_propose;

		SetDscTimer(pTransferToHlsSession, EN_SESSION_TIMEOUT);

		SendDscMessage(req, addr);
	}
}

void CVbeIdcHlsService::OnNetworkError(CHlsTransferToHlsSession* pTransferToHlsSession)
{
	// 等超时再处理
}

void CVbeIdcHlsService::OnTimeOut(CHlsTransferSession* pTransferSession)
{
	MaybeRetryProposeOnFailed(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pTransferSession);
}

void CVbeIdcHlsService::OnNetworkError(CHlsTransferSession* pTransferSession)
{
	MaybeRetryProposeOnFailed(VBE_IDC::EN_NETWORK_ERROR_TYPE, pTransferSession);
}

CVbeIdcHlsService::CHlsRegisterUserSession::~CHlsRegisterUserSession()
{
	m_userKey.FreeBuffer();
	m_cryptUserKey.FreeBuffer();
}


CVbeIdcHlsService::IHlsBaseTimerSession::IHlsBaseTimerSession(CVbeIdcHlsService& rHlsService)
	:m_rHlsService(rHlsService)
{
}

CVbeIdcHlsService::CHlsTransferSession::CHlsTransferSession(CVbeIdcHlsService& rService)
	: IHlsBaseTimerSession(rService)
{
}

CVbeIdcHlsService::CHlsTransferSession::~CHlsTransferSession()
{
	m_transKey.FreeBuffer();

	if (m_dbRecord)
	{
		DSC_THREAD_TYPE_DELETE(m_dbRecord);
	}

	m_dbProposeItem.m_propose.FreeBuffer();
	m_dbProposeItem.m_sponsorUserKey.FreeBuffer();
}

void CVbeIdcHlsService::CHlsTransferSession::OnTimer(void)
{
	m_rHlsService.OnTimeOut(this);
}

void CVbeIdcHlsService::CHlsTransferSession::OnNetError(void)
{
	m_rHlsService.OnNetworkError(this);
}

CVbeIdcHlsService::CHlsCheckUserSession::CHlsCheckUserSession()
{
}


CVbeIdcHlsService::CHlsCheckUserQuerySession::CHlsCheckUserQuerySession()
{
}

CVbeIdcHlsService::CHlsTransferToHlsSession::CHlsTransferToHlsSession(CVbeIdcHlsService& rService)
	:IHlsBaseTimerSession(rService)
{
}

CVbeIdcHlsService::CHlsTransferToHlsSession::~CHlsTransferToHlsSession()
{
}

void CVbeIdcHlsService::CHlsTransferToHlsSession::OnTimer(void)
{
	m_rHlsService.OnTimeOut(this);
}

void CVbeIdcHlsService::CHlsTransferToHlsSession::OnNetError(void)
{
	m_rHlsService.OnNetworkError(this);
}

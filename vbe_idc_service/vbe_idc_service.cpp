#include <random>
#include "vbe_idc/vbe_idc_service/vbe_idc_service.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg_wrapper.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_user_util.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_cc_action_msg.h"
#include "vbe_idc/vbe_idc_service/vbh_agent_service.h"

#include "openssl/rand.h"
#include "openssl/err.h"

#include "dsc/db/per/persistence.h"
#include "dsc/dsc_database_factory.h"

/////////////////////////////////////////////////////////////////////
//db
class CLastBonus
{
public:
	CLastBonus()
		: m_userKey("USER_KEY")
		, m_lastReceiveDate("LAST_RECEIVE_DATE")
		, m_bonus_num("BONUS_NUM")
	{
	}

public:
	PER_BIND_ATTR(m_userKey, m_lastReceiveDate, m_bonus_num);

public:
	CColumnWrapper< CDscString > m_userKey;
	CColumnWrapper< CDBDate > m_lastReceiveDate;
	CColumnWrapper< double > m_bonus_num;
};

class CLastBonusCriterion : public CSelectCriterion
{
public:
	CLastBonusCriterion(const CDscString& userKey)
		: m_userKey(userKey)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where(rPerSelect["USER_KEY"] == m_userKey);
	}

private:
	CDscString m_userKey;
};

class CLastBonusUpdateCriterion : public CUpdateCriterion
{
public:
	CLastBonusUpdateCriterion(const CDscString& userKey) : m_userKey(userKey)
	{

	}

	virtual void SetCriterion(CPerUpdate& rPerUpdate)
	{
		rPerUpdate.Where((rPerUpdate["USER_KEY"] == m_userKey));
	}

private:
	CDscString m_userKey;
};

class CIDMHistory
{
public:
	CIDMHistory()
		: m_nCoinType("COIN_TYPE")
		, m_strOwner("OWNER")
		, m_ActionId("ACTION_ID")
		, m_nType("TYPE")
		, m_CreateTime("CREATE_TIME")
		, m_nIDMNum("NUM")
		, m_strTxId("TX_ID")
		, m_strAddr("ADDR")
	{
	}

public:
	PER_BIND_ATTR(m_nCoinType, m_strOwner, m_ActionId, m_nType, m_CreateTime, m_nIDMNum, m_strTxId, m_strAddr);

public:
	CColumnWrapper< ACE_UINT32 > m_nCoinType;    //币的类型   0:全部 1:IDM
	CColumnWrapper< CDscString > m_strOwner;       //这条记录的拥有着
	CColumnWrapper< ACE_UINT32 > m_ActionId;     //发生变化的action id
	CColumnWrapper< ACE_UINT32 > m_nType;        //类型: 0:全部 1:采收 2:转账 3:充币 4:提币 5: 奖励 6: 早鸟扣除
	CColumnWrapper< CDBDateTime > m_CreateTime;   //该记录的插入时间

	CColumnWrapper< double > m_nIDMNum;           //所涉及的IDM数目
	CColumnWrapper< CDscString >  m_strTxId;      //交易ID
	CColumnWrapper< CDscString >  m_strAddr;      //相关对方的地址或者账号
};

class CForceHistory
{
public:
	CForceHistory()
		: m_strOwner("OWNER")
		, m_ActionId("ACTION_ID")
		, m_nType("TYPE")
		, m_AwardTime("AWARD_TIME")
		, m_dForceNum("NUM")
		, m_strTxId("TX_ID")
	{
	}

public:
	PER_BIND_ATTR(m_strOwner, m_ActionId, m_nType, m_AwardTime, m_dForceNum, m_strTxId);

public:
	CColumnWrapper< CDscString > m_strOwner;       //这条记录的拥有着
	CColumnWrapper< ACE_UINT32 > m_ActionId;     //发生变化的action id
	CColumnWrapper< ACE_UINT32 > m_nType;        //类型: 1:注册 2:手机号验证 3:邮箱验证 4:抽奖 5: 关注公众号
	CColumnWrapper< CDBDateTime > m_AwardTime;   //奖励给该用户的时间

	CColumnWrapper< double > m_dForceNum;           //所涉及的Force数目
	CColumnWrapper< CDscString >  m_strTxId;      //交易ID
};

class CEarlyBirdHistory
{
public:
	CEarlyBirdHistory()
		: m_strOwner("OWNER")
		, m_ActionId("ACTION_ID")
		, m_nStatus("STATUS")
		, m_CreateTime("CREATE_TIME")
		, m_dDecBonusNum("DEC_NUM")
		, m_dIncBonusNum("INC_NUM")
		, m_strTxId("TX_ID")
		, m_SignTimeID("SIGN_TIME_ID")
		, m_SignTime("SIGN_TIME")
	{
	}

public:
	PER_BIND_ATTR(m_strOwner, m_ActionId, m_nStatus, m_CreateTime, m_dDecBonusNum, m_dIncBonusNum, m_strTxId, m_SignTimeID, m_SignTime);

public:
	CColumnWrapper< CDscString > m_strOwner;       //这条记录的拥有着
	CColumnWrapper< ACE_UINT32 > m_ActionId;     //发生变化的action id
	CColumnWrapper< ACE_UINT32 > m_nStatus;        //类型: 0:未参加  1:已参加
	CColumnWrapper< CDBDateTime > m_CreateTime;   //创建时间

	CColumnWrapper< double > m_dDecBonusNum;           //扣除的Bonus数目
	CColumnWrapper< double > m_dIncBonusNum;           //增加的Bonus数目
	CColumnWrapper< CDscString >  m_strTxId;      //交易ID
	CColumnWrapper< CDBDate > m_SignTimeID;        //类型: id查找标识
	CColumnWrapper< CDBDateTime > m_SignTime;   //签到时间
};

class CIfExistsFightCriterion : public CSelectCriterion
{
public:
	CIfExistsFightCriterion(const CDscString& owner, CDBDate signTimeID)
		: m_strOwner(owner)
		, m_SignTimeId(signTimeID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["OWNER"] == m_strOwner) && rPerSelect["SIGN_TIME_ID"] == m_SignTimeId);
	}

private:
	CDscString m_strOwner;
	CDBDate m_SignTimeId;
};

class CShareInfo
{
public:
	CShareInfo()
		: m_userID("USER_ID")
		, m_targetID("TARGET_ID")
		, m_shareStatus("SHARE_STATUS")
		, m_shareTime("SHARE_TIME")
		, m_shareInfo("SHARE_INFO")
		, m_targetName("TARGET_NAME")
		, m_sharerName("SHARER_NAME")
	{
	}

public:
	PER_BIND_ATTR(m_userID, m_targetID, m_shareStatus, m_shareTime, m_shareInfo, m_targetName, m_sharerName);

public:
	CColumnWrapper< CDscString > m_userID;
	CColumnWrapper< CDscString > m_targetID;
	CColumnWrapper< size_t > m_shareStatus;

	CColumnWrapper< CDBDateTime > m_shareTime;

	CColumnWrapper< CDscString > m_shareInfo;
	CColumnWrapper< CDscString > m_targetName;
	CColumnWrapper< CDscString > m_sharerName;
};

class CShareInfoUpdateCriterion : public CUpdateCriterion
{
public:
	CShareInfoUpdateCriterion(ACE_UINT64 sqlID) : m_nSqlID(sqlID)
	{

	}

	virtual void SetCriterion(CPerUpdate& rPerUpdate)
	{
		rPerUpdate.Where((rPerUpdate["SHARE_STATUS"] == 1) && (rPerUpdate["ID"] == m_nSqlID));
	}

private:
	ACE_UINT64 m_nSqlID;
};

class CQueryShareHistoryInfo
{
public:
	CQueryShareHistoryInfo()
		: m_shareTime("SHARE_TIME")
		, m_shareInfo("SHARE_INFO")
		, m_targetName("TARGET_NAME")
		, m_targetID("TARGET_ID")
		, m_sharerName("SHARER_NAME")
		, m_userID("USER_ID")
	{
	}

public:
	PER_BIND_ATTR(m_shareTime, m_shareInfo, m_targetName, m_targetID, m_sharerName, m_userID);

public:

	CColumnWrapper< CDBDateTime > m_shareTime;

	CColumnWrapper< CDscString > m_shareInfo;
	CColumnWrapper< CDscString > m_targetName;
	CColumnWrapper< CDscString > m_targetID;
	CColumnWrapper< CDscString > m_sharerName;
	CColumnWrapper< CDscString > m_userID;
};

class CQueryShareHistoryInfoCriterion : public CSelectCriterion
{
public:
	CQueryShareHistoryInfoCriterion(int nStartIndex, int nEndIndex, const CDscString& userID) : m_nStartIndex(nStartIndex),
		m_nEndIndex(nEndIndex), m_userID(userID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where(rPerSelect["USER_ID"] == m_userID);
		rPerSelect.Limit(m_nEndIndex - m_nStartIndex + 1, m_nStartIndex - 1);
		rPerSelect.OrderByDesc("SHARE_TIME");
	}

private:
	int m_nStartIndex;
	int m_nEndIndex;
	CDscString m_userID;
};

class CQueryLookHistoryInfoCriterion : public CSelectCriterion
{
public:
	CQueryLookHistoryInfoCriterion(int nStartIndex, int nEndIndex, const CDscString& userID) : m_nStartIndex(nStartIndex),
		m_nEndIndex(nEndIndex), m_userID(userID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where(rPerSelect["TARGET_ID"] == m_userID);
		rPerSelect.Limit(m_nEndIndex - m_nStartIndex + 1, m_nStartIndex - 1);
		rPerSelect.OrderByDesc("SHARE_TIME");
	}

private:
	int m_nStartIndex;
	int m_nEndIndex;
	CDscString m_userID;
};

class CIDMHistoryCriterion : public CSelectCriterion
{
public:
	CIDMHistoryCriterion(ACE_UINT32 nStartIndex, ACE_UINT32 nEndIndex, const CDscString& userID, ACE_UINT32 nCoinType, ACE_UINT32 nType)
		: m_nStartIndex(nStartIndex)
		, m_nEndIndex(nEndIndex)
		, m_userID(userID)
		, m_nCoinType(nCoinType)
		, m_nType(nType)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		if (m_nType == 0)
		{
			if (m_nCoinType == 0)
			{
				rPerSelect.Where(rPerSelect["OWNER"] == m_userID);
			}
			else
			{
				rPerSelect.Where((rPerSelect["OWNER"] == m_userID) && (rPerSelect["COIN_TYPE"] == m_nCoinType));
			}
		}
		else
		{
			if (m_nCoinType == 0)
			{
				rPerSelect.Where((rPerSelect["OWNER"] == m_userID) && (rPerSelect["TYPE"] == m_nType));
			}
			else
			{
				rPerSelect.Where((rPerSelect["OWNER"] == m_userID) && (rPerSelect["COIN_TYPE"] == m_nCoinType) && (rPerSelect["TYPE"] == m_nType));
			}
		}
		rPerSelect.Limit(m_nEndIndex - m_nStartIndex + 1, m_nStartIndex - 1);
		rPerSelect.OrderByDesc("CREATE_TIME");
	}

private:
	ACE_UINT32 m_nStartIndex;
	ACE_UINT32 m_nEndIndex;
	CDscString m_userID;
	ACE_UINT32 m_nCoinType;
	ACE_UINT32 m_nType;
};

class CForceHistoryCriterion : public CSelectCriterion
{
public:
	CForceHistoryCriterion(ACE_UINT32 nStartIndex, ACE_UINT32 nEndIndex, const CDscString& userID)
		: m_nStartIndex(nStartIndex)
		, m_nEndIndex(nEndIndex)
		, m_owner(userID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["OWNER"] == m_owner));
		rPerSelect.Limit(m_nEndIndex - m_nStartIndex + 1, m_nStartIndex - 1);
		rPerSelect.OrderByDesc("AWARD_TIME");
	}

private:
	ACE_UINT32 m_nStartIndex;
	ACE_UINT32 m_nEndIndex;
	CDscString m_owner;
};

class CIfEarlyBirdSignCriterion : public CSelectCriterion
{
public:
	CIfEarlyBirdSignCriterion(const CDscString& userID, CDBDate signTimeID)
		: m_userID(userID)
		, m_SignTimeId(signTimeID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["OWNER"] == m_userID) && (rPerSelect["SIGN_TIME_ID"] == m_SignTimeId));
	}

private:
	CDscString m_userID;
	CDBDate m_SignTimeId;
};

class CEarlyBirdUpdateCriterion : public CUpdateCriterion
{
public:
	CEarlyBirdUpdateCriterion(const CDscString& userID, CDBDate TimeID)
		: m_userID(userID)
		, m_TimeID(TimeID)
	{

	}

	virtual void SetCriterion(CPerUpdate& rPerUpdate)
	{
		rPerUpdate.Where((rPerUpdate["OWNER"] == m_userID) && (rPerUpdate["SIGN_TIME_ID"] == m_TimeID));
	}

private:
	CDscString m_userID;
	CDBDate m_TimeID;
};

class CEarlyBirdHistoryCriterion : public CSelectCriterion
{
public:
	CEarlyBirdHistoryCriterion(const CDscString& owner, CDBDate signTimeID)
		: m_owner(owner)
		, m_SignTimeID(signTimeID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["OWNER"] == m_owner) && (rPerSelect["SIGN_TIME_ID"] == m_SignTimeID));
		rPerSelect.OrderByDesc("CREATE_TIME");
	}

private:
	CDscString m_owner;
	CDBDate m_SignTimeID;        //类型: id查找标识
};

class CEarlyBirdPeopleCount
{
public:
	CEarlyBirdPeopleCount()
		: count("count(*)")
	{
	}

public:
	PER_BIND_ATTR(count);

public:
	CColumnWrapper< ACE_INT32 > count;           //所涉及的idm数目
};

class CEarlyBirdPeopleCountCriterion : public CSelectCriterion
{
public:
	CEarlyBirdPeopleCountCriterion(CDBDate signTimeID)
		: m_SignTimeID(signTimeID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where(rPerSelect["SIGN_TIME_ID"] == m_SignTimeID);
	}

private:
	CDBDate m_SignTimeID;        //类型: id查找标识
};

class CEarlyBirdRankList
{
public:
	CEarlyBirdRankList()
		: m_owner("OWNER")
		, m_dBonus("INC_NUM")
	{
	}

public:
	PER_BIND_ATTR(m_owner, m_dBonus);

public:
	CColumnWrapper< CDscString > m_owner;        //状态 获奖用户
	CColumnWrapper< double > m_dBonus;           //所涉及的idm数目
};

class CEarlyBirdRankListCriterion : public CSelectCriterion
{
public:
	CEarlyBirdRankListCriterion(CDBDate signTimeID, ACE_UINT32 nStatus, ACE_UINT32 nNum)
		: m_SignTimeID(signTimeID)
		, m_nStatus(nStatus)
		, m_nNum(nNum)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["SIGN_TIME_ID"] == m_SignTimeID) && (rPerSelect["STATUS"] == m_nStatus));
		rPerSelect.OrderByDesc("INC_NUM");
		rPerSelect.Limit(m_nNum);
	}

private:
	CDBDate m_SignTimeID;        //类型: id查找标识
	ACE_UINT32 m_nStatus;
	ACE_UINT32 m_nNum;
};

class CGetSignHistory
{
public:
	CGetSignHistory()
		: m_signTime("SIGN_TIME_ID")
		, m_nStatus("STATUS")
	{
	}

public:
	PER_BIND_ATTR(m_signTime, m_nStatus);

public:
	CColumnWrapper< CDBDate > m_signTime;
	CColumnWrapper<ACE_UINT32> m_nStatus;
};

class CGetSignHistoryCriterion : public CSelectCriterion
{
public:
	CGetSignHistoryCriterion(CDBDate signTimeID_0, CDBDate signTimeID_31, const CDscString& owner)
		: m_SignTimeID_0(signTimeID_0)
		, m_SignTimeID_31(signTimeID_31)
		, m_owner(owner)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["SIGN_TIME_ID"] > m_SignTimeID_0) && (rPerSelect["SIGN_TIME_ID"] <= m_SignTimeID_31) && (rPerSelect["OWNER"] == m_owner));
		//rPerSelect.OrderByDesc("INC_NUM");
	}

private:
	CDBDate m_SignTimeID_0;
	CDBDate m_SignTimeID_31;
	CDscString m_owner;
};

class CEBHDisRewardCriterion : public CSelectCriterion
{
public:
	CEBHDisRewardCriterion(CDBDate SignTimeID)
		: m_SignTimeId(SignTimeID)
	{

	}

	virtual void SetCriterion(CPerSelect& rPerSelect)
	{
		rPerSelect.Where((rPerSelect["STATUS"] == 1) && (rPerSelect["OBTAIN_STATUS"] == 0) && (rPerSelect["SIGN_TIME_ID"] == m_SignTimeId));
	}

private:
	CDBDate m_SignTimeId;        //签到日期
};

class CEarlyBirdHistoryDisReward
{
public:
	CEarlyBirdHistoryDisReward()
		: m_owner("OWNER")
		, m_nObtainStatus("OBTAIN_STATUS")
		, m_dIncBonusNum("INC_NUM")
	{
	}

public:
	PER_BIND_ATTR(m_owner, m_nObtainStatus, m_dIncBonusNum);

public:
	CColumnWrapper< CDscString > m_owner;             //获奖用户
	CColumnWrapper< ACE_UINT32 > m_nObtainStatus;        //是否获得过奖励，0 为否 1为是
	CColumnWrapper< double > m_dIncBonusNum;           //增加的Bonus数目
};

class CEBHistoryDisRewardUpdateCriterion : public CUpdateCriterion
{
public:
	CEBHistoryDisRewardUpdateCriterion(const CDscString& userID, CDBDate signTimeID)
		: m_owner(userID)
		, m_SignTimeId(signTimeID)
	{

	}

	virtual void SetCriterion(CPerUpdate& rPerUpdate)
	{
		rPerUpdate.Where((rPerUpdate["OWNER"] == m_owner) && (rPerUpdate["SIGN_TIME_ID"] == m_SignTimeId));
	}

private:
	CDscString m_owner;
	CDBDate m_SignTimeId;
};
/////////////////////////////////////////////////////////////////
#define EARLY_BIRD_IDM 0.014

CVbeIdcService::CVbeIdcService(const CDscString& strIpAddr, const ACE_INT32 nPort)
	: m_strIpAddr(strIpAddr)
	, m_nPort(nPort)
{
}

ACE_INT32 CVbeIdcService::OnInit(void)
{
	if (CDscHtsServerService::OnInit())
	{
		DSC_RUN_LOG_ERROR("bc endorser service init failed!");

		return -1;
	}

	if (m_hlsRouter.Open())
	{
		DSC_RUN_LOG_ERROR("hls service router init failed.");
		return -1;
	}

	m_pAcceptor = DSC_THREAD_TYPE_NEW(CMcpAsynchAcceptor<CVbeIdcService>) CMcpAsynchAcceptor<CVbeIdcService>(*this);
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

	// 添加数据库相关
	if (CDscDatabaseFactoryDemon::instance()->CreateDatabase(m_database, m_dbConnection))
	{
		DSC_RUN_LOG_ERROR("connect database failed.");
	}

	if (-1 == CDscConfigureDemon::instance()->GetDscProfileInt("IDOM_REWARD_TIME", m_RewardTime))
	{
		DSC_RUN_LOG_WARNING("cann't read 'IDOM_REWARD_TIME' configure item value");
		return -1;
	}

	if (-1 == CDscConfigureDemon::instance()->GetDscProfileInt("IDOM_SIGN_LOW_TIME", m_SignLowTime))
	{
		DSC_RUN_LOG_WARNING("cann't read 'IDOM_SIGN_LOW_TIME' configure item value");
		return -1;
	}

	if (-1 == CDscConfigureDemon::instance()->GetDscProfileInt("IDOM_SIGN_HIGH_TIME", m_SignHighTime))
	{
		DSC_RUN_LOG_WARNING("cann't read 'IDOM_SIGN_HIGH_TIME' configure item value");
		return -1;
	}

	m_pTimerDistributeReward = DSC_THREAD_TYPE_NEW(CDistributeRewardTimer) CDistributeRewardTimer(*this);
	if (m_pTimerDistributeReward)
	{
		this->SetDscTimer(m_pTimerDistributeReward, EN_DISTRIBUTE_REWARD_CYCLE);
	}

	if (-1 == CDscConfigureDemon::instance()->GetDscProfileString("IDOM_SYSTEM_CRYPT_KEY", m_strIDMCryptKey))
	{
		DSC_RUN_LOG_WARNING("cann't read 'IDOM_SYSTEM_CRYPT_KEY' configure item value");
	}

	if (-1 == CDscConfigureDemon::instance()->GetDscProfileString("IDOM_SYSTEM_KEY", m_strIDMUserID))
	{
		DSC_RUN_LOG_WARNING("cann't read 'IDOM_SHARE_TIME' configure item value");
		return -1;
	}

	DSC_RUN_LOG_INFO("vbe service init succeed!");

	return 0;
}

ACE_INT32 CVbeIdcService::OnExit(void)
{
	return 0;
}

void CVbeIdcService::SetVbhAgentService(CVbhAgentService* pSrv)
{
	m_pHas = pSrv;
}


void CVbeIdcService::OnQueryCryptUserKeyResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& cryptUserKey)
{
	auto pLoginSession = (CLoginSession*)EraseSession(m_mapLoginSession, nRequestID);

	if (pLoginSession)
	{
		char* decryptBuf = nullptr;
		ACE_UINT32 token = 0;
		DSC::CDscShortBlob nonce;

		if (nReturnCode == VBH::EN_OK_TYPE)
		{
			VBE_IDC::CLoginCltVbeReqDataWrapper wrapper(nonce);

			decryptBuf = VBE_IDC::DecryptWrapperMsg(wrapper, cryptUserKey, pLoginSession->m_nonce.c_str(), pLoginSession->m_nonce.size());
			if (decryptBuf)
			{
				auto onlineSession = DSC_THREAD_TYPE_NEW(COnlineUser) COnlineUser;

				token = onlineSession->m_nToken = NewToken();
				onlineSession->m_nChannelID = pLoginSession->m_nChannelID;
				onlineSession->m_userKey.Clone(pLoginSession->m_userKey);
				onlineSession->m_vbeUserKey.Clone(pLoginSession->m_vbeUserKey);
				onlineSession->m_cryptUserKey.Clone(cryptUserKey);

				SetDscTimer(onlineSession, EN_ONLINE_USER_TIMEOUT_VALUE);
				m_mapOnlineUsers.Insert(onlineSession->m_nToken, onlineSession);
			}
			else
			{
				nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
			}
			
		}
		ResponseLogin(nReturnCode, pLoginSession->m_nCltSessionID, pLoginSession->m_pServiceHandler->GetHandleID(), cryptUserKey, nonce, token);
		DSC_THREAD_TYPE_DELETE(pLoginSession);

		if (decryptBuf)
		{
			DSC_THREAD_FREE(decryptBuf);
		}
	}
}

//void CVbeIdcService::OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& cuserInfo)
//{
//	auto pGetUserInfoSession = (CGetUserInfoSession*)EraseSession(m_mapGetUserInfoSession, nRequestID);
//
//	if (pGetUserInfoSession)
//	{
//		if (nReturnCode)
//		{
//			DSC_RUN_LOG_INFO("OnQueryUserInfoResponse error:%d", nReturnCode);
//			return ResponseGetUserInfo(nReturnCode, pGetUserInfoSession->m_nCltSessionID, pGetUserInfoSession->m_pServiceHandler->GetHandleID());
//		}
//		
//		CDscString userInfo;
//		switch (pGetUserInfoSession->m_nQueryType)
//		{
//			case VBE_IDC::EN_IDC_QUERY_USER_INFO_TYPE:
//			{
//				userInfo.assign(cuserInfo.GetBuffer(), cuserInfo.GetSize());
//				break;
//			}
//			case VBE_IDC::EN_IDC_AUTH_USER_TYPE:
//			{
//				break;
//			}
//			case VBE_IDC::EN_IDC_QUERY_USER_ASSET_TYPE:
//			{
//				//查询用户资产  
//				/* @param userInfo out 
//				{
//					"IDMBalance": 10000.000000,
//						"ForceBalance" : 123.000000,
//						"IDMBonus" : 0.001400
//				}*/
//				onQueryAsset(pGetUserInfoSession, cuserInfo, userInfo);
//				break;
//			}
//			default:
//				break;
//		}
//
//		DSC::CDscShortBlob blobUserInfo(userInfo.data(), userInfo.size());
//		ResponseGetUserInfo(nReturnCode, pGetUserInfoSession->m_nCltSessionID, pGetUserInfoSession->m_pServiceHandler->GetHandleID(), pGetUserInfoSession->m_pOnlineUser->m_cryptUserKey, pGetUserInfoSession->m_nonce, 
//			blobUserInfo);
//		DSC_THREAD_TYPE_DELETE(pGetUserInfoSession);
//	}
//}

void CVbeIdcService::OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& cuserInfo)
{
	auto pVbhOperationSession = (CVbhOperationSession*)EraseSession(m_mapVbhOperationSession, nRequestID);

	if (pVbhOperationSession)
	{
		switch (pVbhOperationSession->m_nOperationType)
		{
		case VBE_IDC::EN_IDC_QUERY_USER_INFO_TYPE:
		{
			onQueryUserInfo(nReturnCode, pVbhOperationSession, cuserInfo);
			break;
		}
		case VBE_IDC::EN_IDC_AUTH_USER_TYPE:
		{
			//鉴权
			Authentication(nReturnCode, pVbhOperationSession, cuserInfo);
			break;
		}
		case VBE_IDC::EN_IDC_QUERY_USER_ASSET_TYPE:
		{
			//查询用户资产  
			onQueryAsset(nReturnCode, pVbhOperationSession, cuserInfo);
			break;
		}
		case VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE:
		{
			//收取idm奖励
			onReceiveBonus(nReturnCode, pVbhOperationSession, cuserInfo);
			break;
		}
		case VBE_IDC::EN_IDC_ADD_FORCE_TYPE:
		{
			//增加原力
			onAddForce(nReturnCode, pVbhOperationSession, cuserInfo);
			break;
		}
		case VBE_IDC::EN_IDC_ADD_TO_EARLY_BIRD_TYPE:
		{
			//参加早鸟签到
			onAdd2EarlyBird(nReturnCode, pVbhOperationSession, cuserInfo);
			break;
		}
		default:
			break;
		}
	}
	else
	{
		DSC_RUN_LOG_INFO("cann't find pVbhOperationSession, session id:%d ", nRequestID);
	}
}

void CVbeIdcService::OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo)
{
}

void CVbeIdcService::OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& transKey)
{
	auto pVbhOperationSession = m_mapVbhOperationSession.Find(nRequestID);

	if (pVbhOperationSession)
	{
		if (nReturnCode)
		{
			EraseSession(m_mapVbhOperationSession, pVbhOperationSession->m_nSessionID);
			ResponseUpdate(nReturnCode, nRequestID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
		}
		else
		{
			pVbhOperationSession->m_transKey.Clone(transKey);
		}
	}
}

void CVbeIdcService::OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	auto pVbhOperationSession = (CVbhOperationSession*)EraseSession(m_mapVbhOperationSession, nRequestID);

	if (pVbhOperationSession)
	{
		if (nReturnCode)
		{
			ResponseUpdate(nReturnCode, nRequestID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
			DSC_THREAD_TYPE_DELETE(pVbhOperationSession);
			return;
		}
	
		switch (pVbhOperationSession->m_nOperationType)
		{
			case VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE:
			{
				ResponseUpdate(nReturnCode, nRequestID, pVbhOperationSession->m_pServiceHandler->GetHandleID(), pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, pVbhOperationSession->m_nonce, pVbhOperationSession->m_transKey);
				break;
			}
			case VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE:
			{
				onUpdateRspBonus(pVbhOperationSession);
				ResponseUpdate(nReturnCode, nRequestID, pVbhOperationSession->m_pServiceHandler->GetHandleID(), pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, pVbhOperationSession->m_nonce, pVbhOperationSession->m_transKey);
				break;
			}
			case VBE_IDC::EN_IDC_ADD_FORCE_TYPE:
			{
				onUpdateRspAddForce(pVbhOperationSession);
				ResponseUpdate(nReturnCode, nRequestID, pVbhOperationSession->m_pServiceHandler->GetHandleID(), pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, pVbhOperationSession->m_nonce, pVbhOperationSession->m_transKey);
				break;
			}
			case VBE_IDC::EN_IDC_ADD_TO_EARLY_BIRD_TYPE:
			{
				onUpdateRspEarlyBird(pVbhOperationSession);
				ResponseUpdate(nReturnCode, nRequestID, pVbhOperationSession->m_pServiceHandler->GetHandleID(), pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, pVbhOperationSession->m_nonce, pVbhOperationSession->m_transKey);
				break;
			}
			default:
				break;
		}
		DSC_THREAD_TYPE_DELETE(pVbhOperationSession);

	}
	else
	{
		DSC_RUN_LOG_INFO("cann't find pVbhOperationSession, session id:%d ", nRequestID);
	}
}

void CVbeIdcService::ResponseLogin(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID)
{
	VBE_IDC::CLoginVbeCltRsp rsp;
	
	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	this->SendHtsMsg(rsp, nHandleID);
}

void CVbeIdcService::ResponseLogin(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID,const DSC::CDscShortBlob& envelopeKey, DSC::CDscShortBlob& nonce, ACE_UINT32 nToken)
{
	VBE_IDC::CLoginVbeCltRsp rsp;
	VBE_IDC::CLoginVbeCltRspDataWrapper wrapper(nonce, nToken);

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	EncryptSendResponse(rsp, wrapper, envelopeKey, nHandleID);
}

void CVbeIdcService::ResponseTransfer(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID)
{
	VBE_IDC::CTransferVbeCltRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	this->SendHtsMsg(rsp, nHandleID);
}

void CVbeIdcService::ResponseTransfer(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& transKey)
{
	VBE_IDC::CTransferVbeCltRsp rsp;
	VBE_IDC::CTransferVbeCltRspDataWrapper wrapper(nonce, transKey);

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	EncryptSendResponse(rsp, wrapper, enveloperKey, nHandleID);
}

void CVbeIdcService::ResponseUpdate(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID)
{
	VBE_IDC::CUpdateVbeCltRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	this->SendHtsMsg(rsp, nHandleID);
}

void CVbeIdcService::ResponseUpdate(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& updateKey)
{
	VBE_IDC::CUpdateVbeCltRsp rsp;
	VBE_IDC::CUpdateVbeCltRspDataWrapper wrapper(nonce, updateKey);

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	EncryptSendResponse(rsp, wrapper, enveloperKey, nHandleID);
}

void CVbeIdcService::ResponseIdcInterface(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID)
{
	VBE_IDC::CIdcInterfaceVbeCltRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	this->SendHtsMsg(rsp, nHandleID);
}

void CVbeIdcService::ResponseIdcInterface(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscBlob& updateKey)
{
	VBE_IDC::CIdcInterfaceVbeCltRsp rsp;
	VBE_IDC::CIdcInterfaceVbeCltRspDataWrapper wrapper(nonce, updateKey);

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	EncryptSendResponse(rsp, wrapper, enveloperKey, nHandleID);
}

void CVbeIdcService::ResponseGetUserInfo(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID)
{
	VBE_IDC::CGetUserInfoVbeCltRsp rsp;

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	this->SendHtsMsg(rsp, nHandleID);
}

void CVbeIdcService::ResponseGetUserInfo(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userinfo)
{
	VBE_IDC::CGetUserInfoVbeCltRsp rsp;
	VBE_IDC::CGetUserInfoVbeCltRspDataWrapper wrapper(nonce, userinfo);

	rsp.m_nReturnCode = nReturnCode;
	rsp.m_nSrcRequestID = nRequestID;

	EncryptSendResponse(rsp, wrapper, enveloperKey, nHandleID);

}

CMcpServerHandler* CVbeIdcService::AllocMcpHandler(ACE_HANDLE handle)
{
	return DSC_THREAD_DYNAMIC_TYPE_NEW(CVbeServiceHandler) CVbeServiceHandler(*this, handle, this->AllocHandleID());
}

ACE_UINT32 CVbeIdcService::NewToken()
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

ACE_INT32 CVbeIdcService::DoTransferPropose(CTransferSession* pTransferSession)
{
	CDscMsg::CDscMsgAddr addr;

	// 锁定用户已查找过hls，这里一定可以找到
	if (m_hlsRouter.GetHlsAddr(addr, pTransferSession->m_pOnlineUser->m_nChannelID))
	{
		DSC_RUN_LOG_ERROR("Not Fount User Channel %u", pTransferSession->m_pOnlineUser->m_nChannelID);
		return VBE_IDC::EN_SYSTEM_ERROR_TYPE;
	}

	VBE_IDC::CTransferVbeHlsReq req;

	req.m_bMain = true;
	req.m_nSrcRequestID = m_nSessionID;
	req.m_propose = pTransferSession->m_propose;
	req.m_sponsorUserKey = pTransferSession->m_pOnlineUser->m_userKey;

	if (SendDscMessage(req, addr))
	{
		return VBE_IDC::EN_NETWORK_ERROR_TYPE;
	}

	pTransferSession->m_nSessionID = m_nSessionID;

	SetDscTimer(pTransferSession, EN_SESSION_TIMEOUT_VALUE);

	m_mapTransferSession.DirectInsert(m_nSessionID++, pTransferSession);
	
	return 0;
}


void CVbeIdcService::ReleaseTransferSession(CTransferSession* pTransferSession)
{
	DSC_THREAD_TYPE_DELETE(pTransferSession);
}

void CVbeIdcService::ReleaseUpdateSession(CVbhOperationSession* pVbhOperationSession)
{
	DSC_THREAD_TYPE_DELETE(pVbhOperationSession);
}


ACE_INT32 CVbeIdcService::OnHtsMsg(VBE_IDC::CLoginCltVbeReq& rReq, CMcpHandler* pMcpHandler)
{
	DSC_RUN_LOG_FINE("CLoginCltVbeReq %s", rReq.m_data.c_str());

	DSC::CDscShortBlob vbhUserKey;
	ACE_UINT32 nChannelID;

	if (VBE_IDC::CVbeUserUtil::DecodeVbeUserKey(vbhUserKey, nChannelID, rReq.m_userKey))
	{
		return -1;
	}
	// TODO QueryCryptUserKey
	auto nReturnCode = m_pHas->QueryUserInfo(m_nSessionID, nChannelID, vbhUserKey);

	if (nReturnCode)
	{
		DSC_RUN_LOG_ERROR("QueryUserInfo failed.");
	}
	else
	{
		auto pSession = DSC_THREAD_TYPE_NEW(CLoginSession) CLoginSession(*this);

		pSession->m_nonce.Clone(rReq.m_data); // 等待解密
		pSession->m_nCltSessionID = rReq.m_nSrcRequestID;
		pSession->m_userKey.Clone(vbhUserKey);
		pSession->m_vbeUserKey.Clone(rReq.m_userKey);
		pSession->m_nChannelID = nChannelID;
		InsertSession(m_mapLoginSession, pSession, pMcpHandler);
	}
	DSC_RUN_LOG_FINE("CLoginCltVbeReq out", rReq.m_data.c_str());
	return 0;
}

ACE_INT32  CVbeIdcService::OnHtsMsg(VBE_IDC::CUpdateCltVbeReq& rUpdateCltVbeReq, CMcpHandler* pMcpHandler)
{
	auto pOnlineUser = m_mapOnlineUsers.Find(rUpdateCltVbeReq.m_nToken);

	if (!pOnlineUser)
	{
		ResponseUpdate(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rUpdateCltVbeReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}

	ACE_INT32 nReturnCode = VBE_IDC::EN_OK_TYPE;

	DSC::CDscShortBlob nonce;
	DSC::CDscShortBlob userInfo;
	ACE_UINT32 nActionID;
	ACE_UINT32 nUpdateType;
	VBE_IDC::CUpdateCltVbeReqDataWrapper wrapper(nonce, userInfo, nActionID, nUpdateType);
	char* decryptBuf = VBE_IDC::DecryptWrapperMsg(wrapper, pOnlineUser->m_cryptUserKey, rUpdateCltVbeReq.m_data.c_str(), rUpdateCltVbeReq.m_data.size());

	if (!decryptBuf)
	{
		ResponseUpdate(VBE_IDC::EN_DECRYPT_ERROR_TYPE, rUpdateCltVbeReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}

	switch (nUpdateType)
	{
		case VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE:
		{
			nReturnCode = m_pHas->Propose(m_nSessionID, pOnlineUser->m_nChannelID, true,
				VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, pOnlineUser->m_userKey, userInfo);
			break;
		}
		case VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE:
		{
			nReturnCode = m_pHas->QueryUserInfo(m_nSessionID, pOnlineUser->m_nChannelID, pOnlineUser->m_userKey);
			break;
		}
		case VBE_IDC::EN_IDC_ADD_FORCE_TYPE:
		{
			nReturnCode = m_pHas->QueryUserInfo(m_nSessionID, pOnlineUser->m_nChannelID, pOnlineUser->m_userKey);
			break;
		}
		case VBE_IDC::EN_IDC_TRANS_TYPE:
		{
			//转账
			VBE_IDC::CTransInfoCltIdc transInfoCltIdc;
			DSC::Decode(transInfoCltIdc, userInfo.GetBuffer(), userInfo.GetSize());

			VBE_IDC::CTransInfoIdcVbh transInfoIdcVbh;
			transInfoIdcVbh.m_dIDMNum = transInfoCltIdc.m_dIDMNum;
			transInfoIdcVbh.m_strUserKey.assign(pOnlineUser->m_userKey.GetBuffer(), pOnlineUser->m_userKey.GetSize());
			transInfoIdcVbh.m_strTargetKey = transInfoCltIdc.m_strTargetKey;

			char* pProposal = NULL;
			size_t nProposalLen;
			DSC::Encode(transInfoIdcVbh, pProposal, nProposalLen);
			DSC::CDscShortBlob transInfo;
			transInfo.Set(pProposal, nProposalLen);

			nReturnCode = m_pHas->Propose(m_nSessionID, pOnlineUser->m_nChannelID, true,
				VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, pOnlineUser->m_userKey, transInfo);
			DSC_THREAD_SIZE_FREE(pProposal, nProposalLen);
			break;
		}
		case VBE_IDC::EN_IDC_ADD_TO_EARLY_BIRD_TYPE:
		{
			CDscString userKey(pOnlineUser->m_userKey.GetBuffer(), pOnlineUser->m_userKey.GetSize());
			if (IfExistsEarlyBirdHistory(userKey))
			{
				ResponseUpdate(VBE_IDC::EN_ALREADY_ENTER_EARLY_BIRD_ERROR_TYPE, rUpdateCltVbeReq.m_nSrcRequestID, pMcpHandler->GetHandleID());
				return VBE_IDC::EN_ALREADY_ENTER_EARLY_BIRD_ERROR_TYPE;
			}
			nReturnCode = m_pHas->QueryUserInfo(m_nSessionID, pOnlineUser->m_nChannelID, pOnlineUser->m_userKey);
			break;
		}
		default:
			break;
	}
	
	if (nReturnCode == VBE_IDC::EN_OK_TYPE)
	{
		//DSC::CDscShortBlob nonce;
		//VBE_IDC::CGetUserInfoCltVbeReqDataWrapper wrapper(nonce);
		//char* decryptBuf = DecryptWrapperMsg(wrapper, pOnlineUser->m_cryptUserKey, rGetUserInfoReq.m_data.c_str(), rGetUserInfoReq.m_data.size());

		//if (decryptBuf)
		//{
			auto pUpdateSession = DSC_THREAD_TYPE_NEW(CVbhOperationSession) CVbhOperationSession(*this);

			DSC::CDscShortBlob tmpUserKey;

			tmpUserKey.Clone(pOnlineUser->m_userKey);

			pUpdateSession->m_nonce.Clone(nonce);
			pUpdateSession->m_pOnlineUser = pOnlineUser;
			pUpdateSession->m_nOperationType = nUpdateType;
			pUpdateSession->m_userInfo.Clone(userInfo);
			pUpdateSession->m_nCltSessionID = rUpdateCltVbeReq.m_nSrcRequestID;
			InsertSession(m_mapVbhOperationSession, pUpdateSession, pMcpHandler);
			DSC_THREAD_FREE(decryptBuf);
			//this->ResetDscTimer(pOnlineUser, EN_ONLINE_USER_TIMEOUT_VALUE);
		//}
		//else
		//{
		//	nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
		//}
	}
	return 0;

	// 组装propose
	//VBE_IDC::CVbeIdcCcActionUpdateMsg msg;

	//msg.m_sponsorUserKey = pUpdateSession->m_pOnlineUser->m_vbeUserKey;
	//msg.m_nUpdateType = m_nUpdateType;
	//msg.m_userInfo = m_userInfo;

	//char* proposeBuf = nullptr;
	//size_t proposeBufLen = 0;

	//DSC::Encode(msg, proposeBuf, proposeBufLen);
	//pUpdateSession->m_propose.Set(proposeBuf, proposeBufLen);


	//DoUpdatePropose(pUpdateSession);
}

ACE_INT32  CVbeIdcService::OnHtsMsg(VBE_IDC::CIdcInterfaceCltVbeReq& rIdcInterfaceCltVbeReq, CMcpHandler* pMcpHandler)
{
	auto pOnlineUser = m_mapOnlineUsers.Find(rIdcInterfaceCltVbeReq.m_nToken);

	if (!pOnlineUser)
	{
		ResponseIdcInterface(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rIdcInterfaceCltVbeReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}

	ACE_INT32 nReturnCode = VBE_IDC::EN_OK_TYPE;

	DSC::CDscShortBlob nonce;
	DSC::CDscShortBlob userInfo;
	ACE_UINT32 nInterfaceType;
	VBE_IDC::CIdcInterfaceCltVbeReqDataWrapper wrapper(nonce, userInfo, nInterfaceType);
	char* decryptBuf = VBE_IDC::DecryptWrapperMsg(wrapper, pOnlineUser->m_cryptUserKey, rIdcInterfaceCltVbeReq.m_data.c_str(), rIdcInterfaceCltVbeReq.m_data.size());

	if (!decryptBuf)
	{
		ResponseIdcInterface(VBE_IDC::EN_DECRYPT_ERROR_TYPE, rIdcInterfaceCltVbeReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}

	//将请求的按照类型分发
	CDscString retInfo;
	nReturnCode = TypeProc(nInterfaceType, userInfo, pMcpHandler, pOnlineUser->m_userKey, retInfo);
	
	DSC::CDscBlob info(retInfo.data(), retInfo.size());
	ResponseIdcInterface(nReturnCode, rIdcInterfaceCltVbeReq.m_nSrcRequestID, pMcpHandler->GetHandleID(), 
		pOnlineUser->m_cryptUserKey, nonce, info);

	return 0;
}

ACE_INT32 CVbeIdcService::OnHtsMsg(VBE_IDC::CTransferCltVbeReq& rTransferReq, CMcpHandler* pMcpHandler)
{
	auto pOnlineUser = m_mapOnlineUsers.Find(rTransferReq.m_nToken);

	if (!pOnlineUser)
	{
		ResponseTransfer(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}
	
	DSC::CDscShortBlob nonce;
	ACE_UINT32 nTotalCoin;
	DSC::CDscShortList<DSC::CDscShortBlob> userKeyList;
	DSC::CDscShortList<ACE_UINT32> coinList;
	VBE_IDC::CTransferCltVbeReqDataWrapper wrapper(nonce, nTotalCoin, userKeyList, coinList);
	char* decryptBuf = VBE_IDC::DecryptWrapperMsg(wrapper, pOnlineUser->m_cryptUserKey, rTransferReq.m_data.c_str(), rTransferReq.m_data.size());

	if (!decryptBuf)
	{
		ResponseTransfer(VBE_IDC::EN_DECRYPT_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}

	if (coinList.size() != userKeyList.size() || coinList.size() == 0)
	{
		ResponseTransfer(VBE_IDC::EN_PARAM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}

	// 先把接收方按channel分组
	auto pTransferSession = DSC_THREAD_TYPE_NEW(CTransferSession) CTransferSession(*this);
	auto coinIt = coinList.begin();

	pTransferSession->m_nTotalCoin = 0;
	for (auto& userKey : userKeyList)
	{
		DSC::CDscShortBlob vbhUserKey;
		ACE_UINT32 nChannelID;

		if (VBE_IDC::CVbeUserUtil::DecodeVbeUserKey(vbhUserKey, nChannelID, userKey))
		{
			ResponseTransfer(VBE_IDC::EN_PARAM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());
			DSC_THREAD_TYPE_DELETE(pTransferSession);

			return 0;
		}

		auto receiverListIt = pTransferSession->m_mapReceiverList.find(nChannelID);
		CTransferSession::CReceiverList* receiverList;

		if (receiverListIt == pTransferSession->m_mapReceiverList.end())
		{
			receiverList = DSC_THREAD_TYPE_NEW(CTransferSession::CReceiverList) CTransferSession::CReceiverList;
			receiverList->m_bChecked = false;
			receiverList->m_bSentCheck = false;
			pTransferSession->m_mapReceiverList[nChannelID] = receiverList;
		}
		else
		{
			receiverList = receiverListIt->second;
		}

		DSC::CDscShortBlob tmpUserKey;

		tmpUserKey.Clone(vbhUserKey);

		receiverList->m_lstReceiver.push_back(tmpUserKey);
		pTransferSession->m_nTotalCoin += *coinIt;
		coinIt++;
	}

	if (pTransferSession->m_nTotalCoin != nTotalCoin)
	{
		DSC_THREAD_TYPE_DELETE(pTransferSession);
		ResponseTransfer(VBE_IDC::EN_PARAM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

		return 0;
	}
	///把发起者放到receiver 中，方便查询
	auto receiverListIt = pTransferSession->m_mapReceiverList.find(pOnlineUser->m_nChannelID);
	CTransferSession::CReceiverList* receiverList;

	if (receiverListIt == pTransferSession->m_mapReceiverList.end())
	{
		receiverList = DSC_THREAD_TYPE_NEW(CTransferSession::CReceiverList) CTransferSession::CReceiverList;
		receiverList->m_bChecked = false;
		receiverList->m_bSentCheck = false;
		pTransferSession->m_mapReceiverList[pOnlineUser->m_nChannelID] = receiverList;
	}
	else
	{
		receiverList = receiverListIt->second;
	}

	DSC::CDscShortBlob tmpUserKey;

	tmpUserKey.Clone(pOnlineUser->m_userKey);

	receiverList->m_lstReceiver.push_back(tmpUserKey);

	/// 
	pTransferSession->m_nonce.Clone(nonce);
	pTransferSession->m_pOnlineUser = pOnlineUser;
	this->ResetDscTimer(pOnlineUser, EN_ONLINE_USER_TIMEOUT_VALUE);

// 组装propose
	VBE_IDC::CVbeIdcCcActionTransferMsg msg;

	msg.m_nTotalCoin = nTotalCoin;
	msg.m_sponsorUserKey = pTransferSession->m_pOnlineUser->m_vbeUserKey;
	msg.m_lstUserKey = userKeyList;
	msg.m_lstCoin = coinList;

	char* proposeBuf = nullptr;
	size_t proposeBufLen = 0;

	DSC::Encode(msg, proposeBuf, proposeBufLen);
	pTransferSession->m_propose.Set(proposeBuf, proposeBufLen);

	if (pTransferSession->m_mapReceiverList.size() == 1)
	{
		// channel 内交易，直接提案
		DoTransferPropose(pTransferSession);
	}
	else
	{
		// channel 间交易， 先查用户是否有效
		for (auto it = pTransferSession->m_mapReceiverList.begin(); it != pTransferSession->m_mapReceiverList.end(); it++)
		{
			VBE_IDC::CCheckUsersVbeHlsReq req;

			req.m_nSrcRequestID = m_nSessionID;

			for (auto& receiver : it->second->m_lstReceiver)
			{
				req.m_lstUserKey.push_back(receiver);
			}

			CDscMsg::CDscMsgAddr addr;

			if (m_hlsRouter.GetHlsAddr(addr, it->first))
			{
				ReleaseTransferSession(pTransferSession);
				ResponseTransfer(VBE_IDC::EN_SYSTEM_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

				return 0;
			}

			if (SendDscMessage(req, addr))
			{
				ReleaseTransferSession(pTransferSession);
				ResponseTransfer(VBE_IDC::EN_NETWORK_ERROR_TYPE, rTransferReq.m_nSrcRequestID, pMcpHandler->GetHandleID());

				return 0;
			}

			it->second->m_bSentCheck = true;

			auto pCheckSession = DSC_THREAD_TYPE_NEW(CCheckUserSession) CCheckUserSession(*this);

			pCheckSession->m_pTransferSession = pTransferSession;
			pCheckSession->m_nChannelID = it->first;

			InsertSession(m_mapCheckUserSession, pCheckSession, pMcpHandler);
		}
	}

	pTransferSession->m_pServiceHandler = (CVbeServiceHandler*)pMcpHandler;
	pTransferSession->m_pServiceHandler->m_arrUserSession.Insert(pTransferSession);

	DSC_THREAD_FREE(decryptBuf);

	return 0;
}

ACE_INT32 CVbeIdcService::OnHtsMsg(VBE_IDC::CGetUserInfoCltVbeReq& rGetUserInfoReq, CMcpHandler* pMcpHandler)
{
	auto pOnlineUser = m_mapOnlineUsers.Find(rGetUserInfoReq.m_nToken);
	ACE_INT32 nReturnCode = VBE_IDC::EN_OK_TYPE;

	if (pOnlineUser)
	{
		nReturnCode = m_pHas->QueryUserInfo(m_nSessionID, pOnlineUser->m_nChannelID, pOnlineUser->m_userKey);

		if (nReturnCode == VBE_IDC::EN_OK_TYPE)
		{
			DSC::CDscShortBlob nonce;
			VBE_IDC::CGetUserInfoCltVbeReqDataWrapper wrapper(nonce);
			char* decryptBuf = DecryptWrapperMsg(wrapper, pOnlineUser->m_cryptUserKey, rGetUserInfoReq.m_data.c_str(), rGetUserInfoReq.m_data.size());

			if (decryptBuf)
			{
				auto pGetUserInfoSession = DSC_THREAD_TYPE_NEW(CVbhOperationSession) CVbhOperationSession(*this);

				pGetUserInfoSession->m_nonce.Clone(nonce);
				pGetUserInfoSession->m_nOperationType = rGetUserInfoReq.m_nQueryType;
				pGetUserInfoSession->m_pOnlineUser = pOnlineUser;
				pGetUserInfoSession->m_nCltSessionID = rGetUserInfoReq.m_nSrcRequestID;
				InsertSession(m_mapVbhOperationSession, pGetUserInfoSession, pMcpHandler);

				DSC_THREAD_FREE(decryptBuf);
			}
			else
			{
				nReturnCode = VBE_IDC::EN_DECRYPT_ERROR_TYPE;
			}
		}
	}
	else
	{
		nReturnCode = VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE;
	}
	
	return 0;
}

void CVbeIdcService::OnDscMsg(VBE_IDC::CCheckUsersHlsVbeRsp& rCheckUserRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pCheckUserSession = (CCheckUserSession*)EraseSession(m_mapCheckUserSession, rCheckUserRsp.m_nSrcRequestID);

	if (pCheckUserSession)
	{
		if (rCheckUserRsp.m_nReturnCode)
		{
			ResponseTransfer(rCheckUserRsp.m_nReturnCode, pCheckUserSession->m_pTransferSession->m_nCltSessionID, pCheckUserSession->m_pTransferSession->m_pServiceHandler->GetHandleID());
			ReleaseTransferSession(pCheckUserSession->m_pTransferSession);
		}
		else
		{
			ACE_UINT32 checkedCount = 0;

			for (auto& it : pCheckUserSession->m_pTransferSession->m_mapReceiverList)
			{
				if (it.first == pCheckUserSession->m_nChannelID)
				{
					it.second->m_bChecked = true;
				}

				if (it.second->m_bChecked)
				{
					checkedCount++;
				}
			}

			if (checkedCount == pCheckUserSession->m_pTransferSession->m_mapReceiverList.size())
			{
				DoTransferPropose(pCheckUserSession->m_pTransferSession);
			}
		}

		DSC_THREAD_TYPE_DELETE(pCheckUserSession);
	}
}

void CVbeIdcService::OnDscMsg(VBE_IDC::CTransferHlsVbeRsp& rTransferRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pTransferSession = m_mapTransferSession.Find(rTransferRsp.m_nSrcRequestID);

	if (pTransferSession)
	{
		if (rTransferRsp.m_nReturnCode)
		{
			ResponseTransfer(rTransferRsp.m_nReturnCode, rTransferRsp.m_nSrcRequestID, pTransferSession->m_pServiceHandler->GetHandleID());
		}
		else
		{
			ResponseTransfer(rTransferRsp.m_nReturnCode, rTransferRsp.m_nSrcRequestID, pTransferSession->m_pServiceHandler->GetHandleID(), pTransferSession->m_pOnlineUser->m_cryptUserKey, pTransferSession->m_nonce, rTransferRsp.m_transKey);
			ReleaseTransferSession(pTransferSession);
		}
	}
}

void CVbeIdcService::OnDscMsg(VBE_IDC::CIDChainAuthNotifyRsp& rIDChainAuthNotifyRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	auto pVbhOperationSession = (CVbhOperationSession*)EraseSession(m_mapVbhOperationSession, rIDChainAuthNotifyRsp.m_nSessionID);

	if (pVbhOperationSession)
	{
	
		ResponseGetUserInfo(0, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID());

		DSC_THREAD_TYPE_DELETE(pVbhOperationSession);
	}
	else
	{
		DSC_RUN_LOG_INFO("cann't find pVbhOperationSession, session id:%d ", rIDChainAuthNotifyRsp.m_nSessionID);
	}
}

///////////////////////////////////////////////////////////////////////////////////
// idc业务相关处理逻辑
void CVbeIdcService::onQueryUserInfo(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo)
{
	if (nReturnCode)
	{
		DSC_RUN_LOG_INFO("OnQueryUserInfoResponse error:%d", nReturnCode);
		return ResponseGetUserInfo(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
	}

	DSC::CDscShortBlob userInfo;
	userInfo.Set(oriUserInfo.GetBuffer(), oriUserInfo.GetSize());
	ResponseGetUserInfo(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID(), pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, pVbhOperationSession->m_nonce,
		userInfo);
	DSC_THREAD_TYPE_DELETE(pVbhOperationSession);
}

void CVbeIdcService::Authentication(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& cuserInfo)
{
	char arrUuid[DSC_UUID_LEN];

	DSC::Uuid(arrUuid);

	VBE_IDC::CIDChainAuthNotifyReq authNotify;

	authNotify.m_token.Set(arrUuid, DSC_UUID_LEN);
	authNotify.m_nSessionID = pVbhOperationSession->m_nSessionID;

	CDscMsg::CDscMsgAddr addr(VBE_IDC::EN_FINE_FOOD_SERVICE_TYPE, CDscAppManager::Instance()->GetNodeID(), VBE_IDC::EN_VBE_FINE_FOOD_SERVICE_APP_TYPE, DSC::EN_INVALID_ID);
	if (this->SendDscMessage(authNotify, addr))
	{
		DSC_RUN_LOG_INFO("SendDscMessage failed");
	}
}

void CVbeIdcService::onQueryAsset(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo)
{
	if (nReturnCode)
	{
		DSC_RUN_LOG_INFO("OnQueryUserInfoResponse error:%d", nReturnCode);
		return ResponseGetUserInfo(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
	}

	SJsonReader* pJsonKeyReader = ::create_json_reader(oriUserInfo.GetBuffer(), oriUserInfo.GetSize());
	if (pJsonKeyReader == NULL)
	{
		DSC_RUN_LOG_ERROR("onQueryAsset create_json_reader error, userInfo:%s", oriUserInfo.GetBuffer());
		return;
	}
	
	VBE_IDC::CJsonIDMUserOriInfo jsonInfo;
	if (JSON_CODE::Decode(jsonInfo, pJsonKeyReader))
	{
		DSC_RUN_LOG_ERROR("onQueryAsset  Decode jsonInfo failed");
		json_reader_free(pJsonKeyReader);
		return;
	}

	VBE_IDC::CJsonIDMAssetInfo assetInfo;
	assetInfo.IDMBonus = BonusNums(pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, jsonInfo.force);
	assetInfo.ForceBalance = jsonInfo.force;
	assetInfo.IDMBalance = jsonInfo.idm;

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(assetInfo);
	CDscString userInfo;
	if (pJsonKeyWriter)
	{
		userInfo.append(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		DSC_RUN_LOG_ERROR("onQueryAsset encode assetInfo Encode ");
	}
	json_reader_free(pJsonKeyReader);

	DSC::CDscShortBlob blobUserInfo;
	blobUserInfo.Set(userInfo.data(), userInfo.size());
	ResponseGetUserInfo(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID(), pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, pVbhOperationSession->m_nonce,
		blobUserInfo);
	DSC_THREAD_TYPE_DELETE(pVbhOperationSession);
}

void CVbeIdcService::onReceiveBonus(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo)
{
	if (nReturnCode)
	{
		DSC_RUN_LOG_INFO("onReceiveBonus error:%d", nReturnCode);
		return ResponseUpdate(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
	}
	//查询用户信息之后发起更新提案
	SJsonReader* pJsonKeyReader = ::create_json_reader(oriUserInfo.GetBuffer(), oriUserInfo.GetSize());
	if (pJsonKeyReader == NULL)
	{
		DSC_RUN_LOG_ERROR("onQueryAsset create_json_reader error, userInfo:%s", oriUserInfo.GetBuffer());
		return;
	}

	VBE_IDC::CJsonIDMUserOriInfo jsonInfo;
	if (JSON_CODE::Decode(jsonInfo, pJsonKeyReader))
	{
		DSC_RUN_LOG_ERROR("onQueryAsset  Decode jsonInfo failed");
		json_reader_free(pJsonKeyReader);
		return;
	}

	double dBonusNums = ACE_OS::atof(pVbhOperationSession->m_userInfo.GetBuffer());
	jsonInfo.idm = jsonInfo.idm + dBonusNums;

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(jsonInfo);
	DSC::CDscShortBlob propose;
	if (pJsonKeyWriter)
	{
		propose.Set(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		DSC_RUN_LOG_ERROR("onQueryAsset encode assetInfo Encode ");
	}
	m_pHas->Propose(m_nSessionID, pVbhOperationSession->m_pOnlineUser->m_nChannelID, true,
		VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, pVbhOperationSession->m_pOnlineUser->m_userKey, propose);

	json_reader_free(pJsonKeyReader);
}

void CVbeIdcService::onAddForce(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo)
{
	if (nReturnCode)
	{
		DSC_RUN_LOG_INFO("onReceiveBonus error:%d", nReturnCode);
		return ResponseUpdate(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
	}
	//查询用户信息之后发起更新提案
	SJsonReader* pJsonKeyReader = ::create_json_reader(oriUserInfo.GetBuffer(), oriUserInfo.GetSize());
	if (pJsonKeyReader == NULL)
	{
		DSC_RUN_LOG_ERROR("onQueryAsset create_json_reader error, userInfo:%s", oriUserInfo.GetBuffer());
		return;
	}

	VBE_IDC::CJsonIDMUserOriInfo jsonInfo;
	if (JSON_CODE::Decode(jsonInfo, pJsonKeyReader))
	{
		DSC_RUN_LOG_ERROR("onQueryAsset  Decode jsonInfo failed");
		json_reader_free(pJsonKeyReader);
		return;
	}
	//获取信息 解码 然后增加原力
	VBE_IDC::CIDCAddForce addForce;
	DSC::Decode(addForce, pVbhOperationSession->m_userInfo.GetBuffer(), pVbhOperationSession->m_userInfo.GetSize());
	
	jsonInfo.force = jsonInfo.force + addForce.m_nForce;

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(jsonInfo);
	DSC::CDscShortBlob propose;
	if (pJsonKeyWriter)
	{
		propose.Set(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		DSC_RUN_LOG_ERROR("onQueryAsset encode assetInfo Encode ");
	}
	m_pHas->Propose(m_nSessionID, pVbhOperationSession->m_pOnlineUser->m_nChannelID, true,
		VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, pVbhOperationSession->m_pOnlineUser->m_userKey, propose);

	json_reader_free(pJsonKeyReader);
}

void CVbeIdcService::onAdd2EarlyBird(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo)
{
	if (nReturnCode)
	{
		DSC_RUN_LOG_INFO("onReceiveBonus error:%d", nReturnCode);
		return ResponseUpdate(nReturnCode, pVbhOperationSession->m_nCltSessionID, pVbhOperationSession->m_pServiceHandler->GetHandleID());
	}
	//查询用户信息之后发起更新提案
	SJsonReader* pJsonKeyReader = ::create_json_reader(oriUserInfo.GetBuffer(), oriUserInfo.GetSize());
	if (pJsonKeyReader == NULL)
	{
		DSC_RUN_LOG_ERROR("onQueryAsset create_json_reader error, userInfo:%s", oriUserInfo.GetBuffer());
		return;
	}

	VBE_IDC::CJsonIDMUserOriInfo jsonInfo;
	if (JSON_CODE::Decode(jsonInfo, pJsonKeyReader))
	{
		DSC_RUN_LOG_ERROR("onQueryAsset  Decode jsonInfo failed");
		json_reader_free(pJsonKeyReader);
		return;
	}

	double dBonusNums = ACE_OS::atof(pVbhOperationSession->m_userInfo.GetBuffer());
	jsonInfo.idm = jsonInfo.idm - EARLY_BIRD_IDM;

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(jsonInfo);
	DSC::CDscShortBlob propose;
	if (pJsonKeyWriter)
	{
		propose.Set(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		DSC_RUN_LOG_ERROR("onQueryAsset encode assetInfo Encode ");
	}
	m_pHas->Propose(m_nSessionID, pVbhOperationSession->m_pOnlineUser->m_nChannelID, true,
		VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, pVbhOperationSession->m_pOnlineUser->m_userKey, propose);

	json_reader_free(pJsonKeyReader);
}

void CVbeIdcService::onUpdateRspEarlyBird(CVbhOperationSession* pVbhOperationSession)
{
	CDscString strTxid(pVbhOperationSession->m_transKey.GetBuffer(), pVbhOperationSession->m_transKey.GetSize());
	CDscString strAddr("systemKey");
	CDscString ownerKey(pVbhOperationSession->m_pOnlineUser->m_userKey.GetBuffer(),
		pVbhOperationSession->m_pOnlineUser->m_userKey.GetSize());

	InsertEarlyBirdHistory(ownerKey, VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE, 0, 0.014, 0.0, strTxid);
	InsertIDMHistory(1, ownerKey, VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE, 6, -0.014, strTxid, strAddr);
}

void CVbeIdcService::onUpdateRspAddForce(CVbhOperationSession* pVbhOperationSession)
{
	VBE_IDC::CIDCAddForce addForce;
	DSC::Decode(addForce, pVbhOperationSession->m_userInfo.GetBuffer(), pVbhOperationSession->m_userInfo.GetSize());
	CDscString ownerKey(pVbhOperationSession->m_pOnlineUser->m_userKey.GetBuffer(),
		pVbhOperationSession->m_pOnlineUser->m_userKey.GetSize());
	CDscString strTxid(pVbhOperationSession->m_transKey.GetBuffer(), pVbhOperationSession->m_transKey.GetSize());

	ACE_INT32 ret = InsertForceHistory(ownerKey, VBE_IDC::EN_IDC_ADD_FORCE_TYPE,
		addForce.m_nType, addForce.m_nForce, strTxid);
}

void CVbeIdcService::onUpdateRspBonus(CVbhOperationSession* pVbhOperationSession)
{
	double dIdmNum = ACE_OS::atof(pVbhOperationSession->m_userInfo.GetBuffer());
	ACE_INT32 retCode = UpdateLastBonus(pVbhOperationSession->m_pOnlineUser->m_cryptUserKey, dIdmNum);
	if (retCode == 0)
	{
		CDscString strTxid(pVbhOperationSession->m_transKey.GetBuffer(), pVbhOperationSession->m_transKey.GetSize());
		CDscString strAddr("systemKey");
		CDscString ownerKey(pVbhOperationSession->m_pOnlineUser->m_userKey.GetBuffer(),
			pVbhOperationSession->m_pOnlineUser->m_userKey.GetSize());
		//DSC::CDscShortBlob addr(strAddr.data(), strAddr.size());
		InsertIDMHistory(1, ownerKey, VBE_IDC::EN_IDC_RECEIVE_BONUS_TYPE,
			1, dIdmNum, strTxid, strAddr);
	}
	else
	{
		//todo 回滚
	}
}

bool CVbeIdcService::IfExistsEarlyBirdHistory(const CDscString& owner)
{
	CTableWrapper< CEarlyBirdHistory>  tEarlyBirdHistory("T_EARLY_BIRD_HISTORY");

	CDBDate time = CDBDate::Today(this->GetCurTime() + 60 * 60 * 24);

	CIfExistsFightCriterion criterionQuery(owner, time);

	if (::PerSelect(tEarlyBirdHistory, m_database, m_dbConnection, &criterionQuery))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed, or no result");

		return false;
	}

	if (*tEarlyBirdHistory->m_SignTimeID == time)
	{
		return true;
	}

	return false;
}

ACE_INT32 CVbeIdcService::InsertForceHistory(const CDscString& owner, ACE_UINT32 nActionId, ACE_UINT32 nType, ACE_INT32 dForceNum, CDscString& strTxId)
{
	CTableWrapper< CForceHistory > tForceHistory("T_FORCE_HISTORY");
	CForceHistory& tHistory = *tForceHistory;
	CDBDateTime& createTime = *tHistory.m_AwardTime;

	createTime = CDBDateTime::GetDBDateTime(this->GetCurTime());
	tHistory.m_AwardTime.SetChanged(true);

	tHistory.m_strOwner.SetValue(owner);
	tHistory.m_ActionId.SetValue(nActionId);
	tHistory.m_nType.SetValue(nType);
	tHistory.m_dForceNum.SetValue(dForceNum);
	tHistory.m_strTxId.SetValue(strTxId);

	if (::PerInsert(tForceHistory, m_database, m_dbConnection))
	{
		DSC_RUN_LOG_ERROR("insert from t_force_history failed");
		return -1;
	}
	return 0;
}

ACE_INT32 CVbeIdcService::InsertEarlyBirdHistory(const CDscString& owner, ACE_UINT32 nActionId, ACE_UINT32 nStatus, double dDecNum, double dIncNum, CDscString& strTxId)
{
	CTableWrapper< CEarlyBirdHistory > tEarlyBirdHistory("T_EARLY_BIRD_HISTORY");
	CEarlyBirdHistory& tHistory = *tEarlyBirdHistory;

	CDBDateTime& createTime = *tHistory.m_CreateTime;
	createTime = CDBDateTime::GetDBDateTime(this->GetCurTime());

	tHistory.m_CreateTime.SetChanged(true);
	tHistory.m_strOwner.SetValue(owner);
	tHistory.m_ActionId.SetValue(nActionId);
	tHistory.m_nStatus.SetValue(nStatus);
	tHistory.m_dDecBonusNum.SetValue(dDecNum);
	tHistory.m_dIncBonusNum.SetValue(dIncNum);
	tHistory.m_strTxId.SetValue(strTxId);
	tHistory.m_SignTimeID = CDBDate::Today(this->GetCurTime() + 60 * 60 * 24); //明天的


	if (::PerInsert(tEarlyBirdHistory, m_database, m_dbConnection))
	{
		DSC_RUN_LOG_ERROR("insert from t_force_history failed");
		return -1;
	}

	return 0;
}

ACE_INT32 CVbeIdcService::InsertIDMHistory(ACE_UINT32 nCoinType, const CDscString& owner, ACE_UINT32 nActionId, ACE_UINT32 nType, double nIDMNum, CDscString& strTxId, const CDscString& Addr)
{
	CTableWrapper< CIDMHistory > tIDMHistory("T_IDM_HISTORY");
	CIDMHistory& tHistory = *tIDMHistory;
	CDBDateTime& createTime = *tHistory.m_CreateTime;

	createTime = CDBDateTime::GetDBDateTime(this->GetCurTime());
	tHistory.m_CreateTime.SetChanged(true);

	tHistory.m_nCoinType.SetValue(nCoinType);
	tHistory.m_strOwner.SetValue(owner);
	tHistory.m_ActionId.SetValue(nActionId);
	tHistory.m_nType.SetValue(nType);
	tHistory.m_nIDMNum.SetValue(nIDMNum);
	tHistory.m_strTxId.SetValue(strTxId);
	tHistory.m_strAddr.SetValue(Addr);

	if (::PerInsert(tIDMHistory, m_database, m_dbConnection))
	{
		DSC_RUN_LOG_ERROR("insert from t_idm_history failed");
		return -1;
	}
	return 0;
}

ACE_INT32 CVbeIdcService::UpdateLastBonus(const DSC::CDscShortBlob& cryptKey, double idmNum)
{
	CTableWrapper< CLastBonus > lastBonus("T_LAST_BONUS");
	CDscString userKey(cryptKey.GetBuffer(), cryptKey.GetSize());
	CLastBonusCriterion criterionQuery(userKey);

	if (::PerSelect(lastBonus, m_database, m_dbConnection, &criterionQuery))
	{
		DSC_RUN_LOG_ERROR("select from T_LAST_BONUS failed, or no result");

		return -1;
	}

	CDBDate toDate(CDBDate::Today(this->GetCurTime()));

	if (*lastBonus->m_lastReceiveDate == toDate)
	{
		//比较奖励数量
		if (*lastBonus->m_bonus_num >= idmNum)
		{
			CLastBonusUpdateCriterion lastBonusCriterion(userKey);
			lastBonus->m_bonus_num.SetValue(*lastBonus->m_bonus_num - idmNum);
			//lastBonus->m_lastReceiveDate.SetValue(CDBDate::Today(this->GetCurTime()));
			if (::PerUpdate(lastBonus, m_database, m_dbConnection, lastBonusCriterion) <= 0)
			{
				DSC_RUN_LOG_ERROR("update T_LAST_BONUS failed");
				return -1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			DSC_RUN_LOG_ERROR("error:m_bonus_num less idmNum");
			return -1;
		}
	}
	else
	{
		DSC_RUN_LOG_ERROR("error date not today");
		return -1;
	}
}
double CVbeIdcService::BonusNums(const DSC::CDscShortBlob& cryptKey, double forceNum)
{
	CTableWrapper< CLastBonus>  lastBonus("T_LAST_BONUS");

	CDscString userKey(cryptKey.GetBuffer(), cryptKey.GetSize());
	CLastBonusCriterion criterionQuery(userKey);

	if (::PerSelect(lastBonus, m_database, m_dbConnection, &criterionQuery))
	{
		DSC_RUN_LOG_ERROR("select from T_LAST_BONUS failed, or no result");

		return 0.0; // false;
	}

	CDBDate toDate(CDBDate::Today(this->GetCurTime()));

	if (*lastBonus->m_lastReceiveDate == toDate)
	{
		//今天已经收取过奖励 返回剩下奖励数量
		return *lastBonus->m_bonus_num;
	}
	//今天没有收取过奖励 根据原力值返回总数量 并设置更新时间
	double bonusNum = 0.0007 * forceNum + 0.014;

	CLastBonusUpdateCriterion lastBonusCriterion(userKey);
	lastBonus->m_bonus_num.SetValue(bonusNum);
	lastBonus->m_lastReceiveDate.SetValue(toDate);
	if (::PerUpdate(lastBonus, m_database, m_dbConnection, lastBonusCriterion) <= 0)
	{
		DSC_RUN_LOG_ERROR("update T_LAST_BONUS failed");
		return 0.0;
	}
	else
	{
		return bonusNum;
	}
}

ACE_INT32 CVbeIdcService::ifEarlyBirdSign(const CDscString& owner)
{
	int time5To8 = 0;
	int enterFight = 0;
	int sign = 0;

	struct tm atm;
	DSC::CDscDate curDate;
	CDscGetTime::localtime_r(atm, ACE_OS::time(0));
	//客户端小于5点 5~8点 大于8点有不同需求
	if (atm.tm_hour < m_SignLowTime)
	{
		DSC_RUN_LOG_ERROR("time less %d", m_SignLowTime);
		time5To8 = 1;
	}
	else if (atm.tm_hour >= m_SignHighTime)
	{
		//DSC_RUN_LOG_ERROR("time greater %d", m_SignHighTime);
		if (atm.tm_hour < m_RewardTime)
		{
			//DSC_RUN_LOG_ERROR("time greater %d, less %d", m_SignHighTime, m_RewardTime);
			time5To8 = 2;
		}
		else if (atm.tm_hour == m_RewardTime)
		{
			if (atm.tm_min < 30)
			{
				time5To8 = 2;
			}
			else
			{
				time5To8 = 3;
			}
		}
		else
		{
			time5To8 = 3;
		}
	}

	CTableWrapper< CEarlyBirdHistory>  tEarlyBirdHistory("T_EARLY_BIRD_HISTORY");

	CDBDate time = CDBDate::Today(this->GetCurTime());

	CIfEarlyBirdSignCriterion criterionQuery(owner, time);

	if (::PerSelect(tEarlyBirdHistory, m_database, m_dbConnection, &criterionQuery))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed, or no result");
		enterFight = 1;
	}
	else
	{
		if (*tEarlyBirdHistory->m_nStatus == 1)
		{
			sign = 1;
		}
	}

	return sign * 100 + enterFight * 10 + time5To8;
}
//////////////////////////////////////////////////////////////////////////////////
// idc服务相关逻辑 和vbh无关
ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CShareInfoReqComm req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	CShareInfoSession* pShareInfoSession = DSC_THREAD_TYPE_NEW(CShareInfoSession) CShareInfoSession(*this, pMcpHandler->GetHandleID());

	pShareInfoSession->m_strShareInfo.assign(req.m_shareInfo.GetBuffer(), req.m_shareInfo.GetSize());
	pShareInfoSession->m_strSharerSign.assign(req.m_sharerSign.GetBuffer(), req.m_sharerSign.GetSize());
	// 记录分享人的id  用来插入数据库
	pShareInfoSession->m_nSharerKey.assign(userKey.GetBuffer(), userKey.GetSize());

	ACE_INT32 retCode = 0;

	if (m_shareinfoSession.Insert(m_nShareInfoSessionId, pShareInfoSession))
	{
		DSC_RUN_LOG_INFO("insert session to map failed, session:%d", m_nShareInfoSessionId);
		DSC_THREAD_TYPE_DEALLOCATE(pShareInfoSession);

		retCode = VBE_IDC::IDC_SHARE_INFO_ADD_SESSION_ERROR_TYPE;
	}
	else
	{
		//rsp.m_nShareInfoUrlId = m_nShareInfoSessionId;   //问题: 这里把sessionid 做为共享信息的id，如果业务量很大，查询次数频繁，是否会超范围 ?
		VBE_IDC::CJsonShareInfoUrlID shareInfoID;
		shareInfoID.shareInfoUrlID = m_nShareInfoSessionId;

		SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(shareInfoID);
		if (pJsonKeyWriter)
		{
			retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
			++m_nShareInfoSessionId;
			this->SetDscTimer(pShareInfoSession, m_nShareTime);
			json_writer_free(pJsonKeyWriter);
		}
		else
		{
			DSC_RUN_LOG_ERROR("share user info encode Error ");
			retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
		}
		
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CDownLoadShareInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	CShareInfoSession* pShareInfoSession = m_shareinfoSession.Find(req.m_shareInfoUrl);

	ACE_INT32 retCode = 0;
	
	VBE_IDC::CJsonDownloadShareInfo downloadShareInfo;

	if (pShareInfoSession)
	{
		CTableWrapper< CShareInfo > shareInfo("T_SHARE_INFO");
		CShareInfo& info = *shareInfo;
		CDBDateTime& userDateTime = *info.m_shareTime;

		userDateTime = CDBDateTime::GetDBDateTime(this->GetCurTime());

		info.m_shareInfo.SetValue(pShareInfoSession->m_strShareInfo.c_str());
		info.m_targetName.SetValue(req.m_viewerSign.GetBuffer());
		info.m_sharerName.SetValue(pShareInfoSession->m_strSharerSign.data());
		info.m_shareStatus.SetValue(1); // 1状态为信息已被获取
		info.m_shareTime.SetChanged(true);
		info.m_userID.SetValue(pShareInfoSession->m_nSharerKey);
		CDscString targetKey(userKey.GetBuffer(), userKey.GetSize());
		info.m_targetID.SetValue(targetKey);
		if (::PerInsert(shareInfo, m_database, m_dbConnection))
		{
			retCode = VBE_IDC::IDC_SHARE_INFO_INSERT_DATABASE_ERRPR_TYPE;
			DSC_RUN_LOG_ERROR("insert from shareInfo failed");
		}
		else
		{
			downloadShareInfo.shareInfo = pShareInfoSession->m_strShareInfo;
			pShareInfoSession->m_nSqlID = m_dbConnection.InsertID();
			retCode = 0;
		}
	}
	else
	{
		retCode = VBE_IDC::IDC_SHARE_INFO_CANNOT_FIND_ERRPR_TYPE;
	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(downloadShareInfo);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CDeleteShareInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	CShareInfoSession* pShareInfoSession = m_shareinfoSession.Find(req.m_shareInfoUrl);

	ACE_INT32 retCode = 0;
	ACE_UINT64 tmpSqlID = 0;

	if (pShareInfoSession)
	{
		tmpSqlID = pShareInfoSession->m_nSqlID;
		retCode = 0;
		this->CancelDscTimer(pShareInfoSession);
		m_shareinfoSession.Erase(pShareInfoSession);
		DSC_THREAD_TYPE_DEALLOCATE(pShareInfoSession);
	}
	else
	{
		retCode = VBE_IDC::IDC_SHARE_INFO_CANNOT_FIND_ERRPR_TYPE;
	}

	if (tmpSqlID)
	{
		CTableWrapper< CShareInfo > tblShareInfo("T_SHARE_INFO");
		CShareInfoUpdateCriterion shareInfoCriterion(tmpSqlID);
		tblShareInfo->m_shareStatus.SetValue(0);
		if (::PerUpdate(tblShareInfo, m_database, m_dbConnection, shareInfoCriterion) < 0)
		{
			retCode = VBE_IDC::IDC_DELETE_INFO_UPDATE_DATABASE_ERRPR_TYPE;
			DSC_RUN_LOG_ERROR("update from delete info failed");
		}
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CQueryShareHistoryInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	ACE_INT32 retCode = 0;

	CTableWrapper< CCollectWrapper<CQueryShareHistoryInfo> > lstQueryShareHistoryInfo("T_SHARE_INFO");

	CSelectCriterion* criterionQueryShareHistoryInfo;
	CDscString userID(userKey.GetBuffer(), userKey.GetSize());
	if (req.m_nQueryType == 0)
	{
		//查询分享历史
		criterionQueryShareHistoryInfo = DSC_THREAD_TYPE_NEW(CQueryShareHistoryInfoCriterion) CQueryShareHistoryInfoCriterion(req.m_nStartIndex,
			req.m_nEndIndex, userID);
	}
	else
	{
		criterionQueryShareHistoryInfo = DSC_THREAD_TYPE_NEW(CQueryLookHistoryInfoCriterion) CQueryLookHistoryInfoCriterion(req.m_nStartIndex,
			req.m_nEndIndex, userID);
	}

	VBE_IDC::CJsonQueryShareInfoHistory queryShareInfoHistory;
	if (::PerSelect(lstQueryShareHistoryInfo, m_database, m_dbConnection, criterionQueryShareHistoryInfo))
	{
		DSC_RUN_LOG_ERROR("select from T_SHARE_INFO failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = 0;

		VBE_IDC::CJsonShareDataList listJson;
		VBE_IDC::CJsonShareData jsonData;

		for (CCollectWrapper<CQueryShareHistoryInfo>::iterator it = lstQueryShareHistoryInfo->begin(); it != lstQueryShareHistoryInfo->end(); ++it)
		{
			CDBDateTime& shareTime = *it->m_shareTime;
			CDscString& shareInfo = *it->m_shareInfo;

			jsonData.shareData = shareInfo;
			jsonData.shareTime = shareTime;
			if (req.m_nQueryType == 0)
			{
				jsonData.shareTarget = *it->m_targetName;
				jsonData.targetDid = *it->m_targetID;
			}
			else
			{
				jsonData.shareTarget = *it->m_sharerName;
				jsonData.targetDid = *it->m_userID;
			}

			listJson.data.push_back(jsonData);
		}

		SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(listJson);
		if (pJsonKeyWriter)
		{
			queryShareInfoHistory.shareInfoHistory.append(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
			json_writer_free(pJsonKeyWriter);
		}
		else
		{
			DSC_RUN_LOG_ERROR("ecode json from CQueryShareHistoryInfoReq failed");
		}
	}

	DSC_THREAD_TYPE_DEALLOCATE(criterionQueryShareHistoryInfo);

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(queryShareInfoHistory);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CGetAssetHistoryInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	ACE_INT32 retCode = 0;
	VBE_IDC::CJsonQueryAssetHistory queryAssetHistory;
	queryAssetHistory.startIndex = req.m_nStartIndex;
	queryAssetHistory.endIndex = req.m_nEndIndex;

	CDscString userID(userKey.GetBuffer(), userKey.GetSize());

	CTableWrapper< CCollectWrapper<CIDMHistory> > lstIDMHistory("T_IDM_HISTORY");
	CIDMHistoryCriterion idmHistoryCriterion(req.m_nStartIndex, req.m_nEndIndex
		, userID, req.m_nCoinType, req.m_nRecordType);

	CDscString tmpDataJson;

	if (::PerSelect(lstIDMHistory, m_database, m_dbConnection, &idmHistoryCriterion))
	{
		DSC_RUN_LOG_ERROR("select from T_IDM_HISTORY failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = 0;

		VBE_IDC::CJsonIDMHistoryDataList listJson;
		VBE_IDC::CJsonIDMHistoryData jsonData;

		for (CCollectWrapper<CIDMHistory>::iterator it = lstIDMHistory->begin(); it != lstIDMHistory->end(); ++it)
		{
			jsonData.createTime = *it->m_CreateTime;
			jsonData.coinType = *it->m_nCoinType;
			jsonData.owner = *it->m_strOwner;
			jsonData.type = *it->m_nType;
			jsonData.IDMNum = *it->m_nIDMNum;
			jsonData.TxId = *it->m_strTxId;
			jsonData.Addr = *it->m_strAddr;

			listJson.data.push_back(jsonData);
		}

		SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(listJson);
		if (pJsonKeyWriter)
		{
			tmpDataJson.append(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
			queryAssetHistory.assetInfoHistory.assign(tmpDataJson.data(), tmpDataJson.size());
			json_writer_free(pJsonKeyWriter);
		}
		else
		{
			DSC_RUN_LOG_ERROR("ecode json from CGetAssetHistoryInfoReq failed");
		}
	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(queryAssetHistory);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CGetForceHistoryInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	VBE_IDC::CJsonQueryForceHistory queryForceHistory;
	queryForceHistory.endIndex = req.m_nEndIndex;
	queryForceHistory.startIndex = req.m_nStartIndex;
	ACE_INT32 retCode = 0;
	CDscString userID(userKey.GetBuffer(), userKey.GetSize());
	CTableWrapper< CCollectWrapper<CForceHistory> > lstForceHistory("T_FORCE_HISTORY");
	CForceHistoryCriterion forceHistoryCriterion(req.m_nStartIndex
		, req.m_nEndIndex
		, userID);

	CDscString tmpDataJson;
	if (::PerSelect(lstForceHistory, m_database, m_dbConnection, &forceHistoryCriterion))
	{
		DSC_RUN_LOG_ERROR("select from T_FORCE_HISTORY failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = 0;

		VBE_IDC::CJsonForceHistoryDataList listJson;
		VBE_IDC::CJsonForceHistoryData jsonData;

		for (CCollectWrapper<CForceHistory>::iterator it = lstForceHistory->begin(); it != lstForceHistory->end(); ++it)
		{
			jsonData.awardTime = *it->m_AwardTime;
			jsonData.awardType = *it->m_nType;
			jsonData.num = *it->m_dForceNum;
			listJson.data.push_back(jsonData);
		}

		SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(listJson);
		if (pJsonKeyWriter)
		{
			tmpDataJson.append(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
			queryForceHistory.forceInfoHistory.assign(tmpDataJson.data(), tmpDataJson.size());
			json_writer_free(pJsonKeyWriter);
		}
		else
		{
			DSC_RUN_LOG_ERROR("ecode json from CGetForceHistoryInfoReq failed");
		}
	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(queryForceHistory);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CEarlyBirdSignReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	ACE_INT32 retCode = 0;
	CDscString userID(userKey.GetBuffer(), userKey.GetSize());
	if (ifEarlyBirdSign(userID) != 0)
	{
		retCode = VBE_IDC::IDC_CANNOT_EARLY_BIRD_SIGN_TYPE;
		return retCode;
	}

	CTableWrapper< CEarlyBirdHistory > tblEarlyBird("T_EARLY_BIRD_HISTORY");

	CDBDate timeId = CDBDate::Today(this->GetCurTime());
	timeId.SetDay(timeId.GetDay());

	CEarlyBirdUpdateCriterion ebUpdateCriterion(userID, timeId);

	tblEarlyBird->m_nStatus.SetValue(1);
	*tblEarlyBird->m_SignTime = CDBDateTime::GetDBDateTime(this->GetCurTime());
	tblEarlyBird->m_SignTime.SetChanged(true);

	retCode = ::PerUpdate(tblEarlyBird, m_database, m_dbConnection, ebUpdateCriterion);
	if (retCode < 0)
	{
		//sql语句执行失败
		retCode = VBE_IDC::IDC_DB_SQL_EXECUTE_FAILED_ERROR_TYPE;
		DSC_RUN_LOG_INFO("idc_early_bird_sign_req failed to update db");
	}
	else if (retCode == 0)
	{
		//没有改变任何行
		retCode = VBE_IDC::IDC_DB_SQL_EXECUTE_NO_CHANGE_ERROR_TYPE;
	}
	else
	{
		//改变了nRetCode行
		retCode = 0;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CGetEarlyBirdHistoryReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	ACE_UINT32 retCode = 0;
	CDscString userID(userKey.GetBuffer(), userKey.GetSize());
	VBE_IDC::CJsonQueryEarlyBirdHistory queryEarlyBirdHistory;
	queryEarlyBirdHistory.status = 2; //未参加挑战

	int nYear;
	int nMonth;
	int nDay;
	// "YYYY-MM-DD"
	if (DSC::DscAtoi(nYear, req.m_queryTime.GetBuffer(), 4) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nMonth, req.m_queryTime.GetBuffer() + 5, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nDay, req.m_queryTime.GetBuffer() + 8, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}

	CDBDate createTimeID;
	createTimeID.SetYear(nYear);
	createTimeID.SetMon(nMonth);
	createTimeID.SetDay(nDay);

	CTableWrapper< CCollectWrapper<CEarlyBirdHistory> > lstEarlyBirdHistory("T_EARLY_BIRD_HISTORY");
	CEarlyBirdHistoryCriterion earlyBirdHistoryCriterion(userID, createTimeID);

	if (::PerSelect(lstEarlyBirdHistory, m_database, m_dbConnection, &earlyBirdHistoryCriterion))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = 0; //IDC::IDC_QUERY_EARLY_BIRD_NO_INFO_ERROR_TYPE;

		for (CCollectWrapper<CEarlyBirdHistory>::iterator it = lstEarlyBirdHistory->begin(); it != lstEarlyBirdHistory->end(); ++it)
		{
			char timeBuf[64] = { 0 };
			sprintf(timeBuf, "%04d-%02d-%02d %02d:%02d:%02d", it->m_SignTime->GetYear(),
				it->m_SignTime->GetMon(), it->m_SignTime->GetDay(),
				it->m_SignTime->GetHour(), it->m_SignTime->GetMinute(),
				it->m_SignTime->GetSecond());

			queryEarlyBirdHistory.createTime.assign(timeBuf, strlen(timeBuf));
			queryEarlyBirdHistory.numBonus = *it->m_dIncBonusNum;
			queryEarlyBirdHistory.status = *it->m_nStatus;
			queryEarlyBirdHistory.Txid.assign(it->m_strTxId->data(), it->m_strTxId->size());
			retCode = 0;
		}
	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(queryEarlyBirdHistory);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CGetEarlyBirdPeopleCountReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	VBE_IDC::CJsonEarlyBirdPeopleCount earlyBirdPeopleCount;
	ACE_UINT32 retCode = 0;

	int nYear;
	int nMonth;
	int nDay;
	// "YYYY-MM-DD"
	if (DSC::DscAtoi(nYear, req.m_queryTime.GetBuffer(), 4) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nMonth, req.m_queryTime.GetBuffer() + 5, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nDay, req.m_queryTime.GetBuffer() + 8, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}

	CDBDate signTimeID;
	signTimeID.SetYear(nYear);
	signTimeID.SetMon(nMonth);
	signTimeID.SetDay(nDay);

	CTableWrapper< CCollectWrapper<CEarlyBirdPeopleCount> > lstEarlyBirdPeopleCount("T_EARLY_BIRD_HISTORY");
	CEarlyBirdPeopleCountCriterion earlyBirdPeopleCountCriterion(signTimeID);

	if (::PerSelect(lstEarlyBirdPeopleCount, m_database, m_dbConnection, &earlyBirdPeopleCountCriterion))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = VBE_IDC::IDC_QUERY_EARLY_BIRD_NO_INFO_ERROR_TYPE;


		for (CCollectWrapper<CEarlyBirdPeopleCount>::iterator it = lstEarlyBirdPeopleCount->begin(); it != lstEarlyBirdPeopleCount->end(); ++it)
		{
			earlyBirdPeopleCount.count = *it->count;
			retCode = 0;
		}

	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(earlyBirdPeopleCount);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CGetEarlyBirdRankListReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	VBE_IDC::CJsonGetEarlyBirdRankList getEarlyBirdRankList;
	
	ACE_UINT32 retCode = 0;

	getEarlyBirdRankList.num = req.m_nNum;

	int nYear;
	int nMonth;
	int nDay;
	// "YYYY-MM-DD"
	if (DSC::DscAtoi(nYear, req.m_queryTime.GetBuffer(), 4) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nMonth, req.m_queryTime.GetBuffer() + 5, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nDay, req.m_queryTime.GetBuffer() + 8, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}

	CDBDate signTimeID;
	signTimeID.SetYear(nYear);
	signTimeID.SetMon(nMonth);
	signTimeID.SetDay(nDay);

	CTableWrapper< CCollectWrapper<CEarlyBirdRankList> > lstEarlyBirdRankList("T_EARLY_BIRD_HISTORY");
	CEarlyBirdRankListCriterion earlyBirdRankListCriterion(signTimeID, 1, req.m_nNum);

	CDscString tmpDataJson;
	if (::PerSelect(lstEarlyBirdRankList, m_database, m_dbConnection, &earlyBirdRankListCriterion))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = 0;   // IDC::IDC_QUERY_EARLY_BIRD_NO_INFO_ERROR_TYPE; 客户端不喜欢用错误码 这里需要客户端自己判断数组大小

		VBE_IDC::CJsonRankDataList listJson;
		VBE_IDC::CJsonRankData jsonData;

		for (CCollectWrapper<CEarlyBirdRankList>::iterator it = lstEarlyBirdRankList->begin(); it != lstEarlyBirdRankList->end(); ++it)
		{
			jsonData.user = *it->m_owner;
			jsonData.num = *it->m_dBonus;

			listJson.data.push_back(jsonData);

			//rsp.m_nReturnCode = 0;
		}

		SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(listJson);
		if (pJsonKeyWriter)
		{
			tmpDataJson.append(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
			getEarlyBirdRankList.rankList.assign(tmpDataJson.data(), tmpDataJson.size());

			json_writer_free(pJsonKeyWriter);
		}
		else
		{
			DSC_RUN_LOG_ERROR("ecode json from CGetForceHistoryInfoReq failed");
		}
	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(getEarlyBirdRankList);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CIfEarlyBirdSignReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{

	VBE_IDC::CJsonIfEarlyBirdSign ifSign;
	ACE_UINT32 retCode = 0;

	CDscString userID(userKey.GetBuffer(), userKey.GetSize());
	ifSign.sign = ifEarlyBirdSign(userID);

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(ifSign);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

ACE_INT32 CVbeIdcService::OnTypeProc(VBE_IDC::CGetSignHistoryReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo)
{
	VBE_IDC::CJsonSignHistoryRsp signHistory;

	ACE_UINT32 retCode = 0;

	int nYear;
	int nMonth;

	// "YYYY-MM"
	if (DSC::DscAtoi(nYear, req.m_queryMonth.GetBuffer(), 4) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}
	if (DSC::DscAtoi(nMonth, req.m_queryMonth.GetBuffer() + 5, 2) != 0)
	{
		retCode = VBE_IDC::IDC_QUERY_TIME_FORMAT_ERRPR_TYPE;
		return retCode;
	}

	CDBDate signTimeID_0;
	signTimeID_0.SetYear(nYear);
	signTimeID_0.SetMon(nMonth);
	signTimeID_0.SetDay(0);

	CDBDate signTimeID_31(signTimeID_0);
	signTimeID_31.SetDay(31);

	CDscString userID(userKey.GetBuffer(), userKey.GetSize());
	CTableWrapper< CCollectWrapper<CGetSignHistory> > lstSignHistory("T_EARLY_BIRD_HISTORY");
	CGetSignHistoryCriterion signHistoryCriterion(signTimeID_0, signTimeID_31, userID);

	CDscString tmpDataJson;
	if (::PerSelect(lstSignHistory, m_database, m_dbConnection, &signHistoryCriterion))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed");

		retCode = VBE_IDC::IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE;
	}
	else
	{
		retCode = 0;   // IDC::IDC_QUERY_EARLY_BIRD_NO_INFO_ERROR_TYPE; 客户端不喜欢用错误码 这里需要客户端自己判断数组大小

		VBE_IDC::CJsonSignHistoryList listJson;
		VBE_IDC::CJsonSignHistory jsonData;

		for (CCollectWrapper<CGetSignHistory>::iterator it = lstSignHistory->begin(); it != lstSignHistory->end(); ++it)
		{
			jsonData.status = *it->m_nStatus;
			jsonData.signTime = *it->m_signTime;

			listJson.data.push_back(jsonData);

			//rsp.m_nReturnCode = 0;
		}

		SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(listJson);
		if (pJsonKeyWriter)
		{
			tmpDataJson.append(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
			signHistory.data.assign(tmpDataJson.data(), tmpDataJson.size());
			json_writer_free(pJsonKeyWriter);
		}
		else
		{
			DSC_RUN_LOG_ERROR("ecode json from CGetForceHistoryInfoReq failed");
		}
	}

	SJsonWriter* pJsonKeyWriter = JSON_CODE::Encode(signHistory);
	if (pJsonKeyWriter)
	{
		retInfo.assign(pJsonKeyWriter->m_pBuffer, pJsonKeyWriter->m_nCurrentPos);
		json_writer_free(pJsonKeyWriter);
	}
	else
	{
		retCode = VBE_IDC::IDC_JSON_ENCODE_ERROR_TYPE;
	}

	return retCode;
}

//1. 从表里查询出签到过的(status=1)、昨天(SIGN_TIME_ID签到日期)、没有获取过奖励(OBTAIN_STATUS=0)的所有记录的owner，账号ID
//2. 把查出来的账号ID进行转账
//3. 修改对应的Obtain_Status状态为1
void CVbeIdcService::StartDistributeReward(CDBDate& toDay)
{
	//ACE_UINT32 nCount = GetEarlyBirdCount(toDay);   //获奖的人数
	//TODO
	m_nRewardUserNum = GetEarlyBirdCount(toDay);
	if (m_nRewardUserNum <= 0)
	{
		DSC_RUN_LOG_ERROR("T_EARLY_BIRD_HISTORY count is 0");
		return;
	}

	//double* bufRewardPool = new double[nCount];
	if (m_pRewardNumsArray)
	{
		delete[] m_pRewardNumsArray;
		m_pRewardNumsArray = NULL;
	}
	double* m_pRewardNumsArray = new double[m_nRewardUserNum];

	if (m_pRewardUserArray)
	{
		delete[] m_pRewardUserArray;
		m_pRewardUserArray = NULL;
	}
	CDscString* m_pRewardUserArray = new CDscString[m_nRewardUserNum];

	if (GenerateRewardPool(m_pRewardNumsArray, m_nRewardUserNum))
	{
		DSC_RUN_LOG_ERROR("g_rewardNumsArray is null");
		return;
	}

	CTableWrapper< CCollectWrapper<CEarlyBirdHistoryDisReward> > lstDisRewardInfo("T_EARLY_BIRD_HISTORY");

	CEBHDisRewardCriterion criterion(toDay);

	if (::PerSelect(lstDisRewardInfo, m_database, m_dbConnection, &criterion))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed");
	}
	else
	{
		int nIndex = 0;
		for (CCollectWrapper<CEarlyBirdHistoryDisReward>::iterator it = lstDisRewardInfo->begin(); it != lstDisRewardInfo->end(); ++it)
		{
			//m_pRewardUserArray[nIndex++] = *it->m_owner;
			//ACE_UINT32 nObtainStatus = *it->m_nObtainStatus;

			//2.为该账号发放奖励
				//分发奖励
			DSC::CDscShortBlob userKey;
			userKey.Set(m_strIDMUserID.data(), m_strIDMUserID.size());


			VBE_IDC::CTransInfoIdcVbh transInfoIdcVbh;
			transInfoIdcVbh.m_dIDMNum = *it->m_dIncBonusNum;
			transInfoIdcVbh.m_strUserKey = m_strIDMUserID;
			transInfoIdcVbh.m_strTargetKey = *it->m_owner;

			char* pProposal = NULL;
			size_t nProposalLen;
			DSC::Encode(transInfoIdcVbh, pProposal, nProposalLen);
		
			DSC::CDscShortBlob transInfo;
			transInfo.Set(pProposal, nProposalLen);

			ACE_INT32 nRetCode = m_pHas->Propose(0, VBE_IDC::EN_VBE_IDC_CHANNELELID, false,
				VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, userKey, transInfo);
			if (nRetCode)
			{
				DSC_RUN_LOG_ERROR("StartDistributeReward error!!! error code:%d", nRetCode);
			}

			//3.更新数据库表Obtain_Status为1，及奖励数额
			UpdateObtainStatus(*it->m_owner, *it->m_dIncBonusNum);
		}
		////分发奖励
		//DSC::CDscShortBlob userKey;
		//userKey.Set(m_strIDMUserID.data(), m_strIDMUserID.size());


		//VBE_IDC::CTransInfoIdcVbh transInfoIdcVbh;
		//transInfoIdcVbh.m_dIDMNum = m_pRewardNumsArray[0];
		//transInfoIdcVbh.m_strUserKey = m_strIDMUserID;
		//transInfoIdcVbh.m_strTargetKey = m_pRewardUserArray[0];

		//char* pProposal = NULL;
		//size_t nProposalLen;
		//DSC::Encode(transInfoIdcVbh, pProposal, nProposalLen);
		//
		//DSC::CDscShortBlob transInfo;
		//transInfo.Set(pProposal, nProposalLen);

		//ACE_INT32 nRetCode = m_pHas->Propose(0, VBE_IDC::EN_VBE_IDC_CHANNELELID, false,
		//	VBE_IDC::EN_IDC_UPDATE_USER_INFO_TYPE, userKey, transInfo);
		//if (nRetCode)
		//{
		//	DSC_RUN_LOG_ERROR("StartDistributeReward error!!! error code:%d", nRetCode);
		//}
	}
}

void CVbeIdcService::UpdateObtainStatus(const CDscString& owner, double dNumIdm)
{
	CTableWrapper< CEarlyBirdHistoryDisReward> tblDisRewardInfo("T_EARLY_BIRD_HISTORY");
	CDBDate time = CDBDate::Today(this->GetCurTime());

	CEBHistoryDisRewardUpdateCriterion criterion(owner, time);

	tblDisRewardInfo->m_nObtainStatus.SetValue(1);
	tblDisRewardInfo->m_dIncBonusNum.SetValue(dNumIdm);

	if (::PerUpdate(tblDisRewardInfo, m_database, m_dbConnection, criterion) < 0)
	{
		DSC_RUN_LOG_ERROR("update from early_bird_history info failed");
	}
}

ACE_UINT32 CVbeIdcService::GetEarlyBirdCount(CDBDate& toDay)
{
	ACE_UINT32 nRet = 0;

	CTableWrapper< CCollectWrapper<CEarlyBirdPeopleCount> > lstDisRewardInfo("T_EARLY_BIRD_HISTORY");

	CEBHDisRewardCriterion criterion(toDay);

	if (::PerSelect(lstDisRewardInfo, m_database, m_dbConnection, &criterion))
	{
		DSC_RUN_LOG_ERROR("select from T_EARLY_BIRD_HISTORY failed");
	}
	else
	{
		for (CCollectWrapper<CEarlyBirdPeopleCount>::iterator it = lstDisRewardInfo->begin(); it != lstDisRewardInfo->end(); ++it)
		{
			nRet = *it->count;
		}
	}

	return nRet;
}

ACE_UINT8 CVbeIdcService::GenerateRewardPool(double* rewardPool, ACE_UINT32 nSize)
{
	double dTotalReward = 0.014 * nSize;

	if (rewardPool == NULL)
	{
		return -1;
	}
	else
	{
		double beLeft = dTotalReward;

		std::mt19937 rng;
		rng.seed(std::random_device()());

		for (ACE_UINT32 i = 0; i < nSize - 1; i++)
		{
			//std::uniform_real_distribution<double> distribution(0, beLeft);
			std::uniform_real_distribution<double> distribution(0.0001, (beLeft / (nSize - i) - 0.0001) * 2);
			rewardPool[i] = floor(distribution(rng) * 10000.000f + 0.5) / 10000.000f;
			beLeft -= rewardPool[i];
		}

		rewardPool[nSize - 1] = beLeft;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////


void CVbeIdcService::COnlineUser::OnTimer()
{
	// TODO 
}
 
CVbeIdcService::IUserSession::IUserSession(CVbeIdcService& rService)
	:m_rService(rService)
{
}
CVbeIdcService::CLoginSession::CLoginSession(CVbeIdcService& rService)
	: IUserSession(rService)
{
}

CVbeIdcService::CLoginSession::~CLoginSession()
{
	m_nonce.FreeBuffer();
	m_userKey.FreeBuffer();
	m_vbeUserKey.FreeBuffer();
}

void CVbeIdcService::CLoginSession::OnTimer()
{
	m_rService.OnTimeOut(this);
}

void CVbeIdcService::CLoginSession::OnNetError()
{
	m_rService.OnNetworkError(this);
}

CVbeIdcService::CTransferSession::CTransferSession(CVbeIdcService& rService)
	:IUserSession(rService)
{
}

CVbeIdcService::CTransferSession::~CTransferSession()
{
	// TODO
}

void CVbeIdcService::CTransferSession::OnTimer()
{
	m_rService.OnTimeOut(this);
}

void CVbeIdcService::CTransferSession::OnNetError()
{
	m_rService.OnNetworkError(this);
}


CVbeIdcService::CVbeServiceHandler::CVbeServiceHandler(CMcpServerService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID)
	:CMcpServerHandler(rService, handle, nHandleID)
{
}

//CVbeIdcService::CGetUserInfoSession::CGetUserInfoSession(CVbeIdcService& rService)
//	: IUserSession(rService)
//{
//}
//
//CVbeIdcService::CGetUserInfoSession::~CGetUserInfoSession()
//{
//	m_nonce.FreeBuffer();
//}
//
//void CVbeIdcService::CGetUserInfoSession::OnTimer(void)
//{
//	m_rService.OnTimeOut(this);
//}
//
//void CVbeIdcService::CGetUserInfoSession::OnNetError(void)
//{
//	m_rService.OnNetworkError(this);
//}

void CVbeIdcService::OnTimeOut(CVbeIdcService::CLoginSession* pRegSession)
{
	ResponseLogin(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pRegSession->m_nCltSessionID, pRegSession->m_pServiceHandler->GetHandleID());
	EraseSession(m_mapLoginSession, pRegSession->m_nSessionID);
	DSC_THREAD_TYPE_DELETE(pRegSession);
}

void CVbeIdcService::OnNetworkError(CVbeIdcService::CLoginSession* pRegSession)
{
	EraseSession(m_mapLoginSession, pRegSession->m_nSessionID);
	DSC_THREAD_TYPE_DELETE(pRegSession);
}

void CVbeIdcService::OnTimeOut(CCheckUserSession* pCheckUserSession)
{
}

void CVbeIdcService::OnNetworkError(CCheckUserSession* pCheckUserSession)
{
}

void CVbeIdcService::OnTimeOut(CVbeIdcService::CTransferSession* pTransferSession)
{
	ResponseTransfer(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pTransferSession->m_nCltSessionID, pTransferSession->m_pServiceHandler->GetHandleID());
	EraseSession(m_mapTransferSession, pTransferSession->m_nSessionID);
	DSC_THREAD_TYPE_DELETE(pTransferSession);
}

void CVbeIdcService::OnNetworkError(CVbeIdcService::CTransferSession* pTransferSession)
{
	EraseSession(m_mapTransferSession, pTransferSession->m_nSessionID);
	DSC_THREAD_TYPE_DELETE(pTransferSession);
}

//void CVbeIdcService::OnTimeOut(CGetUserInfoSession* pGetUserInfoSession)
//{
//	ResponseGetUserInfo(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pGetUserInfoSession->m_nCltSessionID, pGetUserInfoSession->m_pServiceHandler->GetHandleID());
//	EraseSession(m_mapGetUserInfoSession, pGetUserInfoSession->m_nSessionID);
//	DSC_THREAD_TYPE_DELETE(pGetUserInfoSession);
//}
//
//void CVbeIdcService::OnNetworkError(CGetUserInfoSession* pGetUserInfoSession)
//{
//	EraseSession(m_mapGetUserInfoSession, pGetUserInfoSession->m_nSessionID);
//	DSC_THREAD_TYPE_DELETE(pGetUserInfoSession);
//}


CVbeIdcService::CVbhOperationSession::CVbhOperationSession(CVbeIdcService& rService)
	: IUserSession(rService)
{
}

CVbeIdcService::CVbhOperationSession::~CVbhOperationSession()
{
	m_nonce.FreeBuffer();
	m_userInfo.FreeBuffer();
}

void CVbeIdcService::CVbhOperationSession::OnTimer(void)
{
	m_rService.OnTimeOut(this);
}

void CVbeIdcService::CVbhOperationSession::OnNetError(void)
{
	m_rService.OnNetworkError(this);
}

CVbeIdcService::CShareInfoSession::CShareInfoSession(CVbeIdcService& rIDomService, const ACE_UINT32 nHandleID)
	: m_rService(rIDomService), m_nHandleID(nHandleID)
{

}

void CVbeIdcService::CShareInfoSession::OnTimer(void)
{
	m_rService.OnTimeOut(this);
}

CVbeIdcService::CDistributeRewardTimer::CDistributeRewardTimer(CVbeIdcService& rService)
	: m_rService(rService)
{
}

inline void CVbeIdcService::CDistributeRewardTimer::OnTimer(void)
{
	m_rService.OnTimeOut(this);
}

void CVbeIdcService::OnTimeOut(CVbhOperationSession* pUpdateSession)
{
	ResponseGetUserInfo(VBE_IDC::EN_TIMEOUT_ERROR_TYPE, pUpdateSession->m_nCltSessionID, pUpdateSession->m_pServiceHandler->GetHandleID());
	EraseSession(m_mapVbhOperationSession, pUpdateSession->m_nSessionID);
	DSC_THREAD_TYPE_DELETE(pUpdateSession);
}

void CVbeIdcService::OnTimeOut(CShareInfoSession* pShareInfoSession)
{
	ResponseIdcInterface(VBE_IDC::EN_USER_NOT_LOGIN_ERROR_TYPE, pShareInfoSession->m_nCltSessionID, pShareInfoSession->m_nHandleID);
	m_shareinfoSession.Erase(pShareInfoSession);
	DSC_THREAD_TYPE_DELETE(pShareInfoSession);
}

void CVbeIdcService::OnTimeOut(CDistributeRewardTimer* pTimer)
{
	if (0 > m_RewardTime || 23 < m_RewardTime)
	{
		DSC_RUN_LOG_INFO("distribute reward time is invalid");
		return;
	}

	CDBDateTime curTime = CDBDateTime::GetDBDateTime(this->GetCurTime());
	CDBDate toDay(CDBDate::Today(this->GetCurTime()));

	if ((curTime.GetHour() == m_RewardTime) && (toDay > m_latsDisRewardDate))
	{
		m_latsDisRewardDate = toDay;  //今天如果派发过奖励了，就不再进行了
		DSC_RUN_LOG_INFO("Begin to distribute rewards");
		StartDistributeReward(toDay);
	}
	this->SetDscTimer(m_pTimerDistributeReward, EN_DISTRIBUTE_REWARD_CYCLE);
}

void CVbeIdcService::OnNetworkError(CVbhOperationSession* pUpdateSession)
{
	EraseSession(m_mapVbhOperationSession, pUpdateSession->m_nSessionID);
	DSC_THREAD_TYPE_DELETE(pUpdateSession);
}

CVbeIdcService::CCheckUserSession::CCheckUserSession(CVbeIdcService& rService)
	:IUserSession(rService)
{
}

CVbeIdcService::CCheckUserSession::~CCheckUserSession()
{
}

void CVbeIdcService::CCheckUserSession::OnTimer(void)
{
	m_rService.OnTimeOut(this);
}

void CVbeIdcService::CCheckUserSession::OnNetError(void)
{
	m_rService.OnNetworkError(this);
}


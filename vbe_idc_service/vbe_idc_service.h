#ifndef _VBE_IDC_SERVICE_H_0951345980123479
#define _VBE_IDC_SERVICE_H_0951345980123479

#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_hls_router.h"

#include "dsc/protocol/hts/dsc_hts_service.h"
#include "dsc/protocol/mcp/mcp_asynch_acceptor.h"
#include "dsc/container/bare_hash_map.h"
#include "dsc/protocol/mcp/mcp_server_handler.h"

#define BEGIN_BIND_TYPE_PROC \
	ACE_INT32 TypeProc(const ACE_UINT32 nType, DSC::CDscShortBlob& info, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo) \
	{ \
		switch(nType) \
		{ \

#define BIND_TYPE_PROC(PROPOSAL_TYPE) \
		case PROPOSAL_TYPE::EN_IDM_INTERFACE_ID: \
		{ \
			PROPOSAL_TYPE prop; \
			if (DSC_UNLIKELY(DSC::Decode(prop, info.GetBuffer(), info.GetSize()))) \
			{ \
				DSC_RUN_LOG_FINE("idom server decode info error"); \
				return -1; \
			} \
			else \
			{ \
				return this->OnTypeProc(prop, pMcpHandler, userKey, retInfo); \
			} \
		}

#define END_BIND_TYPE_PROC \
		default: \
		{ \
			DSC_RUN_LOG_FINE("cann't proc type:%d", nType); \
			return -1; \
		} \
		} \
	}

class CVbhAgentService;
class PLUGIN_EXPORT CVbeIdcService : public CDscHtsServerService
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBE_IDC::EN_VBE_SERVICE_TYPE,

		EN_HASH_MAP_BITES = 16,
		EN_SESSION_TIMEOUT_VALUE = 60,
		EN_ONLINE_USER_TIMEOUT_VALUE = 600,
		EN_RETRY_TRANSFER_TIMEOUT_VALUE = 30,
		EN_DISTRIBUTE_REWARD_CYCLE = 60,
	};

public:
	CVbeIdcService(const CDscString& strIpAddr, const ACE_INT32 nPort);
	virtual ACE_INT32 OnInit(void) override;
	virtual ACE_INT32 OnExit(void) override;


public:
	void SetVbhAgentService(CVbhAgentService* pSrv);


protected:
	BEGIN_HTS_MESSAGE_BIND
	BIND_HTS_MESSAGE(VBE_IDC::CLoginCltVbeReq)
	BIND_HTS_MESSAGE(VBE_IDC::CGetUserInfoCltVbeReq)
	BIND_HTS_MESSAGE(VBE_IDC::CUpdateCltVbeReq)
	BIND_HTS_MESSAGE(VBE_IDC::CIdcInterfaceCltVbeReq)
	BIND_HTS_MESSAGE(VBE_IDC::CTransferCltVbeReq)
	END_HTS_MESSAGE_BIND

public:
	ACE_INT32  OnHtsMsg(VBE_IDC::CLoginCltVbeReq& rLoginReq, CMcpHandler* pMcpHandler);
	ACE_INT32  OnHtsMsg(VBE_IDC::CGetUserInfoCltVbeReq& rGetUserInfoReq, CMcpHandler* pMcpHandler);
	ACE_INT32  OnHtsMsg(VBE_IDC::CUpdateCltVbeReq& rUpdateCltVbeReq, CMcpHandler* pMcpHandler);
	ACE_INT32  OnHtsMsg(VBE_IDC::CIdcInterfaceCltVbeReq& rIdcInterfaceCltVbeReq, CMcpHandler* pMcpHandler);
	ACE_INT32  OnHtsMsg(VBE_IDC::CTransferCltVbeReq& rTransferReq, CMcpHandler* pMcpHandler);
	
protected:
	BEGIN_BIND_DSC_MESSAGE
		DSC_BIND_MESSAGE(VBE_IDC::CCheckUsersHlsVbeRsp)
		DSC_BIND_MESSAGE(VBE_IDC::CTransferHlsVbeRsp)
		DSC_BIND_MESSAGE(VBE_IDC::CIDChainAuthNotifyRsp)
		END_BIND_DSC_MESSAGE

public:
	void OnDscMsg(VBE_IDC::CCheckUsersHlsVbeRsp& rCheckUserRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CTransferHlsVbeRsp& rTransferRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CIDChainAuthNotifyRsp& rIDChainAuthNotifyRsp, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);


public: 
	// CALLBACK FROM HAS
	void OnQueryCryptUserKeyResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& cryptUserKey);
	void OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo);
	void OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo);
	void OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& transKey);
	void OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID);

private:

	class COnlineUser : public CDscServiceTimerHandler
	{
	public:
		DSC::CDscShortBlob m_userKey; // vbh user key
		DSC::CDscShortBlob m_vbeUserKey; // vbe user Key
		DSC::CDscShortBlob m_cryptUserKey;
		ACE_UINT32 m_nToken;
		ACE_UINT32 m_nChannelID;

	public:
		virtual void OnTimer(void) override;

	public:
		ACE_UINT32 m_nKey = 0;
		COnlineUser* m_pPrev = NULL;
		COnlineUser* m_pNext = NULL;
	};

	class CVbeServiceHandler;
	class IUserSession : public CDscServiceTimerHandler
	{
	public:
		IUserSession(CVbeIdcService& rService);
		IUserSession(const IUserSession&) = delete;
		IUserSession& operator= (const IUserSession&) = delete;
	public:
		virtual void OnNetError(void) = 0;

	public:
		ACE_UINT32 m_nSessionID;
		ACE_UINT32 m_nCltSessionID;
		CVbeServiceHandler* m_pServiceHandler;

	public:
		ACE_UINT32 m_nIndex = 0;

	protected:
		CVbeIdcService& m_rService;
	};

	class CLoginSession : public IUserSession
	{
	public:
		CLoginSession(CVbeIdcService& rService);
		~CLoginSession();
	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		DSC::CDscShortBlob m_nonce;
		DSC::CDscShortBlob m_userKey;
		DSC::CDscShortBlob m_vbeUserKey;
		ACE_UINT32 m_nChannelID;
	public:
		ACE_UINT32 m_nKey = 0;
		CLoginSession* m_pPrev = NULL;
		CLoginSession* m_pNext = NULL;
	};

	class CTransferSession : public IUserSession
	{
	public:
		CTransferSession(CVbeIdcService& rService);
		~CTransferSession();
	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		struct CReceiverList
		{
			DSC::CDscShortList<DSC::CDscShortBlob> m_lstReceiver;
			bool m_bChecked;
			bool m_bSentCheck;
		};
	public:
		COnlineUser* m_pOnlineUser;
		DSC::CDscShortBlob m_nonce;
		DSC::CDscShortBlob m_propose;
		DSC::CDscShortBlob m_transKey;
		ACE_UINT32 m_nTotalCoin;
		dsc_unordered_map_type(ACE_UINT32, CReceiverList*) m_mapReceiverList; // channel id -> receiver list

	public:
		ACE_UINT32 m_nKey = 0;
		CTransferSession* m_pPrev = NULL;
		CTransferSession* m_pNext = NULL;
	};

	class CCheckUserSession : public IUserSession
	{
	public:
		CCheckUserSession(CVbeIdcService& rService);
		~CCheckUserSession();
	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		CTransferSession* m_pTransferSession;
		ACE_UINT32 m_nChannelID;

	public:
		ACE_UINT32 m_nKey = 0;
		CCheckUserSession* m_pPrev = NULL;
		CCheckUserSession* m_pNext = NULL;
	};

	/*class CGetUserInfoSession : public IUserSession
	{
	public:
		CGetUserInfoSession(CVbeIdcService& rService);
		~CGetUserInfoSession();
	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		DSC::CDscShortBlob m_nonce;
		COnlineUser* m_pOnlineUser;
		ACE_UINT32 m_nQueryType;

	public:
		ACE_UINT32 m_nKey = 0;
		CGetUserInfoSession* m_pPrev = NULL;
		CGetUserInfoSession* m_pNext = NULL;
	};*/

	class CVbhOperationSession : public IUserSession
	{
	public:
		CVbhOperationSession(CVbeIdcService& rService);
		~CVbhOperationSession();
	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		DSC::CDscShortBlob m_nonce;
		COnlineUser* m_pOnlineUser;
		ACE_UINT32 m_nOperationType;
		DSC::CDscShortBlob m_userInfo;
		DSC::CDscShortBlob m_propose;
		DSC::CDscShortBlob m_transKey;

	public:
		ACE_UINT32 m_nKey = 0;
		CVbhOperationSession* m_pPrev = NULL;
		CVbhOperationSession* m_pNext = NULL;
	};

	class CShareInfoSession : public CDscServiceTimerHandler
	{
	public:
		CShareInfoSession(CVbeIdcService& rService, const ACE_UINT32 nHandleID);

	public:
		virtual void OnTimer(void) override;

	public:
		CDscString m_strShareInfo;
		CDscString m_strSharerSign;
		const ACE_UINT32 m_nHandleID;
		ACE_UINT64 m_nSqlID = 0;
		CDscString m_nSharerKey;

		ACE_UINT32 m_nCltSessionID;

	public:
		ACE_UINT32 m_nKey = 0;
		CShareInfoSession* m_pPrev = NULL;
		CShareInfoSession* m_pNext = NULL;

	private:
		CVbeIdcService& m_rService;
	};

	class CDistributeRewardTimer : public CDscServiceTimerHandler
	{
	public:
		CDistributeRewardTimer(CVbeIdcService& rService);

	public:
		virtual void OnTimer(void);

	private:
		CVbeIdcService& m_rService;
	};

private:
	class CVbeServiceHandler : public CMcpServerHandler
	{
	public:
		CVbeServiceHandler(CMcpServerService& rService, ACE_HANDLE handle, const ACE_INT32 nHandleID);

	public:
		CDscTypeArray<IUserSession> m_arrUserSession;
	};

private:
	void OnTimeOut(CLoginSession* pRegSession);
	void OnNetworkError(CLoginSession* pRegSession);

	void OnTimeOut(CCheckUserSession* pCheckUserSession);
	void OnNetworkError(CCheckUserSession* pCheckUserSession);

	void OnTimeOut(CTransferSession* pTransferSession);
	void OnNetworkError(CTransferSession* pTransferSession);

	//void OnTimeOut(CGetUserInfoSession* pGetUserInfoSession);
	//void OnNetworkError(CGetUserInfoSession* pGetUserInfoSession);

	void OnTimeOut(CVbhOperationSession* pUpdateSession);
	void OnNetworkError(CVbhOperationSession* pUpdateSession);

	void OnTimeOut(CShareInfoSession* pShareInfoSession);

	void OnTimeOut(CDistributeRewardTimer* pTimer);

	void ResponseLogin(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID);
	void ResponseLogin(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob &envelopeKey,  DSC::CDscShortBlob &nonce,  ACE_UINT32 nToken);
	void ResponseGetUserInfo(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID);
	void ResponseGetUserInfo(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& userinfo);
	void ResponseUpdate(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID);
	void ResponseUpdate(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& updateKey);
	void ResponseIdcInterface(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID);
	void ResponseIdcInterface(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscBlob& updateKey);
	void ResponseTransfer(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID);
	void ResponseTransfer(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_INT32 nHandleID, const DSC::CDscShortBlob& enveloperKey, DSC::CDscShortBlob& nonce, DSC::CDscShortBlob& transKey);

	template<typename MAP_TYPE, typename TYPE>
	void InsertSession(MAP_TYPE& mapSession, TYPE* session, CMcpHandler* mcpHandle);
	template<typename MAP_TYPE>
	IUserSession* EraseSession(MAP_TYPE& mapSession, ACE_UINT32 sessionID);
	template<typename RSP_TYPE, typename WRAPPER_TYPE>
	ACE_INT32 EncryptSendResponse(RSP_TYPE& req, WRAPPER_TYPE& wrapper, const DSC::CDscShortBlob& envelopeKey, ACE_UINT32 nHandleID);

	virtual CMcpServerHandler* AllocMcpHandler(ACE_HANDLE handle) override;

public:
	void onQueryUserInfo(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo);
	void Authentication(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& cuserInfo);
	void onQueryAsset(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo);
	void onReceiveBonus(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo);
	void onAddForce(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo);
	void onAdd2EarlyBird(ACE_INT32 nReturnCode, CVbhOperationSession* pVbhOperationSession, const DSC::CDscShortBlob& oriUserInfo);

	//修改用户信息应答相关操作
	void onUpdateRspBonus(CVbhOperationSession* pVbhOperationSession);
	void onUpdateRspAddForce(CVbhOperationSession* pVbhOperationSession);
	void onUpdateRspEarlyBird(CVbhOperationSession* pVbhOperationSession);

public:
	double BonusNums(const DSC::CDscShortBlob& cryptKey, double forceNum);
	ACE_INT32 UpdateLastBonus(const DSC::CDscShortBlob& cryptKey, double idmNum);
	ACE_INT32 InsertIDMHistory(ACE_UINT32 nCoinType, const CDscString& owner, ACE_UINT32 nActionId, ACE_UINT32 nType, double nIDMNum, CDscString& strTxId, const CDscString& Addr);
	ACE_INT32 InsertForceHistory(const CDscString& owner, ACE_UINT32 nActionId, ACE_UINT32 nType, ACE_INT32 dForceNum, CDscString& strTxId);
	bool IfExistsEarlyBirdHistory(const CDscString& owner);
	ACE_INT32 InsertEarlyBirdHistory(const CDscString& owner, ACE_UINT32 nActionId, ACE_UINT32 nStatus, double dDecNum, double dIncNum, CDscString& strTxId);
	ACE_INT32 ifEarlyBirdSign(const CDscString& owner);
public:
	BEGIN_BIND_TYPE_PROC
	BIND_TYPE_PROC(VBE_IDC::CShareInfoReqComm)
	BIND_TYPE_PROC(VBE_IDC::CDownLoadShareInfoReq)
	BIND_TYPE_PROC(VBE_IDC::CDeleteShareInfoReq)
	BIND_TYPE_PROC(VBE_IDC::CQueryShareHistoryInfoReq)
	BIND_TYPE_PROC(VBE_IDC::CGetAssetHistoryInfoReq)
	BIND_TYPE_PROC(VBE_IDC::CGetForceHistoryInfoReq)
	BIND_TYPE_PROC(VBE_IDC::CEarlyBirdSignReq)
	BIND_TYPE_PROC(VBE_IDC::CGetEarlyBirdHistoryReq)
	BIND_TYPE_PROC(VBE_IDC::CGetEarlyBirdPeopleCountReq)
	BIND_TYPE_PROC(VBE_IDC::CGetEarlyBirdRankListReq)
	BIND_TYPE_PROC(VBE_IDC::CIfEarlyBirdSignReq)
	BIND_TYPE_PROC(VBE_IDC::CGetSignHistoryReq)
	END_BIND_TYPE_PROC

public:
	ACE_INT32 OnTypeProc(VBE_IDC::CShareInfoReqComm req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CDownLoadShareInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CDeleteShareInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CQueryShareHistoryInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CGetAssetHistoryInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CGetForceHistoryInfoReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CEarlyBirdSignReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CGetEarlyBirdHistoryReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CGetEarlyBirdPeopleCountReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CGetEarlyBirdRankListReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CIfEarlyBirdSignReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);
	ACE_INT32 OnTypeProc(VBE_IDC::CGetSignHistoryReq req, CMcpHandler* pMcpHandler, const DSC::CDscShortBlob& userKey, CDscString& retInfo);

private:
	ACE_UINT32 NewToken();
	ACE_INT32 DoTransferPropose(CTransferSession* pTransferSession);
	void ReleaseTransferSession(CTransferSession* pTransferSession);

	void ReleaseUpdateSession(CVbhOperationSession* rUpdateSession);

private:
	using online_user_session_map_type = CBareHashMap<ACE_UINT32, COnlineUser, EN_HASH_MAP_BITES>;

	using login_session_map_type = CBareHashMap<ACE_UINT32, CLoginSession, EN_HASH_MAP_BITES>;
	using transfer_session_map_type = CBareHashMap<ACE_UINT32, CTransferSession, EN_HASH_MAP_BITES>;
	using check_user_session_map_type = CBareHashMap<ACE_UINT32, CCheckUserSession, EN_HASH_MAP_BITES>;
	//using get_userinfo_session_map_type = CBareHashMap<ACE_UINT32, CGetUserInfoSession, EN_HASH_MAP_BITES>;
	using vbh_operation_session_map_type = CBareHashMap<ACE_UINT32, CVbhOperationSession, EN_HASH_MAP_BITES>;
	using shareinfo_session_type = CBareHashMap<ACE_UINT32, CShareInfoSession, EN_HASH_MAP_BITES>;

private:
	online_user_session_map_type m_mapOnlineUsers;

	login_session_map_type m_mapLoginSession;
	check_user_session_map_type m_mapCheckUserSession;
	transfer_session_map_type m_mapTransferSession;
	//get_userinfo_session_map_type m_mapGetUserInfoSession;
	vbh_operation_session_map_type m_mapVbhOperationSession;
	shareinfo_session_type m_shareinfoSession;

private:
	ACE_UINT32 m_nShareInfoSessionId = 1;
	const ACE_INT32 m_nShareTime = 300;

	ACE_INT32 m_RewardTime;
	ACE_INT32 m_SignLowTime;
	ACE_INT32 m_SignHighTime;

	CDistributeRewardTimer* m_pTimerDistributeReward = NULL;
	CDBDate m_latsDisRewardDate;

	CDscString m_strIDMCryptKey;    //IDC系统密钥
	CDscString m_strIDMUserID;    //系统账户ID

	VBE_IDC::CVbeHlsRouter m_hlsRouter;

	ACE_UINT32 m_nTokenSeq = 0;
	ACE_UINT32 m_nSessionID = 0;
	CVbhAgentService* m_pHas = nullptr;
	const CDscString m_strIpAddr;
	const ACE_INT32 m_nPort;
	CMcpAsynchAcceptor<CVbeIdcService>* m_pAcceptor = NULL;

private:
	CDscDatabase m_database;
	CDBConnection m_dbConnection;

public:
	CDscString* m_pRewardUserArray;
	double* m_pRewardNumsArray;
	ACE_INT32 m_nRewardUserNum;

	void StartDistributeReward(CDBDate& toDay);
	void UpdateObtainStatus(const CDscString& owner, double dNumIdm);
	ACE_UINT32 GetEarlyBirdCount(CDBDate& toDay);
	ACE_UINT8 GenerateRewardPool(double* rewardPool, ACE_UINT32 nSize);
};

#include "vbe_idc/vbe_idc_service/vbe_idc_service.inl"




#endif // !_VBE_SERVICE_H_0951345980123479







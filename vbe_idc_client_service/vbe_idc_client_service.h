#ifndef VBE_IDC_CLIENT_SERVICE_H_275873835682456
#define VBE_IDC_CLIENT_SERVICE_H_275873835682456


#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"

#include "vbe_idc/vbe_idc_client_sdk/vbe_idc_client_sdk.h"

#include "vbh/common/vbh_encrypt_lib.h"

#include "openssl/ec.h"

#include "dsc/protocol/hts/dsc_hts_service.h"
#include "dsc/service_timer/dsc_service_timer_handler.h"
#include "dsc/container/bare_hash_map.h"

class PLUGIN_EXPORT CVbeIdcClientService : public CDscHtsClientService
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBE_IDC::EN_VBE_IDC_CLIENT_SERVICE_TYPE
	};

public:
	ACE_INT32 OnInit(void);
	ACE_INT32 OnExit(void);
protected:
	BEGIN_HTS_MESSAGE_BIND
		BIND_HTS_MESSAGE(VBE_IDC::CRegisterUserRegCltRsp)
		BIND_HTS_MESSAGE(VBE_IDC::CLoginVbeCltRsp)
		BIND_HTS_MESSAGE(VBE_IDC::CTransferVbeCltRsp)
		BIND_HTS_MESSAGE(VBE_IDC::CGetUserInfoVbeCltRsp)
		BIND_HTS_MESSAGE(VBE_IDC::CUpdateVbeCltRsp)
		BIND_HTS_MESSAGE(VBE_IDC::CIdcInterfaceVbeCltRsp)
		END_HTS_MESSAGE_BIND

public:
	ACE_INT32 OnHtsMsg(VBE_IDC::CRegisterUserRegCltRsp& rRegistUserRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBE_IDC::CLoginVbeCltRsp& rLoginRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBE_IDC::CTransferVbeCltRsp& rTransferRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBE_IDC::CGetUserInfoVbeCltRsp& rGetUserInfoRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBE_IDC::CUpdateVbeCltRsp& rUpdateVbeCltRsp, CMcpHandler* pMcpHandler);
	ACE_INT32 OnHtsMsg(VBE_IDC::CIdcInterfaceVbeCltRsp& rIdcInterfaceVbeCltRsp, CMcpHandler* pMcpHandler);

protected:
	BEGIN_BIND_DSC_MESSAGE
		DSC_BIND_MESSAGE(VBE_IDC::CInitSdk)
		DSC_BIND_MESSAGE(VBE_IDC::CRegisterUserApiCltReq)
		DSC_BIND_MESSAGE(VBE_IDC::CLoginApiCltReq)
		DSC_BIND_MESSAGE(VBE_IDC::CGetUserInfoApiCltReq)
		DSC_BIND_MESSAGE(VBE_IDC::CUpdateUserInfoApiCltReq)
		DSC_BIND_MESSAGE(VBE_IDC::CIDCCommonApiCltReq)
		DSC_BIND_MESSAGE(VBE_IDC::CTransferApiCltReq)
		END_BIND_DSC_MESSAGE

public:
	void OnDscMsg(VBE_IDC::CInitSdk& rInitSdk, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CRegisterUserApiCltReq& rRegistUserReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CLoginApiCltReq& rLoginReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CGetUserInfoApiCltReq& rGetUserInfoReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CUpdateUserInfoApiCltReq& rUpdateUserInfoApiCltReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CIDCCommonApiCltReq& rIDCCommonApiCltReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
	void OnDscMsg(VBE_IDC::CTransferApiCltReq& rTransferReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);

	virtual ACE_INT32 OnConnectedNodify(CMcpClientHandler* pMcpClientHandler) override;
	virtual void OnNetworkError(CMcpHandler* pMcpHandler) override;
	ACE_INT32 OnConnectFailedNodify(PROT_COMM::CDscIpAddr& remoteAddr, const ACE_UINT32 nHandleID) override;

private:
	class CVbePeer
	{
	public:
		CDscString m_strIpAddr;
		ACE_INT32 m_nPort = 0;
		ACE_UINT32 m_nHandleID = 0;
		bool m_bConnected = false;
		DSC::CDscShortVector <ACE_UINT32> m_channels;
	public:
		ACE_UINT32 m_nKey = 0;
		CVbePeer* m_pPrev = NULL;
		CVbePeer* m_pNext = NULL;
	};
	
	class COnlineUser;

	//通用基础session 
	class IBaseSession : public CDscServiceTimerHandler
	{
	public:
		IBaseSession(CVbeIdcClientService& rRegistUserService);

	public:
		virtual void OnNetError(void) = 0; //网络出错时，释放本session的函数

	public:
		ACE_UINT32 m_nCltSessionID; //客户端sessionID
		ACE_UINT32 m_nSrcSessionID; //发送业务请求方的sessionID

	public:
		ACE_UINT32 m_nIndex = 0; //使用 CDscTypeArray 容器必须具备的接口

	protected:
		CVbeIdcClientService& m_rClientBaseService;
	};

	class CHandleSession  //管理通信句柄的session
	{
	public:
		CDscTypeArray<IBaseSession> m_arrSession;
		CMcpHandler* m_pMcpHandler = NULL;

	public:
		ACE_UINT32 m_nIndex = 0; //使用 CDscTypeArray 容器必须具备的接口

	public:
		ACE_UINT32 m_nKey = 0;
		CHandleSession* m_pPrev = NULL;
		CHandleSession* m_pNext = NULL;
	};

	//注册用户的session
	class CRegisterUserSession : public IBaseSession
	{
	public:
		CRegisterUserSession(CVbeIdcClientService& rClientBaseService);
		~CRegisterUserSession();

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		ACE_INT32 m_nChannelID;
		bool m_waitingSend;
		DSC::CDscShortBlob m_nonce;
		DSC::CDscShortBlob m_userInfo;
		DSC::CDscShortBlob m_serverCryptKey;

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CRegisterUserSession* m_pPrev = NULL;//hash map用到
		CRegisterUserSession* m_pNext = NULL;//hash map用到

	};

	class CLoginSession : public IBaseSession
	{
	public:
		CLoginSession(CVbeIdcClientService& rClientBaseService);
		~CLoginSession();

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		bool m_waitingSend;
		DSC::CDscShortBlob m_nonce;
		DSC::CDscShortBlob m_userKey;
		DSC::CDscShortBlob m_cryptUserKey;
		CVbePeer* m_VbeServer = nullptr;

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CLoginSession* m_pPrev = NULL;//hash map用到
		CLoginSession* m_pNext = NULL;//hash map用到

	};

	class CGetUserInfoSession : public IBaseSession
	{
	public:
		CGetUserInfoSession(CVbeIdcClientService& rClientBaseService);
		~CGetUserInfoSession();

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:

		DSC::CDscShortBlob m_nonce;
		COnlineUser* m_pOnlineUser;
		ACE_UINT32 m_nQueryType;
	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CGetUserInfoSession* m_pPrev = NULL;//hash map用到
		CGetUserInfoSession* m_pNext = NULL;//hash map用到
	};

	class CUpdateSession : public IBaseSession
	{
	public:
		CUpdateSession(CVbeIdcClientService& rClientBaseService);
		~CUpdateSession();

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		DSC::CDscShortBlob m_nonce;
		COnlineUser* m_pOnlineUser;
		ACE_UINT32 m_nUpdateType;

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CUpdateSession* m_pPrev = NULL;//hash map用到
		CUpdateSession* m_pNext = NULL;//hash map用到
	};

	class CIdcInterfaceSession : public IBaseSession
	{
	public:
		CIdcInterfaceSession(CVbeIdcClientService& rClientBaseService);
		~CIdcInterfaceSession();

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		DSC::CDscShortBlob m_nonce;
		COnlineUser* m_pOnlineUser;
		ACE_UINT32 m_nInterfaceType;

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CIdcInterfaceSession* m_pPrev = NULL;//hash map用到
		CIdcInterfaceSession* m_pNext = NULL;//hash map用到
	};

	class CTransferSession : public IBaseSession
	{
	public:
		CTransferSession(CVbeIdcClientService& rClientBaseService);
		~CTransferSession();

	public:
		virtual void OnTimer(void) override;
		virtual void OnNetError(void) override;

	public:
		DSC::CDscShortBlob m_nonce;
		COnlineUser* m_pOnlineUser;

	public:
		ACE_UINT32 m_nKey = 0;//hash map用到
		CTransferSession* m_pPrev = NULL;//hash map用到
		CTransferSession* m_pNext = NULL;//hash map用到

	};

	class COnlineUser
	{
	public:
		COnlineUser(CVbeIdcClientService& srv);
		~COnlineUser();
	public:
		DSC::CDscShortBlob m_userKey;
		DSC::CDscShortBlob m_cryptUserKey;
		ACE_UINT32 m_nToken;
		ACE_UINT32 m_nVbeToken;
		CVbePeer* m_VbeServer = nullptr;

	public:
		CVbeIdcClientService& m_clientService;
	public:
		ACE_UINT32 m_nKey = 0;
		COnlineUser* m_pPrev = NULL;
		COnlineUser* m_pNext = NULL;
	};

private:
	enum
	{
		EN_HASH_MAP_BITES = 16,
		EN_SESSION_TIMEOUT_VALUE = 60,
		EN_ONLINE_USER_TIMEOUT_VALUE = 600,
	};


public:
	void OnTimeOut(CRegisterUserSession* pRegistUserSession);
	void OnNetError(CRegisterUserSession* pRegistUserSession);

	void OnTimeOut(CLoginSession* pLoginSession);
	void OnNetError(CLoginSession* pLoginSession);

	void OnTimeOut(CGetUserInfoSession* pGetUserInfoSession);
	void OnNetError(CGetUserInfoSession* pGetUserInfoSession);

	void OnTimeOut(CUpdateSession* rUpdateSession);
	void OnNetError(CUpdateSession* rUpdateSession);

	void OnTimeOut(CIdcInterfaceSession* rIdcInterfaceSession);
	void OnNetError(CIdcInterfaceSession* rIdcInterfaceSession);

	void OnTimeOut(CTransferSession* pTransferSession);
	void OnNetError(CTransferSession* pTransferSession);

private:
	using register_user_session_map_type = CBareHashMap<ACE_UINT32, CRegisterUserSession, EN_HASH_MAP_BITES>;
	using login_session_map_type = CBareHashMap<ACE_UINT32, CLoginSession, EN_HASH_MAP_BITES>;
	using get_userinfo_session_map_type = CBareHashMap<ACE_UINT32, CGetUserInfoSession, EN_HASH_MAP_BITES>;
	using update_session_map_type = CBareHashMap<ACE_UINT32, CUpdateSession, EN_HASH_MAP_BITES>;
	using idc_interface_session_map_type = CBareHashMap<ACE_UINT32, CIdcInterfaceSession, EN_HASH_MAP_BITES>;
	using transfer_session_map_type = CBareHashMap<ACE_UINT32, CTransferSession, EN_HASH_MAP_BITES>;
	
	using vbe_peer_map_type = CBareHashMap<ACE_UINT32, CVbePeer, EN_HASH_MAP_BITES>; // channelID -> vbe peer
	using online_user_session_map_type = CBareHashMap<ACE_UINT32, COnlineUser, EN_HASH_MAP_BITES>;  // token -> loginuser

private:

	void OnRegisterUserResponse(ACE_INT32 nReturnCode,  ACE_UINT32 nRequestID, DSC::CDscShortBlob *userKey = nullptr);
	void OnLoginResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 token = 0);
	void OnGetUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nQueryType, DSC::CDscShortBlob* userinfo = nullptr);
	void OnUpdateResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nUpdateType, DSC::CDscShortBlob* userinfo = nullptr);
	void OnIdcInterfaceResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, ACE_UINT32 nInterfaceType, DSC::CDscShortBlob* userinfo = nullptr);
	void OnTransferResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, DSC::CDscShortBlob* transKey = nullptr);
	
	ACE_INT32 SendRequest(CRegisterUserSession* pSession);
	ACE_INT32 SendRequest(CLoginSession* pSession);
	ACE_INT32 SendRequest(CTransferSession* pSession);

	void MakeNonce(DSC::CDscShortBlob& rNonce);
	ACE_UINT32 NewToken();

	template<typename MAP_TYPE, typename TYPE>
	void InsertSession(MAP_TYPE& mapSession, TYPE* session);

	template<typename MAP_TYPE>
	IBaseSession* EraseSession(MAP_TYPE& mapSession, ACE_UINT32 nSessionID);
	template<typename REQ_TYPE, typename WRAPPER_TYPE>
	ACE_INT32 EncryptSendRequest(REQ_TYPE &req, WRAPPER_TYPE &wrapper, const DSC::CDscShortBlob &envelopeKey, ACE_UINT32 nHandleID);


	ACE_INT32 LoadPeerInfo(void);
	ACE_INT32 ConnectRegServer(void);
	ACE_INT32 ConnectVbeServer(CVbePeer *vbeServer);

	register_user_session_map_type m_mapRegisterUserSession;
	login_session_map_type m_mapLoginSession;
	get_userinfo_session_map_type m_mapGetUserInfoSession;
	update_session_map_type m_mapUpdateSession;
	idc_interface_session_map_type m_mapIdcInterfaceSession;
	transfer_session_map_type m_mapTransferSession;

	online_user_session_map_type m_mapOnlineUsers;

	ACE_UINT32 m_nSessionID = 1;
	ACE_UINT32 m_seed = 0;
	ACE_UINT32 m_nTokenSeq = 0;

	CVbePeer* m_pRegServer = nullptr;
	DSC::CDscList <CVbePeer*> m_lstVbePeers;
	vbe_peer_map_type m_mapVbePeers;

	VBE_IDC_SDK::IClientSdkMsgCallback *m_pCallback = nullptr;
	DSC::CDscShortBlob m_peerEnvelopeKey;
	bool m_bReady = false;

};

#include "vbe_idc/vbe_idc_client_service/vbe_idc_client_service.inl"

#endif

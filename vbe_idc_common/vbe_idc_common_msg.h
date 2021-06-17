#ifndef _VBE_IDC_COMMON_MSG_H_81045682375601561236
#define _VBE_IDC_COMMON_MSG_H_81045682375601561236

#include "dsc/codec/dsc_codec/dsc_codec.h"
#include "dsc/codec/json_codec/json_codec_bind_macros.h"
#include "dsc/db/db_lib/db_datetime.h"

#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"

namespace VBE_IDC
{
	class CInitSdk
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_INIT_SDK_MSG
		};

	public:
		DSC_BIND_ATTR(m_pCallBack);

	public:
		void* m_pCallBack = NULL;
	};

	class CLoginApiCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_LOGIN_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_userKey, m_cryptUserKey);

	public:
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_userKey;
		DSC::CDscShortBlob m_cryptUserKey;
	};

	class CLoginCltVbeReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_LOGIN_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_userKey, m_data);

	public:
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_userKey;
		DSC::CDscShortBlob m_data; // nonce
	};

	class CLoginVbeCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_LOGIN_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_data);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_data; // nonce+token
	};

	class CRegisterUserApiCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_REGIST_USER_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nChannelID, m_nSrcRequestID, m_userInfo, m_serverCryptKey);

	public:

		ACE_UINT32 m_nChannelID; //�û�����channel

		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_userInfo;
		DSC::CDscShortBlob m_serverCryptKey;
	};

	class CRegisterUserCltRegReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_REGIST_USER_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_data);

	public:
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_data; // nonce+userinfo
	};

	class CRegisterUserRegHlsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_REGIST_USER_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nChannelID, m_nSrcRequestID, m_userInfo, m_serverCryptKey);

	public:
		ACE_INT32 m_nChannelID;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_userInfo; 
		DSC::CDscShortBlob m_serverCryptKey;
	};

	class CRegisterUserHlsRegRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_REGIST_USER_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_userKey);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_userKey;
	};

	class CRegisterUserRegCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_REGIST_USER_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_data);
	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_data; // nonce + userkey + cryptUserKey
	};

	class CCheckUsersVbeHlsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_CHECK_USERS_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_lstUserKey);

	public:
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortList<DSC::CDscShortBlob> m_lstUserKey;
	};
	class CCheckUsersHlsVbeRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_CHECK_USERS_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
	};

	class CLockUsersVbeHlsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_LOCK_USERS_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_userKeyList);

	public:
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortList<DSC::CDscShortBlob> m_userKeyList;
	};
	

	class CLockUsersHlsVbeRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_LOCK_USERS_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_nLockKey);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nLockKey;
	};

	class CUnlockUsersVbeHlsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_UNLOCK_USERS_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nLockKey, m_userKeyList);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nLockKey;
		DSC::CDscShortList<DSC::CDscShortBlob> m_userKeyList;
	};

	class CUnlockUsersHlsVbeRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_UNLOCK_USERS_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode,m_nSrcRequestID);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
	};

	class CTransferApiCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_nTotalCoin, m_userKeyList, m_coinList);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		ACE_UINT32 m_nTotalCoin;
		DSC::CDscShortList<DSC::CDscShortBlob> m_userKeyList;
		DSC::CDscShortList<ACE_UINT32> m_coinList;
	};

	class CTransferCltVbeReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_data);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		DSC::CDscShortBlob m_data; // nonce + total coin + user key list + coin list
	};

	class CTransferVbeCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode,m_nSrcRequestID, m_data);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_data; // nonce + trans key
	};

	class CTransferVbeHlsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_bMain,m_nSrcRequestID, m_sponsorUserKey, m_propose);
	public:
		bool m_bMain;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_sponsorUserKey;
		DSC::CDscShortBlob m_propose;
	};

	class CTransferHlsVbeRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_nChannelID, m_transKey);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nChannelID;
		DSC::CDscShortBlob m_transKey;
	};

	class CTransferHlsHlsReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_HLS_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_propose);
	public:
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_propose;
	};

	class CTransferHlsHlsRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_TRANSFER_HLS_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID);
	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
	};

	class CGetUserInfoApiCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_GET_USERINFO_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_nQueryType);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		ACE_UINT32 m_nQueryType;
	};

	class CGetUserInfoCltVbeReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_GET_USERINFO_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_nQueryType, m_data);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		ACE_UINT32 m_nQueryType;
		DSC::CDscShortBlob m_data; // nonce
	};

	class CGetUserInfoVbeCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_GET_USERINFO_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_data);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_data; // nonce+userinfo
	};

	class CUpdateUserInfoApiCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_UPDATE_USERINFO_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_nActionID, m_nUpdateType, m_userInfo);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		ACE_UINT32 m_nActionID; //�����û���ϢΪ1  ת��Ϊ2
		ACE_UINT32 m_nUpdateType;
		DSC::CDscShortBlob m_userInfo;
	};

	class CUpdateCltVbeReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_UPDATE_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_data);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		DSC::CDscShortBlob m_data; // nonce + userInfo + actionID + updateType
	};

	class CUpdateVbeCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_UPDATE_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_data);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscShortBlob m_data; // nonce +  key
	};

	class CIdcInterfaceCltVbeReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_IDC_INTERFACE_REQ_MSG
		};

	public:
		DSC_BIND_ATTR(m_nSrcRequestID, m_nToken, m_data);

	public:
		ACE_UINT32 m_nSrcRequestID;
		ACE_UINT32 m_nToken;
		DSC::CDscShortBlob m_data; // nonce + userInfo + interfaceType
	};

	class CIdcInterfaceVbeCltRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_IDC_INTERFACE_RSP_MSG
		};

	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nSrcRequestID, m_data);

	public:
		ACE_INT32 m_nReturnCode;
		ACE_UINT32 m_nSrcRequestID;
		DSC::CDscBlob m_data; // nonce +  info
	};

	//��Ȩ����
	class CIDChainAuthNotifyReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_IDC_AUTH_NOTIFY_REQ
		};

	public:
		DSC_BIND_ATTR(m_token, m_nSessionID);

	public:
		DSC::CDscBlob m_token;
		ACE_UINT32 m_nSessionID;
	};

	class CIDChainAuthNotifyRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_IDC_AUTH_NOTIFY_RSP
		};

	public:
		DSC_BIND_ATTR(m_nSessionID);

	public:
		ACE_UINT32 m_nSessionID;
	};
	///////////////////////////////////////////////////////////////////
	//ҵ�����json���ݽṹ
	class CJsonIDMUserOriInfo
	{
	public:
		JSON_BIND_ATTR(name, idNum, mail, phone, idm, force);

	public:
		CDscString name;
		CDscString idNum;
		CDscString mail;
		CDscString phone;
		double idm;
		double force;
	};

	class CJsonIDMAssetInfo
	{
	public:
		JSON_BIND_ATTR(IDMBalance, ForceBalance, IDMBonus);

	public:
		double IDMBalance;
		double ForceBalance;
		double IDMBonus;
	};

	class CJsonShareInfoUrlID
	{
	public:
		JSON_BIND_ATTR(shareInfoUrlID);

	public:
		ACE_UINT32 shareInfoUrlID;
	};

	class CJsonDownloadShareInfo
	{
	public:
		JSON_BIND_ATTR(shareInfo);

	public:
		CDscString shareInfo;
	};

	class CJsonShareData
	{
	public:
		JSON_BIND_ATTR(shareTarget, shareData, shareTime, targetDid);

	public:
		CDscString shareTarget;
		CDscString shareData;
		CDBDateTime shareTime;
		CDscString targetDid;
	};

	class CJsonShareDataList
	{
	private:

	public:
		JSON_BIND_ATTR(data);

	public:
		DSC::CDscList<CJsonShareData> data;
	};

	class CJsonQueryShareInfoHistory
	{
	public:
		JSON_BIND_ATTR(shareInfoHistory);

	public:
		CDscString shareInfoHistory;
	};

	class CJsonQueryAssetHistory
	{
	public:
		JSON_BIND_ATTR(assetInfoHistory, startIndex, endIndex);

	public:
		CDscString assetInfoHistory;
		ACE_INT32 startIndex;
		ACE_INT32 endIndex;
	};

	class CJsonIDMHistoryData
	{
	public:
		JSON_BIND_ATTR(coinType, owner, type, createTime, IDMNum, TxId, Addr);

	public:

		ACE_UINT32 coinType;    //�ҵ�����   0:ȫ�� 1:IDM
		CDscString owner;       //������¼��ӵ����
		ACE_UINT32 type;        //����: 0:ȫ�� 1:���� 2:ת�� 3:��� 4:��� 5: ����
		CDBDateTime createTime;   //�ü�¼�Ĳ���ʱ��

		double IDMNum;           //���漰��IDM��Ŀ
		CDscString TxId;      //����ID
		CDscString Addr;      //��ضԷ��ĵ�ַ�����˺�
	};

	class CJsonIDMHistoryDataList
	{
	public:
		JSON_BIND_ATTR(data);

	public:
		DSC::CDscList<CJsonIDMHistoryData> data;
	};

	class CJsonQueryForceHistory
	{
	public:
		JSON_BIND_ATTR(forceInfoHistory, startIndex, endIndex);

	public:
		CDscString forceInfoHistory;
		ACE_INT32 startIndex;
		ACE_INT32 endIndex;
	};

	class CJsonForceHistoryData
	{
	public:
		JSON_BIND_ATTR(awardTime, awardType, num);

	public:
		CDBDateTime awardTime;
		ACE_UINT32 awardType;    //����: 1:ע�� 2:�ֻ�����֤ 3:������֤ 4:�齱 5: ��ע���ں�
		double num;
	};

	class CJsonForceHistoryDataList
	{
	public:
		JSON_BIND_ATTR(data);

	public:
		DSC::CDscList<CJsonForceHistoryData> data;
	};

	class CJsonQueryEarlyBirdHistory
	{
	public:
		JSON_BIND_ATTR(status, createTime, numBonus, Txid);

	public:
		ACE_UINT32 status;
		CDscString createTime;
		double numBonus;
		CDscString Txid;
	};

	class CJsonEarlyBirdPeopleCount
	{
	public:
		JSON_BIND_ATTR(count);

	public:
		ACE_UINT32 count;
	};

	class CJsonGetEarlyBirdRankList
	{
	public:
		JSON_BIND_ATTR(num, rankList);

	public:
		ACE_UINT32 num;
		CDscString rankList;
	};

	class CJsonRankData
	{
	public:
		JSON_BIND_ATTR(user, num);

	public:
		CDscString user;    //����: 1:ע�� 2:�ֻ�����֤ 3:������֤ 4:�齱 5: ��ע���ں�
		double num;
	};

	class CJsonRankDataList
	{
	public:
		JSON_BIND_ATTR(data);

	public:
		DSC::CDscList<CJsonRankData> data;
	};

	class CJsonIfEarlyBirdSign
	{
	public:
		JSON_BIND_ATTR(sign);

	public:
		ACE_INT32 sign;
	};

	class CJsonSignHistoryRsp
	{
	public:
		JSON_BIND_ATTR(data);

	public:
		CDscString data;
	};

	class CJsonSignHistory
	{
	public:
		JSON_BIND_ATTR(status, signTime);

	public:
		ACE_UINT64 status;
		CDBDate signTime;
	};

	class CJsonSignHistoryList
	{
	public:
		JSON_BIND_ATTR(data);

	public:
		DSC::CDscList<CJsonSignHistory> data;
	};
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//ҵ������û��ϴ�����
	//IDC�û�����ԭ��
	class CIDCAddForce
	{
	public:
		DSC_BIND_ATTR(m_nType, m_nForce)

	public:
		ACE_UINT32 m_nType;   //1:ע�� 2:�ֻ�����֤ 3:������֤ 4:�齱 5: ��ע���ں�
		ACE_INT32 m_nForce;
	};

	//IDC�û�ת����Ϣ
	class CTransInfoCltIdc
	{
	public:
		DSC_BIND_ATTR(m_dIDMNum, m_strTargetKey)

	public:
		double m_dIDMNum;   
		CDscString m_strTargetKey;
	};

	class CTransInfoIdcVbh
	{
	public:
		DSC_BIND_ATTR(m_dIDMNum, m_strUserKey, m_strTargetKey)

	public:
		double m_dIDMNum;
		CDscString m_strUserKey;
		CDscString m_strTargetKey;
	};
	
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//��vbh�޹ص���ز���
	class CIDCCommonApiCltReq
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_IDC_COMMON_REQ_MSG
		};
	public:
		DSC_BIND_ATTR(m_nReqID, m_nReqType, m_nToken, m_userInfo);
	public:
		ACE_INT32 m_nReqID;
		ACE_UINT32 m_nReqType;
		ACE_UINT32 m_nToken;
		DSC::CDscShortBlob m_userInfo;
	};

	class CIDCCommonCltApiRsp
	{
	public:
		enum
		{
			EN_MSG_ID = VBE_IDC::EN_IDC_COMMON_RSP_MSG
		};
	public:
		DSC_BIND_ATTR(m_nReturnCode, m_nReqID, m_nType, m_info);
	public:
		ACE_INT32 m_nReturnCode;
		ACE_INT32 m_nReqID;
		ACE_INT32 m_nType;
		DSC::CDscBlob m_info;
	};

	class CShareInfoReqComm
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_SHARE_INFO
		};
	public:
		DSC_BIND_ATTR(m_sharerSign, m_shareInfo);

	public:
		DSC::CDscBlob m_sharerSign;
		DSC::CDscBlob m_shareInfo;
	};

	class CDownLoadShareInfoReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_DOWNLOAD_SHARE_INFO
		};
	public:
		DSC_BIND_ATTR(m_viewerSign, m_shareInfoUrl);

	public:
		DSC::CDscBlob m_viewerSign;
		ACE_UINT32 m_shareInfoUrl;
	};

	class CDeleteShareInfoReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_DELETE_SHARE_INFO
		};
	public:
		DSC_BIND_ATTR(m_shareInfoUrl);
	public:
		ACE_UINT32 m_shareInfoUrl;
	};

	class CQueryShareHistoryInfoReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_QUERY_SHARE_HISTORY
		};
	public:
		DSC_BIND_ATTR(m_nStartIndex, m_nEndIndex, m_nQueryType);
	public:
		ACE_INT32 m_nStartIndex;
		ACE_INT32 m_nEndIndex;
		ACE_INT32 m_nQueryType; //nQueryType:0 ��Ȩ �����¼ 1 �鿴��¼
	};

	class CGetAssetHistoryInfoReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_GET_ASSET_HISTORY
		};

	public:
		DSC_BIND_ATTR(m_nStartIndex, m_nEndIndex, m_nCoinType, m_nRecordType);
	public:
		ACE_INT32 m_nStartIndex;
		ACE_INT32 m_nEndIndex;

		ACE_INT32 m_nCoinType;
		ACE_INT32 m_nRecordType;
	};

	class CGetForceHistoryInfoReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_GET_FORCE_HISTORY
		};

	public:
		DSC_BIND_ATTR(m_nStartIndex, m_nEndIndex);
	public:
		ACE_UINT32 m_nStartIndex;
		ACE_UINT32 m_nEndIndex;
	};

	class CEarlyBirdSignReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_EARLY_BIRD_SIGN
		};
	public:
		DSC_BIND_ATTR();
	};

	class CGetEarlyBirdHistoryReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_GET_EARLY_BIRD_HISTORY
		};

	public:
		DSC_BIND_ATTR(m_queryTime);
	public:
		DSC::CDscBlob m_queryTime;
	};

	class CGetEarlyBirdPeopleCountReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_GET_EARLY_BIRD_PEOPLE_COUNT
		};

	public:
		DSC_BIND_ATTR(m_queryTime);
	public:
		DSC::CDscBlob m_queryTime;
	};

	class CGetEarlyBirdRankListReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_GET_EARLY_BIRD_RANK_LIST
		};

	public:
		DSC_BIND_ATTR(m_queryTime, m_nNum);
	public:
		DSC::CDscBlob m_queryTime;
		ACE_INT32 m_nNum;
	};

	class CIfEarlyBirdSignReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_IF_EARLY_BIRD
		};
	public:
		DSC_BIND_ATTR();
	};

	class CGetSignHistoryReq
	{
	public:
		enum
		{
			EN_IDM_INTERFACE_ID = VBE_IDC::EN_IDC_INTERFACE_GET_SIGN_HISTORY
		};

	public:
		DSC_BIND_ATTR(m_queryMonth);
	public:
		DSC::CDscBlob m_queryMonth;
	};
	
	///////////////////////////////////////////////////////////////////
}






#endif // !_VBE_COMMON_MSG_H_81045682375601561236

#ifndef CLIENT_C_API_H_328928987643289
#define CLIENT_C_API_H_328928987643289

#include <stdint.h>

#include "vbe_idc_client_c_sdk_export.h"

#ifdef __cplusplus 
extern "C" {
#endif
	typedef void(*on_ready) (void);
	typedef void(*on_abnormal) (void);
	typedef void(*on_exit) (void);

	//ע���û�Ӧ��
	typedef void(*on_regist_idc_user_rsp) (const int nReturnCode, const unsigned int nRequestID,
		const char* pUserKey, const unsigned int nUserKey /*ע��ʱ���ɵ�user-key*/);

	//��¼Ӧ��
	typedef void(*on_login_idc_rsp) (const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nToken /*��¼�󷵻ظ��û���token*/);

	//��ѯ�û���ϢӦ��
	typedef void(*on_query_idc_info_rsp)(const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nQueryType, const char* pInfo, const const unsigned int nInfoLen);

	//������ϢӦ��
	typedef void(*on_update_user_info_rsp)  (const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nUpdateType, const char* pTransKey, const unsigned int nTransKeyLen);

	//idcҵ����ؽӿ�Ӧ��
	typedef void(*on_common_rsp)(const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nInterfaceType, const char* pInfo, const unsigned int nInfoLen);

	

	//��ʼ��
	VBE_IDC_CLIENT_C_SDK_EXPORT int init_vbe_idc_c_sdk(
		on_ready pOnReady,
		on_abnormal pOnAbnormal,
		on_exit pOnExit,
		on_regist_idc_user_rsp pOnRegistUserRsp,
		on_login_idc_rsp pOnLoginIdcRsp,
		on_query_idc_info_rsp pOnQueryUserInfoRsp,
		on_update_user_info_rsp pOnUpdateRsp,
		on_common_rsp pOnQueryTransRsp);


	//������Կ������һ����Կ����base64������ʽ����, key���ڴ���api�ڲ�malloc�� �ϲ�ҵ���Լ�free
	VBE_IDC_CLIENT_C_SDK_EXPORT int create_crypt_key(char** ppCltKey, unsigned int* pnCltKeyLen, char** ppSvrKey, unsigned int* pnSvrKeyLen);

	//ע���û�
	VBE_IDC_CLIENT_C_SDK_EXPORT int reg_idc_user(const unsigned int nRequestID,
		char* pUserInfo, const unsigned int nUserInfoLen,
		char* pServerCryptKey, const unsigned int nServerCryptLen);

	VBE_IDC_CLIENT_C_SDK_EXPORT int Login(const unsigned int nReqID, const char* userKey,
		unsigned long long  nUserKeyLen, const char* cltKey, unsigned long long nCltKeyLen);

	//ֱ�Ӳ�ѯ�û�
	VBE_IDC_CLIENT_C_SDK_EXPORT int query_idc_user(const unsigned int nRequestID, const unsigned int nToken);

	//��Ȩ
	VBE_IDC_CLIENT_C_SDK_EXPORT int auth_idc_user(const unsigned int nRequestID, const unsigned int nToken);

	//�����û���Ϣ
	VBE_IDC_CLIENT_C_SDK_EXPORT int update_idc_user_info(const unsigned int nRequestID, const unsigned int nToken,
		char* pUserInfo, const unsigned int nUserInfoLen);
	//��ѯ�û��ʲ�
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcQueryUserAsset(const unsigned int nRequestID, const unsigned int nToken);
	//���ս���
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcReceiveBonus(const unsigned int nRequestID, const unsigned int nToken, double dNumBonus);
	//����ԭ��
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcAddForce(const unsigned int nRequestID, const unsigned int nToken, int nForce, int nType);
	//�μ�����ǩ��
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcBet2EarlyBird(const unsigned int nRequestID, const unsigned int nToken);
	//ת��
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcTransferIDM(const unsigned int nRequestID, const unsigned int nToken,
		char* pTargetKey, const unsigned int nTargetKeyLen, double dIDMNum);
	//������Ϣ
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomShareInfo(const unsigned int nRequestID, const unsigned int nToken,
		char* pSharerSign, const unsigned int nSharerSignLen, char* pShareInfo, const unsigned int nShareInfoLen);
	//���ط�����Ϣ
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomDownloadShareInfo(const unsigned int nRequestID, const unsigned int nToken,
		char* pViewerSign, const unsigned int nViewerSignLen, char* pShareInfoUrl, const unsigned int nShareInfoUrlLen);
	//ɾ��������Ϣ
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomDeleteShareInfo(const unsigned int nRequestID, const unsigned int nToken,
		char* pShareInfoUrl, const unsigned int nShareInfoUrlLen);
	//��ѯ������ʷ
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomQueryShareHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
		int nQueryType, int nStartIndex, int nEndIndex);
	//��ѯ�ʲ���ʷ
	/* 
		nCoinType 0 ȫ�� 1 IDM
		nRecordType 0:ȫ�� 1:���� 2:��� 3:��� 5: ���� 6.�μӶ�ս
	*/
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetAssetHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
		int nStartIndex, int nEndIndex, int nCoinType, int nRecordType);
	//��ѯԭ����ʷ
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetForceHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
		int nStartIndex, int nEndIndex);
	//����ǩ��
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomEarlyBirdSign(const unsigned int nRequestID, const unsigned int nToken);
	//����ǩ����ʷ 
	//querytime "YYYY-MM-DD"
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetEarlyBirdHistory(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen);
	//����μ���ս����
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetEarlyBirdPeopleCount(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen);
	//����ǩ�����а�
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetEarlyBirdRankList(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen, const unsigned int nNum);
	//�Ƿ��������ǩ��
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomIfEarlyBirdSign(const unsigned int nRequestID, const unsigned int nToken);
	//���²�ѯ����ǩ��   //querytime "YYYY-MM"
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetSignHistoryByMonth(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen);
	
	

	////ֱ�Ӳ�ѯ��������
	//VBE_IDC_CLIENT_C_SDK_EXPORT int32_t query_vbh_trans(const uint32_t nChannelID, const uint32_t nRequestID,
	//	char* pTransKey, const uint32_t nTransKeyLen, /*Ҫ��ѯ��trans-key*/
	//	char* pUserKey, const uint32_t nUserKeyLen, /*�û�key*/
	//	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*�û���Կ*/);

#ifdef __cplusplus 
}
#endif


#endif

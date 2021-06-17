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

	//注册用户应答
	typedef void(*on_regist_idc_user_rsp) (const int nReturnCode, const unsigned int nRequestID,
		const char* pUserKey, const unsigned int nUserKey /*注册时生成的user-key*/);

	//登录应答
	typedef void(*on_login_idc_rsp) (const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nToken /*登录后返回给用户的token*/);

	//查询用户信息应答
	typedef void(*on_query_idc_info_rsp)(const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nQueryType, const char* pInfo, const const unsigned int nInfoLen);

	//更新信息应答
	typedef void(*on_update_user_info_rsp)  (const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nUpdateType, const char* pTransKey, const unsigned int nTransKeyLen);

	//idc业务相关接口应答
	typedef void(*on_common_rsp)(const int nReturnCode, const unsigned int nRequestID,
		const unsigned int nInterfaceType, const char* pInfo, const unsigned int nInfoLen);

	

	//初始化
	VBE_IDC_CLIENT_C_SDK_EXPORT int init_vbe_idc_c_sdk(
		on_ready pOnReady,
		on_abnormal pOnAbnormal,
		on_exit pOnExit,
		on_regist_idc_user_rsp pOnRegistUserRsp,
		on_login_idc_rsp pOnLoginIdcRsp,
		on_query_idc_info_rsp pOnQueryUserInfoRsp,
		on_update_user_info_rsp pOnUpdateRsp,
		on_common_rsp pOnQueryTransRsp);


	//创建秘钥：创建一对秘钥，以base64编码形式返回, key的内存有api内部malloc， 上层业务自己free
	VBE_IDC_CLIENT_C_SDK_EXPORT int create_crypt_key(char** ppCltKey, unsigned int* pnCltKeyLen, char** ppSvrKey, unsigned int* pnSvrKeyLen);

	//注册用户
	VBE_IDC_CLIENT_C_SDK_EXPORT int reg_idc_user(const unsigned int nRequestID,
		char* pUserInfo, const unsigned int nUserInfoLen,
		char* pServerCryptKey, const unsigned int nServerCryptLen);

	VBE_IDC_CLIENT_C_SDK_EXPORT int Login(const unsigned int nReqID, const char* userKey,
		unsigned long long  nUserKeyLen, const char* cltKey, unsigned long long nCltKeyLen);

	//直接查询用户
	VBE_IDC_CLIENT_C_SDK_EXPORT int query_idc_user(const unsigned int nRequestID, const unsigned int nToken);

	//鉴权
	VBE_IDC_CLIENT_C_SDK_EXPORT int auth_idc_user(const unsigned int nRequestID, const unsigned int nToken);

	//更新用户信息
	VBE_IDC_CLIENT_C_SDK_EXPORT int update_idc_user_info(const unsigned int nRequestID, const unsigned int nToken,
		char* pUserInfo, const unsigned int nUserInfoLen);
	//查询用户资产
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcQueryUserAsset(const unsigned int nRequestID, const unsigned int nToken);
	//接收奖励
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcReceiveBonus(const unsigned int nRequestID, const unsigned int nToken, double dNumBonus);
	//增加原力
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcAddForce(const unsigned int nRequestID, const unsigned int nToken, int nForce, int nType);
	//参加早鸟签到
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcBet2EarlyBird(const unsigned int nRequestID, const unsigned int nToken);
	//转账
	VBE_IDC_CLIENT_C_SDK_EXPORT int idcTransferIDM(const unsigned int nRequestID, const unsigned int nToken,
		char* pTargetKey, const unsigned int nTargetKeyLen, double dIDMNum);
	//分享信息
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomShareInfo(const unsigned int nRequestID, const unsigned int nToken,
		char* pSharerSign, const unsigned int nSharerSignLen, char* pShareInfo, const unsigned int nShareInfoLen);
	//下载分享信息
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomDownloadShareInfo(const unsigned int nRequestID, const unsigned int nToken,
		char* pViewerSign, const unsigned int nViewerSignLen, char* pShareInfoUrl, const unsigned int nShareInfoUrlLen);
	//删除分享信息
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomDeleteShareInfo(const unsigned int nRequestID, const unsigned int nToken,
		char* pShareInfoUrl, const unsigned int nShareInfoUrlLen);
	//查询分享历史
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomQueryShareHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
		int nQueryType, int nStartIndex, int nEndIndex);
	//查询资产历史
	/* 
		nCoinType 0 全部 1 IDM
		nRecordType 0:全部 1:采收 2:提币 3:充币 5: 奖励 6.参加对战
	*/
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetAssetHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
		int nStartIndex, int nEndIndex, int nCoinType, int nRecordType);
	//查询原力历史
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetForceHistoryInfo(const unsigned int nRequestID, const unsigned int nToken,
		int nStartIndex, int nEndIndex);
	//早鸟签到
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomEarlyBirdSign(const unsigned int nRequestID, const unsigned int nToken);
	//早鸟签到历史 
	//querytime "YYYY-MM-DD"
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetEarlyBirdHistory(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen);
	//早鸟参加挑战人数
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetEarlyBirdPeopleCount(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen);
	//早鸟签到排行榜
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetEarlyBirdRankList(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen, const unsigned int nNum);
	//是否可以早鸟签到
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomIfEarlyBirdSign(const unsigned int nRequestID, const unsigned int nToken);
	//按月查询早鸟签到   //querytime "YYYY-MM"
	VBE_IDC_CLIENT_C_SDK_EXPORT int iDomGetSignHistoryByMonth(const unsigned int nRequestID, const unsigned int nToken,
		char* pQueryTime, const unsigned int nQueryTimeLen);
	
	

	////直接查询事务内容
	//VBE_IDC_CLIENT_C_SDK_EXPORT int32_t query_vbh_trans(const uint32_t nChannelID, const uint32_t nRequestID,
	//	char* pTransKey, const uint32_t nTransKeyLen, /*要查询的trans-key*/
	//	char* pUserKey, const uint32_t nUserKeyLen, /*用户key*/
	//	char* pUserCryptKey, const uint32_t nUserCryptKeyLen/*用户秘钥*/);

#ifdef __cplusplus 
}
#endif


#endif

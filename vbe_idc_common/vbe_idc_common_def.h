#ifndef _VBE_IDC_COMMON_DEF_H_18501245610467
#define _VBE_IDC_COMMON_DEF_H_18501245610467

#ifdef WIN32
#ifdef DECLARE_VBE_IDC_EXPORT
#define VBE_IDC_EXPORT __declspec(dllexport)
#else
#define VBE_IDC_EXPORT __declspec(dllimport)
#endif
#else
#ifdef __GNUC__
#define VBE_IDC_EXPORT __attribute__ ((visibility("default")))
#else
#define VBE_IDC_EXPORT
#endif
#endif

namespace VBE_IDC
{
	enum __ERRORS
	{
		EN_OK_TYPE = 0,
		EN_USER_NOT_LOGIN_ERROR_TYPE,
		EN_PARAM_ERROR_TYPE,
		EN_DECRYPT_ERROR_TYPE,
		EN_NONCE_ERROR_TYPE,
		EN_SYSTEM_ERROR_TYPE,
		EN_NETWORK_ERROR_TYPE,
		EN_TIMEOUT_ERROR_TYPE,
		EN_LOCK_ERROR_TYPE,
		EN_ALREADY_ENTER_EARLY_BIRD_ERROR_TYPE,
		IDC_SHARE_INFO_ADD_SESSION_ERROR_TYPE,
		IDC_SHARE_INFO_INSERT_DATABASE_ERRPR_TYPE,
		IDC_SHARE_INFO_CANNOT_FIND_ERRPR_TYPE,
		IDC_JSON_ENCODE_ERROR_TYPE,
		IDC_DELETE_INFO_UPDATE_DATABASE_ERRPR_TYPE,
		IDC_QUERY_INFO_SELECT_DATABASE_ERRPR_TYPE,
		IDC_CANNOT_EARLY_BIRD_SIGN_TYPE,
		IDC_DB_SQL_EXECUTE_FAILED_ERROR_TYPE,
		IDC_DB_SQL_EXECUTE_NO_CHANGE_ERROR_TYPE,
		IDC_QUERY_TIME_FORMAT_ERRPR_TYPE,
		IDC_QUERY_EARLY_BIRD_NO_INFO_ERROR_TYPE,
	};

	enum
	{
		EN_VBE_IDC_CHANNELELID = 1111
	};

	enum 
	{
		EN_VBE_CLIENT_APP_TYPE = 301,
		EN_VBE_SERVICE_APP_TYPE = 311,
		EN_VBE_REG_SERVICE_APP_TYPE = 321,
		EN_VBE_FINE_FOOD_SERVICE_APP_TYPE = 444,
	};
	// msg type
	enum 
	{
		EN_INIT_SDK_MSG = 301,
		EN_LOGIN_REQ_MSG,
		EN_LOGIN_RSP_MSG,
		EN_REGIST_USER_REQ_MSG,
		EN_REGIST_USER_RSP_MSG,
		EN_TRANSFER_REQ_MSG,
		EN_TRANSFER_RSP_MSG,
		EN_TRANSFER_HLS_REQ_MSG,
		EN_TRANSFER_HLS_RSP_MSG,
		EN_LOCK_USERS_REQ_MSG,
		EN_LOCK_USERS_RSP_MSG,
		EN_CHECK_USERS_REQ_MSG,
		EN_CHECK_USERS_RSP_MSG,
		EN_UNLOCK_USERS_REQ_MSG,
		EN_UNLOCK_USERS_RSP_MSG,
		EN_GET_USERINFO_REQ_MSG,
		EN_GET_USERINFO_RSP_MSG,
		EN_UPDATE_USERINFO_REQ_MSG,
		EN_UPDATE_REQ_MSG,
		EN_UPDATE_RSP_MSG,
		EN_IDC_COMMON_REQ_MSG,
		EN_IDC_COMMON_RSP_MSG,
		EN_IDC_INTERFACE_REQ_MSG,
		EN_IDC_INTERFACE_RSP_MSG,
		EN_IDC_AUTH_NOTIFY_REQ,
		EN_IDC_AUTH_NOTIFY_RSP,
	};
	enum 
	{
		EN_VBE_IDC_CLIENT_SERVICE_TYPE = 666,
		EN_VBE_REG_SERVICE_TYPE,
		EN_VBE_SERVICE_TYPE,
		EN_VBE_SERVICE_CONTAINER_TYPE,
		EN_VBE_HLS_SERVICE_TYPE,
		EN_VBE_VBH_AGENT_SERVICE_TYPE,
		EN_FINE_FOOD_SERVICE_TYPE,
	};
	enum 
	{
		EN_IDC_QUERY_USER_INFO_TYPE = 0,
		EN_IDC_AUTH_USER_TYPE,
		EN_IDC_QUERY_USER_ASSET_TYPE,
		EN_IDC_UPDATE_USER_INFO_TYPE = 7, //��ԭ��idc����ͬ�� �������Ҫɾ��
		EN_IDC_RECEIVE_BONUS_TYPE,
		EN_IDC_ADD_FORCE_TYPE,
		EN_IDC_ADD_TO_EARLY_BIRD_TYPE,
		EN_IDC_TRANS_TYPE,
	};
	enum
	{
		EN_IDC_INTERFACE_SHARE_INFO,
		EN_IDC_INTERFACE_DOWNLOAD_SHARE_INFO,
		EN_IDC_INTERFACE_DELETE_SHARE_INFO,
		EN_IDC_INTERFACE_QUERY_SHARE_HISTORY,
		EN_IDC_INTERFACE_GET_ASSET_HISTORY,
		EN_IDC_INTERFACE_GET_FORCE_HISTORY,
		EN_IDC_INTERFACE_EARLY_BIRD_SIGN,
		EN_IDC_INTERFACE_GET_EARLY_BIRD_HISTORY,
		EN_IDC_INTERFACE_GET_EARLY_BIRD_PEOPLE_COUNT,
		EN_IDC_INTERFACE_GET_EARLY_BIRD_RANK_LIST,
		EN_IDC_INTERFACE_IF_EARLY_BIRD,
		EN_IDC_INTERFACE_GET_SIGN_HISTORY,
	};
}



#endif
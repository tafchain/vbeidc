#include "vbe_idc/vbe_idc_client_c_sdk/vbe_idc_client_c_api.h"
#include "vbe_idc/vbe_idc_client_sdk/vbe_idc_client_sdk.h"

using namespace VBE_IDC_SDK;

static bool sReady = false;

void onReady()
{

}

void onAbnormal()
{

}

void onExit()
{

}

//注册用户应答
void onRegistIdcUserRsp(const int nReturnCode, const unsigned int nRequestID,
	const char* pUserKey, const unsigned int nUserKey /*注册时生成的user-key*/)
{

}

//登录应答
void onLoginIdcRsp(const int nReturnCode, const unsigned int nRequestID,
	const unsigned int nToken /*登录后返回给用户的token*/)
{

}

//查询用户信息应答
void onQueryIdcInfoRsp(const int nReturnCode, const unsigned int nRequestID,
const unsigned int nQueryType, const char* pInfo, const const unsigned int nInfoLen)
{

}

//更新信息应答
void onUpdateUserInfoRsp(const int nReturnCode, const unsigned int nRequestID,
	const unsigned int nUpdateType, const char* pTransKey, const unsigned int nTransKeyLen)
{

}

//idc业务相关接口应答
void onCommonRsp(const int nReturnCode, const unsigned int nRequestID,
	const unsigned int nInterfaceType, const char* pInfo, const unsigned int nInfoLen)
{

}

int main(int argc, char* argv[])
{
	int ret = init_vbe_idc_c_sdk(onReady, onAbnormal, onExit, onRegistIdcUserRsp, onLoginIdcRsp, onQueryIdcInfoRsp, onUpdateUserInfoRsp, onCommonRsp);

	const char* str = "hiorgermioooqre";
	while (1)
	{
		Sleep(1000);
		if (sReady) {
			printf("RegisterUser");
			break;
		}
	}
	while (true)
	{
		Sleep(1000);
	}
	return 0;
}


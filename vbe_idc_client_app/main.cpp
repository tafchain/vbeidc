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

//ע���û�Ӧ��
void onRegistIdcUserRsp(const int nReturnCode, const unsigned int nRequestID,
	const char* pUserKey, const unsigned int nUserKey /*ע��ʱ���ɵ�user-key*/)
{

}

//��¼Ӧ��
void onLoginIdcRsp(const int nReturnCode, const unsigned int nRequestID,
	const unsigned int nToken /*��¼�󷵻ظ��û���token*/)
{

}

//��ѯ�û���ϢӦ��
void onQueryIdcInfoRsp(const int nReturnCode, const unsigned int nRequestID,
const unsigned int nQueryType, const char* pInfo, const const unsigned int nInfoLen)
{

}

//������ϢӦ��
void onUpdateUserInfoRsp(const int nReturnCode, const unsigned int nRequestID,
	const unsigned int nUpdateType, const char* pTransKey, const unsigned int nTransKeyLen)
{

}

//idcҵ����ؽӿ�Ӧ��
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


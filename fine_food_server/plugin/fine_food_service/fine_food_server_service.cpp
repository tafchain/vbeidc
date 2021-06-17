
#include "fine_food_server_service.h"
#include "dsc/storage/block_storage/dsc_block_format.h"
//#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg.h"


ACE_INT32 CFineFoodService::OnInit(void)
{
	if (CDscReactor::OnInit())
	{
		DSC_RUN_LOG_ERROR("ID Chain Service  init failed!");

		return -1;
	}
	return 0;
}

ACE_INT32 CFineFoodService::OnExit(void)
{
	return CDscReactor::OnExit();
}

void CFineFoodService::OnDscMsg(VBE_IDC::CIDChainAuthNotifyReq& req, const CDscMsg::CDscMsgAddr& rSrcMsgAddr)
{
	VBE_IDC::CIDChainAuthNotifyRsp rsp;
	rsp.m_nSessionID = req.m_nSessionID;

	if (this->SendDscMessage(rsp, rSrcMsgAddr))
	{
		DSC_RUN_LOG_INFO("SendDscMessage failed");
	}
}

#include "vbe_idc/vbe_idc_service/vbh_agent_service.h"
#include "vbe_idc/vbe_idc_service/vbe_idc_service.h"

CVbhAgentService::CVbhAgentService()
{
}

ACE_INT32 CVbhAgentService::OnInit(void)
{
	if (CVbhAdapterBase::OnInit())
	{
		DSC_RUN_LOG_ERROR("transform agent service init failed!");

		return -1;
	}

	return 0;
}

ACE_INT32 CVbhAgentService::OnExit(void)
{
	return 0;
}

void CVbhAgentService::OnQueryUserInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& userInfo)
{
	m_pVbeService->OnQueryUserInfoResponse(nReturnCode, nRequestID, userInfo);
}

void CVbhAgentService::OnQueryTransactionInfoResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscBlob& tranInfo)
{
	m_pVbeService->OnQueryTransactionInfoResponse(nReturnCode, nRequestID, tranInfo);
}

void CVbhAgentService::OnProposeResponse(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID, const DSC::CDscShortBlob& transKey)
{
	m_pVbeService->OnProposeResponse(nReturnCode, nRequestID, transKey);
}

void CVbhAgentService::OnProposeResultNotify(ACE_INT32 nReturnCode, ACE_UINT32 nRequestID)
{
	m_pVbeService->OnProposeResultNotify(nReturnCode, nRequestID);
}

void CVbhAgentService::SetVbeService(CVbeIdcService* pSrv)
{
	m_pVbeService = pSrv;
}

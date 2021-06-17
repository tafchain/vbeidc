#ifndef FINE_FOOD_SERVICE_H_4328989432876432894327432874329
#define FINE_FOOD_SERVICE_H_4328989432876432894327432874329

//#include "openssl/ec.h"

#include "dsc/protocol/hts/dsc_hts_service.h"
#include "dsc/protocol/mcp/mcp_asynch_acceptor.h"

#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_msg.h"
#include "dsc/storage/record_storage/record_storage.h"

class PLUGIN_EXPORT CFineFoodService : public CDscReactor
{
public:
	enum
	{
		EN_SERVICE_TYPE = VBE_IDC::EN_VBE_REG_SERVICE_TYPE
	};


public:
	virtual ACE_INT32 OnInit(void);
	virtual ACE_INT32 OnExit(void);

protected:
	BEGIN_BIND_DSC_MESSAGE
	DSC_BIND_MESSAGE(VBE_IDC::CIDChainAuthNotifyReq)
	END_BIND_DSC_MESSAGE
//
public:
	void OnDscMsg(VBE_IDC::CIDChainAuthNotifyReq& req, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
//	void OnDscMsg(BC::CIDChainUpdateReq& IDChainUpdateReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
//	void OnDscMsg(BC::CIDChainQueryReq& IDChainQueryReq, const CDscMsg::CDscMsgAddr& rSrcMsgAddr);
//
//private:
//	CRecordStorage m_Storage;
//
//private:
//	DSC::CDscBlob hash256(DSC::CDscBlob userInfo, char* buf, const ACE_UINT32 nBufSize);
};

//#include "bc_ca_server_service/bc_ca_server_service.inl"

#endif

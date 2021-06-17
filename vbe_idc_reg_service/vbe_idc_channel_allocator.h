#ifndef _VBE_IDC_CHANNEL_CONTAINER_H_8501227202728
#define _VBE_IDC_CHANNEL_CONTAINER_H_8501227202728


#include "vbe_idc/vbe_idc_common/vbe_idc_hls_router.h"

class CVbeIdcChannelAllocator : public VBE_IDC::CVbeHlsRouter
{
public:
	CVbeIdcChannelAllocator();
	virtual ~CVbeIdcChannelAllocator();

	ACE_INT32 Init();

	// ������������ʵ�ֲ�ͬ�ķ������
	virtual ACE_UINT32 AllocChannel() = 0;
};





#endif

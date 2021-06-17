#ifndef _VBE_IDC_RR_CHANNEL_ALLOCATOR_H_80127027720112617
#define _VBE_IDC_RR_CHANNEL_ALLOCATOR_H_80127027720112617

#include "vbe_idc/vbe_idc_reg_service/vbe_idc_channel_allocator.h"

// ÂÖÑ¯·ÖÅäÆ÷
class CVbeIdcRrChannelAllocator : public CVbeIdcChannelAllocator
{
public:
	CVbeIdcRrChannelAllocator();
	virtual ~CVbeIdcRrChannelAllocator();

	ACE_INT32 Init();

public:
	virtual ACE_UINT32 AllocChannel();

private:
	channel_map_dsc_addr_type::iterator m_iterator;
};



#endif

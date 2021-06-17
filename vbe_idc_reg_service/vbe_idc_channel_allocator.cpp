#include "vbe_idc/vbe_idc_reg_service/vbe_idc_channel_allocator.h"

CVbeIdcChannelAllocator::CVbeIdcChannelAllocator()
{
}

CVbeIdcChannelAllocator::~CVbeIdcChannelAllocator()
{
}

ACE_INT32 CVbeIdcChannelAllocator::Init()
{
	if (VBE_IDC::CVbeHlsRouter::Open())
	{
		return -1;
	}
}

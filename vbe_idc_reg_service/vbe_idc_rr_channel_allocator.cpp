#include "vbe_idc_rr_channel_allocator.h"

CVbeIdcRrChannelAllocator::CVbeIdcRrChannelAllocator()
{
}

CVbeIdcRrChannelAllocator::~CVbeIdcRrChannelAllocator()
{
}

ACE_INT32 CVbeIdcRrChannelAllocator::Init()
{
	if (CVbeIdcChannelAllocator::Init())
	{
		return -1;
	}

	m_iterator = m_mapChannelMapHlsAddr.begin();
}

ACE_UINT32 CVbeIdcRrChannelAllocator::AllocChannel()
{
	if (m_iterator == m_mapChannelMapHlsAddr.end())
	{
		m_iterator = m_mapChannelMapHlsAddr.begin();
	}

	auto ch = m_iterator->first;

	m_iterator++;

	return ch;
}

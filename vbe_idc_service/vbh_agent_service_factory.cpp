
#include "vbe_idc/vbe_idc_service/vbh_agent_service_factory.h"

CDscService* CVbhAgentServiceFactory::CreateDscService(void)
{
	return m_pHas;
}

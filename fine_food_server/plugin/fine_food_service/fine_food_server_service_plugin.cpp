#include "dsc/service/dsc_service_container.h"
#include "dsc/db/per/persistence.h"
#include "dsc/dsc_database_factory.h"

#include "fine_food_server_service.h"
#include "fine_food_server_service_plugin.h"


ACE_INT32 CFineFoodServicePlugin::OnInit(void)
{ 
	/*CFineFoodService* pIDChainService;

	pIDChainService = ::new(std::nothrow) CFineFoodService;
	if (CDscDispatcherCenterDemon::instance()->RegistDscTask(pIDChainService, CFineFoodService::EN_SERVICE_TYPE, 0))
	{
		DSC_DELETE(pIDChainService);
		DSC_RUN_LOG_ERROR("ID Chain Service regist failed");
		return -1;
	}*/
	RegistDscReactorServiceContainer(NULL, CFineFoodService::EN_SERVICE_TYPE, 1);

	return 0;
} 

extern "C" PLUGIN_EXPORT void* CreateDscPlugin(void)
{
	CFineFoodServicePlugin* pPlugIn = NULL;
	
	DSC_NEW(pPlugIn, CFineFoodServicePlugin);

	return pPlugIn;
}

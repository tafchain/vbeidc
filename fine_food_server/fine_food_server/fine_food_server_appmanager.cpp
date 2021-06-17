#include "dsc/dsc_comm_def.h"

#include "fine_food_server/fine_food_server_appmanager.h"

ACE_INT32 CDemoServerAppManager::OnInit()
{
	DSC_FORWARD_CALL(CDscAppManager::OnInit() );

	return 0;
}

ACE_INT32 CDemoServerAppManager::OnExit()
{
	DSC_FORWARD_CALL(CDscAppManager::OnExit());

	return 0;
}



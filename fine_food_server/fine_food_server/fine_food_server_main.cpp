#include "ace/OS_main.h"
#include "ace/OS_NS_stdio.h"

#include <stack>
#include <functional>

#include "dsc/mem_mng/dsc_allocator.h"
#include "dsc/dsc_log.h"

#include "fine_food_server_appmanager.h"
#include "vbe_idc/vbe_idc_reg_server_app/vbe_idc_reg_appmanager.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	CDemoServerAppManager* pAppManager = new CDemoServerAppManager;
	if (!pAppManager)
	{
		ACE_OS::printf("failed to new fine food Server App Manager!");

		return -1;
	}

	pAppManager->SetNodeType(VBE_IDC::EN_VBE_FINE_FOOD_SERVICE_APP_TYPE);
	if (pAppManager->Init(argc, argv))
	{
		ACE_OS::printf("demo server init failed, now exit!\n");
		pAppManager->Exit();
		delete pAppManager;

		return -1;
	}

	ACE_OS::printf("fine food server init succeed, running...\n");
	pAppManager->Run_Loop();
	delete pAppManager;
	ACE_OS::printf("demo server terminated!\n");

	return 0;
}
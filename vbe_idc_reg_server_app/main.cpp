#include "ace/OS_main.h"
#include "ace/OS_NS_stdio.h"

#include "vbe_idc/vbe_idc_reg_server_app/vbe_idc_reg_appmanager.h"
#include "vbe_idc/vbe_idc_common/vbe_idc_common_def.h"

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
	CVbeIdcAppManager* pVbeAppManager = ::new(std::nothrow) CVbeIdcAppManager;
	if (!pVbeAppManager)
	{
		ACE_OS::printf("failed to new VbeAppManager!");

		return -1;
	}

	pVbeAppManager->SetNodeType(VBE_IDC::EN_VBE_REG_SERVICE_APP_TYPE);
	if (pVbeAppManager->Init(argc, argv))
	{
		ACE_OS::printf("VbeAppManager init failed, now exit!\n");
		pVbeAppManager->Exit();
		delete pVbeAppManager;

		return -1;
	}

	ACE_OS::printf("VbeAppManager init succeed, running...\n");
	pVbeAppManager->Run_Loop();
	delete pVbeAppManager;
	ACE_OS::printf("VbeAppManager terminated!\n");

	return 0;
}
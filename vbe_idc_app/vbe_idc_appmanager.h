#ifndef _VBE_IDC_APP_MANAGER_H_340572327256
#define _VBE_IDC_APP_MANAGER_H_340572327256

#include "dsc/dsc_app_mng.h"

class CVbeIdcAppManager : public CDscAppManager
{

protected:
	virtual ACE_INT32 OnInit(void);
	virtual ACE_INT32 OnExit(void);
};

#endif


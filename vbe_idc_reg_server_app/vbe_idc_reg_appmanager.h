#ifndef _VBE_IDC_REG_SERVER_APP_MANAGER_H_289479492345234
#define _VBE_IDC_REG_SERVER_APP_MANAGER_H_289479492345234

#include "dsc/dsc_app_mng.h"

class CVbeIdcAppManager : public CDscAppManager
{

protected:
	virtual ACE_INT32 OnInit(void);
	virtual ACE_INT32 OnExit(void);
};

#endif


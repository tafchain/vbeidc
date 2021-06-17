#ifndef FINE_FOOD_SERVER_APPMANAGER_H_23467234682345623542312317892356823423237
#define FINE_FOOD_SERVER_APPMANAGER_H_23467234682345623542312317892356823423237

#include "dsc/dsc_app_mng.h"

class CDemoServerAppManager : public CDscAppManager
{

protected:
	virtual ACE_INT32 OnInit(void);
	virtual ACE_INT32 OnExit(void);
};

#endif

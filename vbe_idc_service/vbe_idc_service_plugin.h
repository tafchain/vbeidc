#ifndef _VBE_IDC_SERVICE_PLUGIN_H_028460275657
#define _VBE_IDC_SERVICE_PLUGIN_H_028460275657

#include "vbe_idc/vbe_idc_service/vbe_idc_service.h"

#include "dsc/plugin/i_dsc_plugin.h"
#include "dsc/service/dsc_service_container.h"

class CVbeIdcServiceFactory : public IDscServiceFactory
{
public:
	virtual CDscService* CreateDscService(void);

public:

	CVbeIdcService* m_vbeService;

};

class CSdkClientServicePlugIn : public IDscPlugin
{
public:
	ACE_INT32 OnInit(void);
};

#endif


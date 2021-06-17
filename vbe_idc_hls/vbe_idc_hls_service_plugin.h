#ifndef _VBE_IDC_HLS_SERVICE_PLUGIN_H_81075860813745916
#define _VBE_IDC_HLS_SERVICE_PLUGIN_H_81075860813745916

#include "dsc/plugin/i_dsc_plugin.h"

class CVbeIdcHlsServicePlugin : public IDscPlugin
{
public:
	CVbeIdcHlsServicePlugin();

public:
	ACE_INT32 OnInit(void);

};


#endif

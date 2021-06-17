#ifndef Cfine_food_SERVICE_SERVICE_PLUGIN_H_432788432791657865
#define Cfine_food_SERVICE_SERVICE_PLUGIN_H_432788432791657865

#include "dsc/plugin/i_dsc_plugin.h"

class CFineFoodServicePlugin : public IDscPlugin
{ 
public: 
	ACE_INT32 OnInit(void); 
	
}; 
#endif
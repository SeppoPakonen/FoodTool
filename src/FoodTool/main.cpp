#include "FoodTool.h"


GUI_APP_MAIN
{
	if (!GetProfile().is_initialised) {
		ProfileCreator pc;
		pc.Run();
	}
	if (!GetProfile().is_initialised)
		return;
	
	RealizeImageFolder();
	FoodTool().Run();
}

#include "FoodTool.h"


GUI_APP_MAIN
{
	for(int i = 0; i < 1000; i++) Random(1000);
	
	SetLanguage(GetSystemLNG());
	
	Database& db = GetDatabase();
	if (!db.is_init && !db.Init()) {
		PromptOK("ERROR: Expected \"USDA National Nutrient Database for Standard Reference, Release 28\" in the db folder");
		SetExitCode(1);
		return;
	}
	if (db.used_foods.IsEmpty())
		db.VLCD_Preset();
	db.RemoveDuplicates();
	db.SetCommonNutrs();
	
	UnpackSounds();
	
	if (!GetProfile().is_initialised) {
		ProfileCreator pc;
		pc.Run();
	}
	if (!GetProfile().is_initialised)
		return;
	
	RealizeImageFolder();
	FoodTool().Run();
}

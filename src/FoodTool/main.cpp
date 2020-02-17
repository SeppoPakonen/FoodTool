#include "FoodTool.h"
#include "Data.brc"
#include <plugin/bz2/bz2.h>


GUI_APP_MAIN
{
	SetLanguage(GetSystemLNG());
	
	Database& db = GetDatabase();
	if (!db.is_init && !db.Init()) {
		PromptOK("ERROR: Expected \"USDA National Nutrient Database for Standard Reference, Release 28\" in the db folder");
		SetExitCode(1);
		return;
	}
	db.VLCD_Preset();
	db.SetCommonNutrs();
	
	
	String camera_shutter_file = ConfigFile("camera-shutter.wav");
	if (!FileExists(camera_shutter_file)) {
		String camera_shutter_wav = BZ2Decompress(camera_shutter, camera_shutter_length);
		FileOut fout(camera_shutter_file);
		fout << camera_shutter_wav;
	}
	
	String alert_file = ConfigFile("alert.wav");
	if (!FileExists(alert_file)) {
		String alert_wav = BZ2Decompress(alert, alert_length);
		FileOut fout(alert_file);
		fout << alert_wav;
	}
	
	if (!GetProfile().is_initialised) {
		ProfileCreator pc;
		pc.Run();
	}
	if (!GetProfile().is_initialised)
		return;
	
	RealizeImageFolder();
	FoodTool().Run();
}

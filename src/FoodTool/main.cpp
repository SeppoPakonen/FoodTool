#include "FoodTool.h"
#include "Data.brc"
#include <plugin/bz2/bz2.h>


GUI_APP_MAIN
{
	String camera_shutter_file = ConfigFile("camera-shutter.wav");
	if (!FileExists(camera_shutter_file)) {
		String camera_shutter_wav = BZ2Decompress(camera_shutter, camera_shutter_length);
		FileOut fout(camera_shutter_file);
		fout << camera_shutter_wav;
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

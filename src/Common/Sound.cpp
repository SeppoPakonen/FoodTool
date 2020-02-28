#include "Common.h"
#include "data.brc"

void UnpackSounds() {
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
	
}

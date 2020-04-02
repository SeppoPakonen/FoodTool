#ifndef _Common_Sound_h_
#define _Common_Sound_h_


#ifdef flagWIN32
inline void PlayCameraShutter() {PlaySoundA(ConfigFile("camera-shutter.wav"), NULL, SND_ASYNC|SND_FILENAME);}
inline void PlayAlert() {PlaySoundA(ConfigFile("alert.wav"), NULL, SND_ASYNC|SND_FILENAME);}
#else
inline void PlayCameraShutter() {String cmd = "play \"" + ConfigFile("camera-shutter.wav"), out; Sys(cmd, out);}
inline void PlayAlert() {String cmd = "play \"" + ConfigFile("alert.wav"), out; Sys(cmd, out);}
#endif

void UnpackSounds();


#endif

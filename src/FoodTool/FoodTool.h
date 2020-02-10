#ifndef _FoodTool_FoodTool_h
#define _FoodTool_FoodTool_h

#define int64 int64_fuck_opencv
#define uint64 uint64_fuck_opencv
#undef CPU_SSE2
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#undef int64
#undef uint64

#include <random>
#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>
#include <plugin/jpg/jpg.h>

using namespace Upp;

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_header.h>

#include "Optimizer.h"
#include "Food.h"
#include "Profile.h"
#include "VideoCapture.h"
#include "ScheduleCtrl.h"

#define LAYOUTFILE <FoodTool/FoodTool.lay>
#include <CtrlCore/lay.h>

#include "ProfileCreator.h"

#ifdef flagWIN32
inline void PlayCameraShutter() {PlaySoundA(ConfigFile("camera-shutter.wav"), NULL, SND_ASYNC|SND_FILENAME);}
#else
inline void PlayCameraShutter() {}
#endif

Color Rainbow(float progress);

struct MotivationCtrl : public Ctrl {
	int motivation_i = -1, quote_i = -1;
	Image img;
	Vector<String> quotes;
	
	typedef MotivationCtrl CLASSNAME;
	MotivationCtrl();
	
	void Data() {Refresh();}
	void Paint(Draw& d);
};

struct StatusCtrl : public WithStatusLayout<ParentCtrl> {
	Date cur_date;
	
	typedef StatusCtrl CLASSNAME;
	StatusCtrl();
	
	void Data();
};

struct ConfigurationCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithConfigurationLayout<ParentCtrl> conf;
	FatPercentageReferenceWindow iw;
	
	typedef ConfigurationCtrl CLASSNAME;
	ConfigurationCtrl();
	
	void Data();
	void SelectConf();
	void AddConf();
	void UpdateTargetWeight();
	void ShowWeightReference();
};

struct ExceptionsCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithExceptionLayout<ParentCtrl> exc;
	
	typedef ExceptionsCtrl CLASSNAME;
	ExceptionsCtrl();
	
	void Data(bool force=false);
	void AddException();
	
};

struct NoteCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithNoteLayout<ParentCtrl> note;
	
	
	typedef NoteCtrl CLASSNAME;
	NoteCtrl();
	
	void Data();
	void AddNote();
	void SelectNote();
};

struct UsageCtrl : public ParentCtrl {
	ArrayCtrl list;
	Label total;
	
	typedef UsageCtrl CLASSNAME;
	UsageCtrl();
	
	void Data();
};

void SetScaledImageCtrl(ImageCtrl& ic, Image i);

struct WeightCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithWeightLayout<ParentCtrl> edit;
	
	Image front, right, back;
	VideoCapture cap;
	Image last_cap;
	Mutex cap_lock, live_lock;
	TimeStop ts;
	int capture_mode = 0, countdown_seconds = 0;
	bool running = false, stopped = true;
	
	// Persistent
	int last_camera_i = 0, last_camera_count = 0;
	
	enum {
		COUNTDOWN_FRONT, FRONT,
		COUNTDOWN_RIGHT, RIGHT,
		COUNTDOWN_BACK, BACK,
		MODE_COUNT
	};
	
	typedef WeightCtrl CLASSNAME;
	WeightCtrl();
	~WeightCtrl() {Stop();}
	
	void Data();
	void SelectWeightStat();
	void Reset();
	void UpdateBMI();
	
	
	void Serialize(Stream& s) {s % last_camera_i % last_camera_count;}
	void StoreThis() {StoreToFile(*this, ConfigFile("selected_camera.bin"));}
	void LoadThis() {LoadFromFile(*this, ConfigFile("selected_camera.bin"));}
	
	void UpdateCountdown() {PostCallback(THISBACK1(SetInstruction, IntStr(countdown_seconds)));}
	void SetCountdown(int seconds);
	void SetInstruction(String s) {edit.instruction.SetLabel(s);}
	void Stop() {running = false; Wait();}
	void Wait() {while (!stopped) Sleep(100);}
	void AddWeightStat();
	void PreviewCamera();
	void CaptureImages();
	void SetCamImage();
	void OpenCamera(int i);
	void CloseCamera();
	void ProcessCapture(bool preview, int cam);
	void SetFrontImage() {SetScaledImageCtrl(edit.front, front);}
	void SetRightImage() {SetScaledImageCtrl(edit.right, right);}
	void SetBackImage() {SetScaledImageCtrl(edit.back, back);}
	void LoadImages(String f, String r, String b);
};

struct MultipurposeGraph : public Ctrl {
	struct Line : Moveable<Line> {
		String title;
		Color color;
		int width;
	};
	struct Src : Moveable<Src> {
		Vector<Line> lines;
		int vert_line = 0;
		
		Src& Add(String t, int w, Color c) {Line& l = lines.Add(); l.title = t; l.width = w; l.color = c; return *this;}
		Src& Vert(int y) {vert_line = y; return *this;}
	};
	
private:
	Vector<Vector<double>> tmp;
	Vector<Point> polyline;
	Vector<Src> src;
	
public:
	int new_src = 0;
	
	Src& Add() {return src.Add();}
	int GetLineCount(int src);
	Color GetLineColor(int src, int l);
	int GetLineWidth(int src, int l);
	String GetLineTitle(int src, int l);
	int GetCount(int src);
	int GetVertLine(int src);
	int GetHorzLine(int src);
	const Vector<double>& GetValue(int src, int l);
	void Paint(Draw& d);
};

struct GraphCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	MultipurposeGraph graph;
	
	
	typedef GraphCtrl CLASSNAME;
	GraphCtrl();
	
	void Data() {graph.Refresh();}
	void SelectSource();
};

class FoodTool : public WithFoodToolLayout<TopWindow> {
	TabCtrl tabs;
	
	MotivationCtrl motivation;
	StatusCtrl status;
	ConfigurationCtrl conf;
	ExceptionsCtrl exc;
	NoteCtrl notes;
	UsageCtrl usage;
	WeightCtrl weight;
	GraphCtrl graphs;
	
	TimeCallback tc;
public:
	typedef FoodTool CLASSNAME;
	FoodTool();
	
	void Data();
};

#endif

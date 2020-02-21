#ifndef _FoodTool_WeightCtrl_h_
#define _FoodTool_WeightCtrl_h_


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
	void UpdateCameraCount();
	void UpdateCameraList();
	void UpdateSmiley();
	void WeightChanged() {UpdateBMI(); UpdateSmiley();}
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % last_camera_i % last_camera_count;}
	}
	void StoreThis() {StoreToFile(*this, ConfigFile("selected_camera.bin"));}
	void LoadThis() {LoadFromFile(*this, ConfigFile("selected_camera.bin"));}
	
	void UpdateCountdown() {PostCallback(THISBACK1(SetInstruction, IntStr(countdown_seconds)));}
	void SetCountdown(int seconds);
	void SetInstruction(String s) {edit.instruction.SetLabel(s);}
	void Stop() {running = false; Wait();}
	void Wait() {while (!stopped) Sleep(100);}
	void SaveWeightStat();
	void AddWeightStat();
	void SetWeightStat(WeightLossStat& w);
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


#endif

#ifndef _FoodTool_VideoCapture_h_
#define _FoodTool_VideoCapture_h_

class VideoCapture {
	
	cv::VideoCapture cap;
	cv::Mat buffer;
	int id;
	Size target_sz;
	int frame_counter;
	String export_path;
	
public:
	VideoCapture();
	~VideoCapture() {Clear();}
	
	void Clear();
	
	static int GetCount();
	bool Open(int i) {id = i; cap.open(i); return cap.isOpened();}
	void Close() {cap.release();}
	void OpenSize(int i, Size sz) {id = i; SetSize(sz);}
	void Open4_3_960p(int i) {OpenSize(i, Size(1280,960));}
	
	Size GetSize() {return Size((int)cap.get(cv::CAP_PROP_FRAME_WIDTH), (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT));}
	int GetId() const {return id;}
	
	void SetSize(Size sz);
	VideoCapture& SetExportDirectory(String path);
	
	virtual void Process();
	Image GetImage() const;
};

#endif

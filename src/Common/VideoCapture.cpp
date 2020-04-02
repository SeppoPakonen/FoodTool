#include "Common.h"

#ifdef flagLINUX
	#define OPENCV_BYPASS
#endif


VideoCapture::VideoCapture() {
	id = -1;
	target_sz = Size(0,0);
	
	/*String dir = ConfigFile("Captured");
	RealizeDirectory(dir);
	SetExportDirectory(dir);*/
}

void VideoCapture::Clear() {
	cap.release();
	buffer.release();
}

VideoCapture& VideoCapture::SetExportDirectory(String path) {
	frame_counter = 0;
	export_path = path;
	return *this;
}

void VideoCapture::SetSize(Size sz) {
	if (id == -1) return;
	#ifdef OPENCV_BYPASS
	
	#else
	if (cap.isOpened())
		cap.release();
	//cap.set(cv::CAP_PROP_FRAME_WIDTH,	sz.cx);
	//cap.set(cv::CAP_PROP_FRAME_HEIGHT,	sz.cy);
	cap.open(id);
	cap.set(cv::CAP_PROP_FRAME_WIDTH,	sz.cx);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT,	sz.cy);
	//cap.set(cv::CAP_PROP_FPS,			25);
	//cap.set(cv::CAP_PROP_CONVERT_RGB,	true);
	//cap.set(CV_CAP_PROP_FOURCC,			CV_FOURCC('M', 'J', 'P', 'G') );
	#endif
	target_sz = sz;
}

int VideoCapture::GetCount() {
	#ifdef OPENCV_BYPASS
	return 1;
	#else
	int max_tested = 10;
	for (int i = 0; i < max_tested; i++){
		cv::VideoCapture temp_camera(i);
		bool res = (!temp_camera.isOpened());
		temp_camera.release();
		if (res)
			return i;
	}
	return max_tested;
	#endif
}

void VideoCapture::Process(int ms) {
	#ifdef OPENCV_BYPASS
	String cmd = "fswebcam -r 1280x960 --jpeg 95 -D 1 tmp.jpg";
	String out;
	Sys(cmd, out);
	#else
	buffer.release();
	cap.read(buffer);
	if (buffer.rows == 0)
		return;
	/*else if ((target_sz.cx != 0 && buffer.cols != target_sz.cx) ||
		(target_sz.cy != 0 && buffer.rows != target_sz.cy)) {
		cv::resize(buffer, buffer, cv::Size(target_sz.cx, target_sz.cy), 0, 0, cv::INTER_NEAREST);
	}*/
	
	
	if (!export_path.IsEmpty()) {
		String file = export_path + DIR_SEPS + Format("%05d.png", frame_counter);
		imwrite(file.Begin(), buffer);
	}
	
	frame_counter++;
	#endif
}

Image VideoCapture::GetImage(Rect area) const {
	#ifdef OPENCV_BYPASS
	return StreamRaster::LoadFileAny("tmp.jpg");
	#else
	Size sz(buffer.cols, buffer.rows);
	if (sz.cx == 0 || sz.cy == 0)
		return Image();
	int ch = buffer.channels();
	ASSERT(ch == 3);
	
	ImageBuffer ib(sz);
	RGBA* it = ib.Begin();
	auto end = buffer.end<cv::Vec3b>();
	for (auto color = buffer.begin<cv::Vec3b>(); color != end; color++) {
		it->r = (*color)[2];
		it->g = (*color)[1];
		it->b = (*color)[0];
		it->a = 255;
		it++;
	}
	return ib;
	#endif
}


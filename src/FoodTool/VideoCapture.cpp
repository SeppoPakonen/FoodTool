#include "FoodTool.h"


VideoCapture::VideoCapture() {
	id = -1;
	target_sz = Size(0,0);
}

VideoCapture& VideoCapture::SetExportDirectory(String path) {
	frame_counter = 0;
	export_path = path;
	return *this;
}

void VideoCapture::SetSize(Size sz) {
	if (id == -1) return;
	if (cap.isOpened())
		cap.release();
	cap.set(cv::CAP_PROP_FRAME_WIDTH,	sz.cx);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT,	sz.cy);
	cap.open(id);
	cap.set(cv::CAP_PROP_FRAME_WIDTH,	sz.cx);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT,	sz.cy);
	target_sz = sz;
}

int VideoCapture::GetCount() {
	int max_tested = 10;
	for (int i = 0; i < max_tested; i++){
		cv::VideoCapture temp_camera(i);
		bool res = (!temp_camera.isOpened());
		temp_camera.release();
		if (res)
			return i;
	}
	return max_tested;
}

void VideoCapture::Process(int ms) {
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
}

Image VideoCapture::GetImage(Rect area) const {
	Size sz(buffer.cols, buffer.rows);
	if (sz.cx == 0 || sz.cy == 0)
		return Image();
	
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
	/*for(int y = 0; y < sz.cy; y++) {
		for(int x = 0; x < sz.cx; x++) {
			cv::Vec3b color = buffer.at<cv::Vec3b>(cv::Point(x, y));
			it->r = color[2];
			it->g = color[1];
			it->b = color[0];
			it->a = 255;
			it++;
		}
	}*/
	
	return ib;
	
	/*Rect src = RectC(0, 0, buffer.cols, buffer.rows);
	if (!src.Contains(area))
		return Image();
	
	Size sz = area.GetSize();
	ImageBuffer ib(sz);
	RGBA* it = ib.Begin();
	for(int y = 0; y < sz.cy; y++) {
		for(int x = 0; x < sz.cx; x++) {
			cv::Vec3b color = buffer.at<cv::Vec3b>(cv::Point(area.left + x, area.top + y));
			it->r = color[2];
			it->g = color[1];
			it->b = color[0];
			it->a = 255;
			it++;
		}
	}
	
	return ib;*/
}


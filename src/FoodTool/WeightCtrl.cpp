#include "FoodTool.h"


WeightCtrl::WeightCtrl() {
	LoadThis();
	
	Add(split.SizePos());
	split.Horz() << list << edit;
	split.SetPos(2500);
	
	list.AddColumn(t_("Date"));
	list.AddColumn(t_("Weight (kg)"));
	list.ColumnWidths("2 1");
	list <<= THISBACK(SelectWeightStat);
	list.WhenLeftClick << THISBACK(SelectWeightStat);
	
	CtrlLayout(edit);
	edit.weight <<= THISBACK(WeightChanged);
	
	Thread::Start(THISBACK(UpdateCameraCount));
	
	edit.sentiment.Add("Worst");
	edit.sentiment.Add("Bad");
	edit.sentiment.Add("Normal");
	edit.sentiment.Add("Good");
	edit.sentiment.Add("Best");
	
	edit.health.Add("Worst");
	edit.health.Add("Bad");
	edit.health.Add("Normal");
	edit.health.Add("Good");
	edit.health.Add("Best");
	
	edit.workload.Add("Lowest");
	edit.workload.Add("Low");
	edit.workload.Add("Normal");
	edit.workload.Add("High");
	edit.workload.Add("Highest");
	
	edit.capture_images <<= THISBACK(CaptureImages);
	edit.preview_cam <<= THISBACK(PreviewCamera);
	edit.save <<= THISBACK(SaveWeightStat);
	edit.add <<= THISBACK(AddWeightStat);
	edit.reset <<= THISBACK(Reset);
}

void WeightCtrl::UpdateCameraCount() {
	int count = cap.GetCount();
	if (last_camera_count != count) {
		last_camera_i = 0;
		last_camera_count = count;
	}
	PostCallback(THISBACK(UpdateCameraList));
}

void WeightCtrl::UpdateCameraList() {
	for(int i = 0; i < last_camera_count; i++)
		edit.cameras.Add(t_("Source #") + IntStr(i));
	
	if (edit.cameras.GetCount())
		edit.cameras.SetIndex(last_camera_i);
}

void WeightCtrl::Reset() {
	edit.weight.Clear();
	edit.fat.Clear();
	edit.liquid.Clear();
	edit.muscle.Clear();
	edit.bmi.Clear();
	edit.sentiment.SetIndex(2);
	edit.health.SetIndex(2);
	edit.workload.SetIndex(2);
	edit.walking.Clear();
	edit.excess.Clear();
	front.Clear();
	right.Clear();
	back.Clear();
	edit.front.SetImage(Image());
	edit.right.SetImage(Image());
	edit.back.SetImage(Image());
	edit.preview_cam.Enable();
	edit.capture_images.Enable();
	edit.add.Enable();
}

void WeightCtrl::UpdateBMI() {
	double weight = edit.weight.GetData();
	if (!IsFin(weight)) return;
	edit.bmi.SetData(GetBMI(GetProfile().confs.Top().height, weight));
}

void WeightCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.weights.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.weights.GetCount(); i++) {
			int row = prof.weights.GetCount() - 1 - i;
			const WeightLossStat& w = prof.weights[i];
			list.Set(row, 0, Date(w.added));
			list.Set(row, 1, w.weight);
		}
	}
}

void WeightCtrl::UpdateSmiley() {
	Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	double tgt_weight = GetTargetWeight(conf.height);
	double prog = 1.0 - ((double)edit.weight.GetData() - tgt_weight) / (prof.weights[0].weight - tgt_weight);
	edit.smiley.SetImage(GetSmiley(prog));
}

void WeightCtrl::SelectWeightStat() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const WeightLossStat& w = prof.weights[note_i];
	
	edit.weight.SetData(w.weight);
	edit.fat.SetData(w.fat);
	edit.liquid.SetData(w.liquid);
	edit.muscle.SetData(w.muscle);
	edit.bmi.SetData(w.bmi);
	edit.dexa.Set(w.is_dexa);
	edit.sentiment.SetIndex(w.sentiment);
	edit.health.SetIndex(w.health);
	edit.workload.SetIndex(w.workload);
	edit.walking.SetData(w.walking);
	edit.walking_mins.SetData(w.walking_mins);
	edit.excess.SetData(w.excess);
	edit.neck.SetData(w.neck);
	edit.bicep.SetData(w.bicep);
	edit.forearm.SetData(w.forearm);
	edit.chest.SetData(w.chest);
	edit.waist.SetData(w.waist);
	edit.buttocks.SetData(w.buttocks);
	edit.thigh.SetData(w.thigh);
	edit.leg.SetData(w.leg);
	edit.smiley.SetImage(GetSmiley(w.prog));
	
	Thread::Start(THISBACK3(LoadImages, w.GetFrontFile(),w.GetRightFile(), w.GetBackFile()));
	
	edit.preview_cam.Disable();
	edit.capture_images.Disable();
	edit.add.Disable();
}

void WeightCtrl::LoadImages(String f, String r, String b) {
	front = StreamRaster::LoadFileAny(f);
	PostCallback(THISBACK(SetFrontImage));
	
	right = StreamRaster::LoadFileAny(r);
	PostCallback(THISBACK(SetRightImage));
	
	back = StreamRaster::LoadFileAny(b);
	PostCallback(THISBACK(SetBackImage));
}

void WeightCtrl::AddWeightStat() {
	Profile& prof = GetProfile();
	WeightLossStat& w = prof.weights.Add();
	w.added = GetSysTime();
	SetWeightStat(w);
}

void WeightCtrl::SetWeightStat(WeightLossStat& w) {
	Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	
	w.weight = (double)edit.weight.GetData();
	w.fat = (double)edit.fat.GetData();
	w.liquid = (double)edit.liquid.GetData();
	w.muscle = (double)edit.muscle.GetData();
	w.bmi = (double)edit.bmi.GetData();
	w.is_dexa = edit.dexa.Get();
	w.sentiment = edit.sentiment.GetIndex();
	w.health = edit.health.GetIndex();
	w.workload = edit.workload.GetIndex();
	w.walking = (double)edit.walking.GetData();
	w.walking_mins = (double)edit.walking_mins.GetData();
	w.excess = (double)edit.excess.GetData();
	w.neck = (double)edit.neck.GetData();
	w.bicep = (double)edit.bicep.GetData();
	w.forearm = (double)edit.forearm.GetData();
	w.chest = (double)edit.chest.GetData();
	w.waist = (double)edit.waist.GetData();
	w.buttocks = (double)edit.buttocks.GetData();
	w.thigh = (double)edit.thigh.GetData();
	w.leg = (double)edit.leg.GetData();
	double tgt_weight = GetTargetWeight(conf.height);
	w.prog = 1.0 - fabs(w.weight - tgt_weight) / fabs(prof.weights[0].weight - tgt_weight);
	
	if (!front.IsEmpty())
		JPGEncoder().SaveFile(w.GetFrontFile(), front);
	if (!right.IsEmpty())
		JPGEncoder().SaveFile(w.GetRightFile(), right);
	if (!back.IsEmpty())
		JPGEncoder().SaveFile(w.GetBackFile(), back);
	
	prof.StoreThis();
	StoreThis();
	
	Data();
}

void WeightCtrl::SaveWeightStat() {
	if (!list.IsCursor())
		return;
	int i = list.GetCursor();
	int w_i = list.GetCount() - 1 - i;
	Profile& prof = GetProfile();
	WeightLossStat& w = prof.weights[w_i];
	SetWeightStat(w);
}

void WeightCtrl::PreviewCamera() {
	if (!stopped)
		return;
	
	running = true; stopped = false;
	Thread::Start(THISBACK2(ProcessCapture, true, edit.cameras.GetIndex()));
}

void WeightCtrl::CaptureImages() {
	if (!stopped) {
		running = false;
		return;
	}
	
	running = true; stopped = false;
	Thread::Start(THISBACK2(ProcessCapture, false, edit.cameras.GetIndex()));
}

void WeightCtrl::OpenCamera(int i) {
	cap_lock.Enter();
	cap.OpenSize(i, Size(1920,1080));
	cap_lock.Leave();
	last_camera_i = i;
	StoreThis();
}

void WeightCtrl::CloseCamera() {
	cap_lock.Enter();
	cap.Close();
	cap_lock.Leave();
}

void SetScaledImageCtrl(ImageCtrl& ic, Image i) {
	Size sz = ic.GetSize();
	if (i.IsEmpty()) {
		ic.SetImage(Image());
		return;
	}
	Size img_sz = i.GetSize();
	double hratio = (double)sz.cx / img_sz.cx;
	double vratio = (double)sz.cy / img_sz.cy;
	double ratio = min(hratio, vratio);
	img_sz.cx *= ratio;
	img_sz.cy *= ratio;
	Image img = CachedRescale(i, img_sz);
	ic.SetImage(img);
}

void WeightCtrl::SetCamImage() {
	live_lock.Enter();
	Image i = last_cap;
	live_lock.Leave();
	SetScaledImageCtrl(edit.live, i);
}

void WeightCtrl::ProcessCapture(bool preview, int cam) {
	OpenCamera(cam);
	
	if (preview) {
		cap_lock.Enter();
		cap.Process(100);
		Image img = cap.GetImage(Size(1920,1080));
		
		live_lock.Enter();
		last_cap = img;
		live_lock.Leave();
		
		PostCallback(THISBACK(SetCamImage));
		cap_lock.Leave();
	}
	else {
		capture_mode = COUNTDOWN_FRONT;
		SetCountdown(5);
		while (running) {
			
			cap_lock.Enter();
			cap.Process(100);
			Image img = cap.GetImage(Size(1920,1080));
			
			live_lock.Enter();
			last_cap = img;
			live_lock.Leave();
			
			PostCallback(THISBACK(SetCamImage));
			cap_lock.Leave();
			
			if (capture_mode == COUNTDOWN_FRONT ||
				capture_mode == COUNTDOWN_RIGHT ||
				capture_mode == COUNTDOWN_BACK) {
				if (ts.Elapsed() >= 1000) {
					SetCountdown(countdown_seconds-1);
					if (!countdown_seconds)
						capture_mode++;
				}
			}
			else {
				PlayCameraShutter();
				
				if (capture_mode == FRONT) {
					front = img;
					PostCallback(THISBACK(SetFrontImage));
				}
				else if (capture_mode == RIGHT) {
					right = img;
					PostCallback(THISBACK(SetRightImage));
				}
				else if (capture_mode == BACK) {
					back = img;
					PostCallback(THISBACK(SetBackImage));
				}
				
				capture_mode++;
				SetCountdown(3);
				if (capture_mode >= MODE_COUNT)
					break;
			}
			
			Sleep(50);
		}
		
		live_lock.Enter();
		last_cap.Clear();
		live_lock.Leave();
		PostCallback(THISBACK(SetCamImage));
	}
	
	PostCallback(THISBACK1(SetInstruction, ""));
	
	
	CloseCamera();
	running = false;
	stopped = true;
}

void WeightCtrl::SetCountdown(int seconds) {
	countdown_seconds = seconds;
	UpdateCountdown();
	ts.Reset();
}

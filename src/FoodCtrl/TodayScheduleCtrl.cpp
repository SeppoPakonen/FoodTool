#include "FoodCtrl.h"


TodayScheduleCtrl::TodayScheduleCtrl() {
	Profile& prof = GetProfile();
	prof.MakeTodaySchedule(sched);
	prof_version = prof.version;
	
	waking_top = Color(255, 254, 0);
	waking_btm = Color(255, 255, 200);
	eating_top = Color(255, 165, 133);
	eating_btm = Color(255, 195, 163);
	walking_top = Color(187, 255, 184);
	walking_btm = Color(241, 255, 160);
	muscletraining_top = Color(106, 31, 0);
	muscletraining_btm = Color(198, 42, 0);
	sleeping_top = Color(156, 158, 193);
	sleeping_btm = Color(180, 196, 229);
	
	tc.Set(-1000, THISBACK(CheckAlerts));
}

void TodayScheduleCtrl::CheckAlerts() {
	String alert_str;
	
	Time now = GetSysTime();
	for(auto& it : sched.items) {
		if (it.time <= now && !it.done) {
			if (alert_str.IsEmpty()) {
				alert_str = it.msg;
			}
			it.done = true;
		}
	}
	
	if (alert_str.GetCount())
		Alert(alert_str);
}

void TodayScheduleCtrl::Alert(String alert_str) {
	GetTopWindow()->Restore();
	GetTopWindow()->Maximize();
	PlayAlert();
	PromptOK(alert_str);
	WhenAlert();
}

void TodayScheduleCtrl::Paint(Draw& d) {
	Profile& prof = GetProfile();
	Date today = GetSysTime();
	if (prof.version > prof_version || sched.items.IsEmpty() || sched.day != today) {
		prof.MakeTodaySchedule(sched);
		prof_version = prof.version;
	}
	
	USize sz(GetSize());
	d.DrawRect(sz, White());
	
	if (sched.items.IsEmpty())
		return;
	
	double ystep = min(60.0, (double)sz.cy / sched.items.GetCount());
	Font fnt = SansSerif(ystep * 0.8);
	
	for(int i = 0; i < sched.items.GetCount(); i++) {
		const auto& it = sched.items[i];
		int y = i * ystep;
		int h = (i + 1) * ystep - y;
		
		Color top, btm;
		String typestr;
		if (it.type == ScheduleToday::WAKING) {
			typestr = t_("Waking");
			top = waking_top;
			btm = waking_btm;
		}
		else if (it.type == ScheduleToday::EATING) {
			typestr = t_("Eating");
			top = eating_top;
			btm = eating_btm;
		}
		else if (it.type == ScheduleToday::WALKING) {
			typestr = t_("Walking");
			top = walking_top;
			btm = walking_btm;
		}
		else if (it.type == ScheduleToday::RUNNING) {
			typestr = t_("Running");
			top = running_top;
			btm = running_btm;
		}
		else if (it.type == ScheduleToday::SLEEPING) {
			typestr = t_("Sleeping");
			top = sleeping_top;
			btm = sleeping_btm;
		}
		else if (it.type == ScheduleToday::MUSCLETRAINING) {
			typestr = t_("Training");
			top = muscletraining_top;
			btm = muscletraining_btm;
		}
		String txt;
		txt << Format("%d:%02d ", (int)it.time.hour, (int)it.time.minute)
			<< typestr << ": " << it.msg;
			
		DrawGradientVert(d, URectC(0, y, sz.cx, h), top, btm);
		d.DrawText(5+2, y+2, txt, fnt, White());
		d.DrawText(5, y, txt, fnt, Black());
	}
	
}

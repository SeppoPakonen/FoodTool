#ifndef _FoodTool_TodayScheduleCtrl_h_
#define _FoodTool_TodayScheduleCtrl_h_

struct TodayScheduleCtrl : public Ctrl {
	ScheduleToday sched;
	int prof_version;
	Color waking_top, waking_btm;
	Color eating_top, eating_btm;
	Color walking_top, walking_btm;
	Color running_top, running_btm;
	Color sleeping_top, sleeping_btm;
	Color muscletraining_top, muscletraining_btm;
	TimeCallback tc;
	
	
	typedef TodayScheduleCtrl CLASSNAME;
	TodayScheduleCtrl();
	void Paint(Draw& d);
	void Data() {Refresh();}
	void CheckAlerts();
	void Alert(String alert_str);
	
	Callback WhenAlert;
};

#endif

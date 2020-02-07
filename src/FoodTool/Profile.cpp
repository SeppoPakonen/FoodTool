#include "FoodTool.h"

ProfileCreator::ProfileCreator() {
	CtrlLayout(*this, "Profile Creator");
	
	calc1 <<= THISBACK(OpenCalculator1);
	calc2 <<= THISBACK(OpenCalculator2);
	begin <<= THISBACK(Begin);
	
	keto_cals.SetData(1314);
	keto_fats.SetData(93);
	keto_carbs.SetData(20);
	keto_prots.SetData(100);
	
	wl_start.SetData(100);
	wl_loss.SetData(40);
	wl_months.SetData(6);
	wl_cals.SetData(1068);
	
	shop_interval.SetData(5);
}

void ProfileCreator::Begin() {
	Profile& prof = GetProfile();
	
	prof.keto_cals = keto_cals.GetData();
	prof.keto_fats = keto_fats.GetData();
	prof.keto_carbs = keto_carbs.GetData();
	prof.keto_prots = keto_prots.GetData();
	
	prof.wl_start = wl_start.GetData();
	prof.wl_loss = wl_loss.GetData();
	prof.wl_months = wl_months.GetData();
	prof.wl_cals = wl_cals.GetData();
	
	prof.storage.shop_interval = shop_interval.GetData();
	
	prof.begin_time = GetSysTime();
	prof.end_time = prof.begin_time + ((int64)prof.wl_months * (int64)31556926 / (int64)12);
	
	LOG("Started " << Format("%", prof.begin_time));
	LOG("Stopping " << Format("%", prof.end_time));
	
	prof.is_initialised = true;
	prof.StoreThis();
	
	prof.storage.Update();
	
	Close();
}

















Profile::Profile() {
	LoadThis();
	tmp_usage_start = GetSysTime();
	
	if (is_initialised)
		storage.Update();
}

void Profile::AddWeightStat(int kgs, String jpg_file_path) {
	WeightLossStat& wl = weight.Add();
	wl.weight = kgs;
	wl.time = GetSysTime();
	wl.jpg_file = jpg_file_path;
	
	StoreThis();
}

Time Profile::GetCurrentTotalBegin() {
	return begin_time;
}

Time Profile::GetCurrentTotalEnd() {
	return end_time;
}

Time Profile::GetCurrentWeekBegin() {
	Time t = begin_time;
	Time now = GetSysTime();
	Time prev = t;
	while (1) {
		t += GetWeekSeconds();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

Time Profile::GetCurrentMonthBegin() {
	Time t = begin_time;
	Time now = GetSysTime();
	Time prev = t;
	while (1) {
		t += GetMonthSeconds();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

Time Profile::GetCurrentQuarterBegin() {
	Time t = begin_time;
	Time now = GetSysTime();
	Time prev = t;
	while (1) {
		t += 3 * GetMonthSeconds();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}



void Profile::LoadThis() {
	String file = ConfigFile("latest.bin");
	LoadFromFile(*this, file);
}

void Profile::StoreThis() {
	String tstr = Format("%", GetSysTime());
	tstr.Replace("/", "_");
	tstr.Replace(":", "_");
	tstr.Replace(" ", "_");
	String file = ConfigFile(tstr + ".bin");
	StoreToFile(*this, file);
	
	file = ConfigFile("latest.bin");
	StoreToFile(*this, file);
}

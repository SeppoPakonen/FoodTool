#include "FoodTool.h"
#include "Data.brc"
#include <plugin/bz2/bz2.h>


ProfileCreator::ProfileCreator() {
	CtrlLayout(*this, "Profile Creator");
	Icon(Images::icon);
	
	gender.SetData(1);
	height.SetData(176);
	weight.SetData(100);
	age.SetData(30);
	bodyfat.SetData(40);
	activity.SetData(0);
	walking_dist.SetData(0);
	tgt_weight.SetData(65);
	tgt_walking_dist.SetData(0);
	tgt_jogging_dist.SetData(0);
	shop_interval.SetData(5);
	hours_between_meals.SetData(1);
	easy_day_interval.SetData(2);
	waking.SetTime(5,0,0);
	sleeping.SetTime(20,0,0);
	
	UpdateTargetWeight();
	
	height <<= THISBACK(UpdateTargetWeight);
	next <<= THISBACK(Next);
	fatref <<= THISBACK(ShowWeightReference);
}

void ProfileCreator::UpdateTargetWeight() {
	tgt_weight.SetData(GetTargetWeight((double)this->height.GetData() * 0.01, 19));
}

FatPercentageReferenceWindow::FatPercentageReferenceWindow() {
	Title("Fat-percentage reference");
	String bodyfat_jpg = BZ2Decompress(bodyfat, bodyfat_length);
	Image i = StreamRaster::LoadStringAny(bodyfat_jpg);
	img.SetImage(i);
	SetRect(0, 0, i.GetWidth(), i.GetHeight() + 30);
	Add(img.HSizePos().VSizePos(0,30));
	Add(close.HSizePos().BottomPos(0,30));
	close.SetLabel("Close");
	close <<= THISBACK(Close0);
}

void ProfileCreator::ShowWeightReference() {
	if (!iw.IsOpen())
		iw.Open(this);
}

void ProfileCreator::Next() {
	Profile& prof = GetProfile();
	
	Configuration& conf = prof.confs.Add();
	
	prof.is_male = gender.GetData();
	
	conf.added = GetSysTime();
	conf.height = height.GetData();
	conf.age = age.GetData();
	conf.bodyfat = bodyfat.GetData();
	conf.activity = activity.GetData();
	conf.walking_dist = walking_dist.GetData();
	conf.tgt_weight = tgt_weight.GetData();
	conf.tgt_walking_dist = tgt_walking_dist.GetData();
	conf.tgt_jogging_dist = tgt_jogging_dist.GetData();
	conf.shop_interval = shop_interval.GetData();
	conf.hours_between_meals = hours_between_meals.GetData();
	conf.easy_day_interval = easy_day_interval.GetData();
	conf.waking_hour = waking.GetHour();
	conf.waking_minute = waking.GetMinute();
	conf.sleeping_hour = sleeping.GetHour();
	conf.sleeping_minute = sleeping.GetMinute();
	
	prof.begin_date = GetSysTime();
	prof.begin_date--;
	
	prof.AddWeightStat(weight.GetData());
	
	prof.UpdatePlan();
	
	int days = prof.planned_daily.Top().date.Get() - prof.begin_date.Get();
	
	LOG("Calory-deficit: " << (prof.av_calorie_deficit * 100) << "\%");
	LOG("Started " << Format("%", prof.begin_date));
	LOG("Stopping " << Format("%", conf.end_date));
	LOG("Days total " << days);
	
	
	prof.is_initialised = true;
	
	prof.storage.Init(prof.begin_date);
	prof.storage.Update(prof.planned_daily);
	
	prof.StoreThis();
	
	Close();
}


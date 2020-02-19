#include "FoodTool.h"
#include "Data.brc"
#include <plugin/bz2/bz2.h>


ProfileCreator::ProfileCreator() {
	Title("FoodTool first start wizard");
	Icon(Images::icon);
	
	CtrlLayout(tab0);
	CtrlLayout(tab1);
	CtrlLayout(tab2);
	CtrlLayout(tab3);
	CtrlLayout(tab4);
	SetRect(tab0.GetSize());
	
	Add(tab0.SizePos());
	Add(tab1.SizePos());
	Add(tab2.SizePos());
	Add(tab3.SizePos());
	Add(tab4.SizePos());
	Add(tab5.SizePos());
	tab1.Hide();
	tab2.Hide();
	tab3.Hide();
	tab4.Hide();
	tab5.Hide();
	
	tab0.next <<= THISBACK(Next);
	tab1.next <<= THISBACK(Next);
	tab2.next <<= THISBACK(Next);
	tab3.next <<= THISBACK(Next);
	tab4.next <<= THISBACK(Next);
	tab5.next <<= THISBACK(Next);
	tab5.prev <<= THISBACK(Previous);
	tab4.prev <<= THISBACK(Previous);
	tab3.prev <<= THISBACK(Previous);
	tab2.prev <<= THISBACK(Previous);
	tab1.prev <<= THISBACK(Previous);
	
	tab0.welcome.SetQTF(BZ2Decompress(welcome_qtf, welcome_qtf_length));
	
	tab1.gender.SetData(1);
	tab1.height.SetData(176);
	tab1.weight.SetData(100);
	tab1.age.SetData(30);
	tab1.bodyfat.SetData(40);
	tab1.activity.SetData(0);
	tab1.walking_dist.SetData(0);
	tab1.tgt_weight.SetData(65);
	tab1.tgt_walking_dist.SetData(6.6);
	tab1.tgt_jogging_dist.SetData(0);
	tab1.shop_interval.SetData(5);
	tab1.hours_between_meals.SetData(1);
	tab1.easy_day_interval.SetData(7);
	tab1.waking.SetTime(5,0,0);
	tab1.sleeping.SetTime(20,0,0);
	UpdateTargetWeight();
	tab1.height <<= THISBACK(UpdateTargetWeight);
	tab1.fatref <<= THISBACK(ShowWeightReference);
	
	tab3.preset.SetData(0);
	
}

void ProfileCreator::UpdateTargetWeight() {
	tab1.tgt_weight.SetData(GetTargetWeight((double)tab1.height.GetData() * 0.01, 19));
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

void ProfileCreator::Previous() {
	tab0.Hide();
	tab1.Hide();
	tab2.Hide();
	tab3.Hide();
	tab4.Hide();
	tab5.Hide();
	
	
	tab--;
	if (tab == 0) tab0.Show();
	if (tab == 1) tab1.Show();
	if (tab == 2) tab2.Show();
	if (tab == 3) tab3.Show();
	if (tab == 4) tab4.Show();
}
void ProfileCreator::Next() {
	if (tab == 0) {
		
	}
	else if (tab == 1) {
		Profile& prof = GetProfile();
		
		Configuration& conf = prof.confs.GetCount() ? prof.confs.Top() : prof.confs.Add();
		
		prof.is_male = tab1.gender.GetData();
		
		conf.added = GetSysTime();
		conf.height = tab1.height.GetData();
		conf.age = tab1.age.GetData();
		conf.bodyfat = tab1.bodyfat.GetData();
		conf.activity = tab1.activity.GetData();
		conf.walking_dist = tab1.walking_dist.GetData();
		conf.tgt_weight = tab1.tgt_weight.GetData();
		conf.tgt_walking_dist = tab1.tgt_walking_dist.GetData();
		conf.tgt_jogging_dist = tab1.tgt_jogging_dist.GetData();
		conf.shop_interval = tab1.shop_interval.GetData();
		conf.hours_between_meals = tab1.hours_between_meals.GetData();
		conf.easy_day_interval = tab1.easy_day_interval.GetData();
		conf.waking_hour = tab1.waking.GetHour();
		conf.waking_minute = tab1.waking.GetMinute();
		conf.sleeping_hour = tab1.sleeping.GetHour();
		conf.sleeping_minute = tab1.sleeping.GetMinute();
		
		prof.begin_date = GetSysTime();
		prof.begin_date--;
		
		if (prof.weights.IsEmpty())
			prof.AddWeightStat(tab1.weight.GetData());
		
		if (prof.storage.days.IsEmpty())
			prof.storage.Init(prof.begin_date);
		
	}
	else if (tab == 2) {
		Profile& prof = GetProfile();
		if (prof.defs.IsEmpty())
			prof.defs.Add(tab2.edit.prof);
		else
			prof.defs.Top() = tab2.edit.prof;
	}
	else if (tab == 3) {
		Database& db = DB();
		int preset = tab3.preset.GetData();
		
		if (db.used_foods.IsEmpty()) {
			if (preset == 0) {
				db.VLCD_Preset();
			}
			tab3.preset.Disable();
		}
		
		tab4.edit.Data();
	}
	else if (tab == 4) {
		DB().StartStoreThis();
		
		Profile& prof = GetProfile();
		if (prof.storage.meal_types.IsEmpty()) {
			prof.VLCD_Preset();
		}
	}
	else if (tab == 5) {
		DB().StartStoreThis();
		
		Profile& prof = GetProfile();
		prof.is_initialised = true;
		prof.Start(true);
		Close();
		return;
	}
	
	tab0.Hide();
	tab1.Hide();
	tab2.Hide();
	tab3.Hide();
	tab4.Hide();
	tab5.Hide();
	
	tab++;
	if (tab == 1) tab1.Show();
	if (tab == 2) tab2.Show();
	if (tab == 3) tab3.Show();
	if (tab == 4) tab4.Show();
	if (tab == 5) tab5.Show();
}


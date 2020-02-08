#include "FoodTool.h"

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_source.h>


FoodTool::FoodTool()
{
	Title("FoodTool");
	Icon(Images::icon);
	Sizeable().MaximizeBox().MinimizeBox();
	Maximize();
	
	Add(tabs.SizePos());
	
	tabs.Add(status.SizePos(), "Status");
	tabs.Add(weight.SizePos(), "Weight");
	tabs.Add(exc.SizePos(), "Exceptions");
	tabs.Add(notes.SizePos(), "Notes");
	tabs.Add(usage.SizePos(), "Usage");
	tabs.Add(conf.SizePos(), "Configuration");
	tabs.WhenSet << THISBACK(Data);
	
	tc.Set(-500, THISBACK(Data));
}

void FoodTool::Data() {
	int tab = tabs.Get();
	
	if (tab == 0)
		status.Data();
	
}





StatusCtrl::StatusCtrl() {
	CtrlLayout(*this);
	
	cur_date = Date(0,0,0);
	
	Data();
}

void StatusCtrl::Data() {
	Profile& prof = GetProfile();
	Time now = GetSysTime();
	Date today = now;
	
	if (today != cur_date) {
		cur_date = today;
		
		Time shop_begin = prof.storage.GetLastShopping();
		Time shop_end = prof.storage.GetNextShopping();
		int64 shop_total_sec = shop_end.Get() - shop_begin.Get();
		int64 shop_actual_sec = now.Get() - shop_begin.Get();
		shop_prog.Set(shop_actual_sec, shop_total_sec);
		
		this->today.SetData(prof.storage.GetTodaysMenu());
		
		next_shopping.SetTime(shop_end);
		
		shopping_list.SetData(prof.storage.GetNextShoppingList());
		
		Time total_begin = prof.GetCurrentTotalBegin();
		Time total_end = prof.GetCurrentTotalEnd();
		int64 total_total_sec = total_end.Get() - total_begin.Get();
		int64 total_actual_sec = now.Get() - total_begin.Get();
		total_prog.Set(total_actual_sec, total_total_sec);
		
		Time week_begin = prof.GetCurrentWeekBegin();
		Time week_end = week_begin + GetWeekSeconds();
		if (week_end > total_end) week_end = total_end;
		int64 week_total_sec = week_end.Get() - week_begin.Get();
		int64 week_actual_sec = now.Get() - week_begin.Get();
		week_prog.Set(week_actual_sec, week_total_sec);
		
		Time month_begin = prof.GetCurrentMonthBegin();
		Time month_end = month_begin + GetMonthSeconds();
		if (month_end > total_end) month_end = total_end;
		int64 month_total_sec = month_end.Get() - month_begin.Get();
		int64 month_actual_sec = now.Get() - month_begin.Get();
		month_prog.Set(month_actual_sec, month_total_sec);
		
		Time quarter_begin = prof.GetCurrentQuarterBegin();
		Time quarter_end = quarter_begin + 3 * GetMonthSeconds();
		if (quarter_end > total_end) quarter_end = total_end;
		int64 quarter_total_sec = quarter_end.Get() - quarter_begin.Get();
		int64 quarter_actual_sec = now.Get() - quarter_begin.Get();
		quart_prog.Set(quarter_actual_sec, quarter_total_sec);
	}
}




ConfigurationCtrl::ConfigurationCtrl() {
	CtrlLayout(*this);
	
	Profile& prof = GetProfile();
	
	/*keto_cals.SetData(prof.keto_cals);
	keto_fats.SetData(prof.keto_fats);
	keto_carbs.SetData(prof.keto_carbs);
	keto_prots.SetData(prof.keto_prots);
	
	wl_start.SetData(prof.wl_start);
	wl_loss.SetData(prof.wl_loss);
	wl_months.SetData(prof.wl_months);
	wl_cals.SetData(prof.wl_cals);
	
	shop_interval.SetData(prof.storage.shop_interval);
	
	begin.SetData(prof.begin_time);
	end.SetData(prof.end_time);*/
}






ExceptionsCtrl::ExceptionsCtrl() {
	
}






NoteCtrl::NoteCtrl() {
	
}






UsageCtrl::UsageCtrl() {
	
}






WeightCtrl::WeightCtrl() {
	
}






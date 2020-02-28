#include "FoodCtrl.h"


StatusCtrl::StatusCtrl() {
	CtrlLayout(*this);
	
	cur_date = Date(0,0,0);
	
	Data();
}

void StatusCtrl::Data() {
	Profile& prof = GetProfile();
	Date now = GetSysTime();
	
	if (now != cur_date) {
		cur_date = now;
		
		Date shop_begin = prof.storage.GetLastShopping();
		Date shop_end = prof.storage.GetNextShopping();
		int64 shop_total = shop_end.Get() - shop_begin.Get();
		int64 shop_actual = now.Get() - shop_begin.Get();
		shop_prog.Set(shop_actual, shop_total);
		
		today.SetData(prof.storage.GetTodaysMenu());
		
		next_shopping.SetDate(shop_end);
		
		Date total_begin = prof.GetCurrentTotalBegin();
		Date total_end = prof.GetCurrentTotalEnd();
		int64 total_total = total_end.Get() - total_begin.Get();
		int64 total_actual = now.Get() - total_begin.Get();
		total_prog.Set(total_actual, total_total);
		
		Date week_begin = prof.GetCurrentWeekBegin();
		Date week_end = week_begin + 7;
		if (week_end > total_end) week_end = total_end;
		int64 week_total = week_end.Get() - week_begin.Get();
		int64 week_actual = now.Get() - week_begin.Get();
		week_prog.Set(week_actual, week_total);
		
		Date month_begin = prof.GetCurrentMonthBegin();
		Date month_end = month_begin + GetMonthDays();
		if (month_end > total_end) month_end = total_end;
		int64 month_total = month_end.Get() - month_begin.Get();
		int64 month_actual = now.Get() - month_begin.Get();
		month_prog.Set(month_actual, month_total);
		
		Date quarter_begin = prof.GetCurrentQuarterBegin();
		Date quarter_end = quarter_begin + GetQuarterDays();
		if (quarter_end > total_end) quarter_end = total_end;
		int64 quarter_total = quarter_end.Get() - quarter_begin.Get();
		int64 quarter_actual = now.Get() - quarter_begin.Get();
		quart_prog.Set(quarter_actual, quarter_total);
	}
	
	today.Refresh();
}


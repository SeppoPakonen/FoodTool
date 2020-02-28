#include "FoodCtrl.h"



ExceptionsCtrl::ExceptionsCtrl() {
	CtrlLayout(exc);
	Add(split.SizePos());
	split.Horz() << list << exc;
	split.SetPos(3333);
	
	list.AddColumn("#");
	list.AddColumn(t_("Begin"));
	list.AddColumn(t_("Days"));
	list.AddColumn(t_("Deficit"));
	list.AddColumn(t_("Reason"));
	list.ColumnWidths("1 4 2 2 6");
	
	exc.cal_deficit.SetData(20);
	exc.days.SetData(1);
	exc.add <<= THISBACK(AddException);
}

void ExceptionsCtrl::Data(bool force) {
	Profile& prof = GetProfile();
	
	SplitterHotfix(split, 3333);
	
	if (prof.exceptions.GetCount() != list.GetCount() || force) {
		for(int i = 0; i < prof.exceptions.GetCount(); i++) {
			int row = prof.exceptions.GetCount() - 1 - i;
			const IntakeExceptions& e = prof.exceptions[i];
			list.Set(row, 0, i+1);
			list.Set(row, 1, e.begin);
			list.Set(row, 2, e.end.Get() - e.begin.Get());
			list.Set(row, 3, e.calorie_deficit);
			list.Set(row, 4, e.reason);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
	}
}

void ExceptionsCtrl::AddException() {
	Profile& prof = GetProfile();
	Date begin = GetSysTime();
	begin++;
	int existing = -1;
	for(int i = prof.exceptions.GetCount()-1; i >= 0; i--)
		if (prof.exceptions[i].begin == begin)
			{existing = i; break;}
	IntakeExceptions& e = existing < 0 ? prof.exceptions.Add() : prof.exceptions[existing];
	e.reason = exc.reason.GetData();
	e.calorie_deficit = exc.cal_deficit.GetData();
	e.begin = begin;
	e.end = e.begin + (int)exc.days.GetData();
	
	exc.reason.Clear();
	exc.cal_deficit.Clear();
	exc.days.Clear();
	
	prof.Start(true);
	
	Data(true);
}

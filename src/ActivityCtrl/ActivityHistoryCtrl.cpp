#include "ActivityCtrl.h"

ActivityHistoryCtrl::ActivityHistoryCtrl() {
	Add(hsplit.SizePos());
	hsplit.Horz() << list << items;
	
	list.AddIndex();
	list.AddColumn(t_("Time"));
	list.AddColumn(t_("Length"));
	list.AddColumn(t_("Type"));
	list.AddColumn(t_("What"));
	list.AddColumn(t_("Kcal"));
	list.AddColumn(t_("Heartrate (BPM)"));
	list.ColumnWidths("2 1 2 3 1 1");
	list <<= THISBACK(SelectGroup);
	
	items.AddIndex();
	items.AddColumn(t_("Time"));
	items.AddColumn(t_("Duration"));
	items.AddColumn(t_("Type"));
	items.AddColumn(t_("What"));
	items.AddColumn(t_("Kcal"));
	items.AddColumn(t_("Heartrate (BPM)"));
	items.ColumnWidths("2 1 2 3 1 1");
}

void ActivityHistoryCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(hsplit, 5000);
	
	if (prof.activity.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.activity.GetCount(); i++) {
			int row = prof.activity.GetCount() - 1 - i;
			const ActivityGroupItem& it = prof.activity[i];
			list.Set(row, 0, i);
			list.Set(row, 1, it.main.begin);
			list.Set(row, 2, GetTimeDurationString(it.main.begin, it.main.end));
			list.Set(row, 3, it.main.GetTypeString());
			list.Set(row, 4, it.main.what);
			list.Set(row, 5, it.main.kcal);
			list.Set(row, 6, it.main.heartrate);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectGroup();
	}
}

void ActivityHistoryCtrl::SelectGroup() {
	if (!list.IsCursor())
		return;
	
	const Profile& prof = GetProfile();
	int group_i = list.Get(list.GetCursor(), 0);
	const ActivityGroupItem& gr = prof.activity[group_i];
	
	for(int i = 0; i < gr.items.GetCount(); i++) {
		const ActivityItem& it = gr.items[i];
		items.Set(i, 0, i);
		items.Set(i, 1, it.begin);
		items.Set(i, 2, GetTimeDurationString(it.begin, it.end));
		items.Set(i, 3, it.GetTypeString());
		items.Set(i, 4, it.what);
		items.Set(i, 5, it.kcal);
		items.Set(i, 6, it.heartrate);
	}
	items.SetCount(gr.items.GetCount());
}


#include "FoodCtrl.h"


ConfigurationCtrl::ConfigurationCtrl() {
	Add(split.SizePos());
	split.Horz() << list << conf;
	split.SetPos(2500);
	
	CtrlLayout(conf);
	conf.next.SetLabel(t_("Add new"));
	conf.gender.SetEditable(false);
	conf.next <<= THISBACK(AddConf);
	list.AddColumn("#");
	list.AddColumn(t_("Added"));
	list.ColumnWidths("1 5");
	list.WhenAction = THISBACK(SelectConf);
	list.WhenLeftClick = THISBACK(SelectConf);
	
}

void ConfigurationCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(split, 2500);
	
	if (prof.confs.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.confs.GetCount(); i++) {
			int row = prof.confs.GetCount() - 1 - i;
			const Configuration& c = prof.confs[i];
			list.Set(row, 0, i+1);
			list.Set(row, 1, c.added);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectConf();
	}
}

void ConfigurationCtrl::SelectConf() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int conf_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const Configuration& c = prof.confs[conf_i];
	
	conf.gender.SetData(prof.is_male);
	conf.height.SetData(c.height);
	conf.age.SetData(c.age);
	conf.activity.SetData(c.activity);
	conf.walking_dist.SetData(c.walking_dist);
	conf.tgt_exercise_kcal.SetData(c.tgt_exercise_kcal);
	conf.tgt_walking_dist.SetData(c.tgt_walking_dist);
	conf.tgt_jogging_dist.SetData(c.tgt_jogging_dist);
	conf.shop_interval.SetData(c.shop_interval);
	conf.hours_between_meals.SetData(c.hours_between_meals);
	conf.hours_between_making_meals.SetData(c.hours_between_making_meals);
	conf.easy_day_interval.SetData(c.easy_day_interval);
	conf.waking.SetTime(c.waking_hour,c.waking_minute, 0);
	conf.sleeping.SetTime(c.sleeping_hour,c.sleeping_minute,0);
	conf.coffee.SetData(c.daily_coffee);
}

void ConfigurationCtrl::AddConf() {
	Profile& prof = GetProfile();
	Configuration& c = prof.confs.Add();
	
	c.added = GetSysTime();
	c.end_date = Null;
	c.height = conf.height.GetData();
	c.age = conf.age.GetData();
	c.activity = conf.activity.GetData();
	c.walking_dist = conf.walking_dist.GetData();
	c.tgt_exercise_kcal = conf.tgt_exercise_kcal.GetData();
	c.tgt_walking_dist = conf.tgt_walking_dist.GetData();
	c.tgt_jogging_dist = conf.tgt_jogging_dist.GetData();
	c.shop_interval = conf.shop_interval.GetData();
	c.hours_between_meals = conf.hours_between_meals.GetData();
	c.hours_between_making_meals = conf.hours_between_making_meals.GetData();
	c.easy_day_interval = conf.easy_day_interval.GetData();
	c.waking_hour = conf.waking.GetHour();
	c.waking_minute = conf.waking.GetMinute();
	c.sleeping_hour = conf.sleeping.GetHour();
	c.sleeping_minute = conf.sleeping.GetMinute();
	c.daily_coffee = conf.coffee.GetData();
	
	prof.Start(true);
	
	Data();
}

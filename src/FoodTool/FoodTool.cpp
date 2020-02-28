#include "FoodTool.h"

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_source.h>







FoodTool::FoodTool()
{
	Title(t_("FoodTool"));
	Icon(Images::icon);
	Sizeable().MaximizeBox().MinimizeBox();
	Maximize();
	
	AddFrame(menu);
	menu.Set(THISBACK(MainMenu));
	
	Add(updating_lbl.HCenterPos(500).VCenterPos(30));
	Add(tabs.SizePos());
	
	updating_lbl.Show();
	updating_lbl.SetLabel(t_("Updating plan and meals..."));
	updating_lbl.SetFont(SansSerif(30*0.8));
	updating_lbl.SetInk(Color(85, 42, 0));
	updating_lbl.AlignCenter();
	
	today.WhenAlert = THISBACK(SetTodayTab);
	
	foodlog.mode = FOODLOG;
	shoplog.mode = GROCERYLOG;
	receiptlog.mode = RECEIPTLOG;
	
	tabs.Hide();
	tabs.Add(motivation.SizePos(), t_("Motivation"));
	tabs.Add(today.SizePos(), t_("Today"));
	tabs.Add(status.SizePos(), t_("Status"));
	tabs.Add(nutr.SizePos(), t_("Daily Nutritions"));
	tabs.Add(fins.SizePos(), t_("Food Instructions"));
	tabs.Add(activity.SizePos(), t_("Activity"));
	tabs.Add(graphs.SizePos(), t_("Graphs"));
	tabs.Add(weight.SizePos(), t_("Weight"));
	tabs.Add(def.SizePos(), t_("Nutrient Deficits"));
	tabs.Add(exc.SizePos(), t_("Exceptions"));
	tabs.Add(notes.SizePos(), t_("Notes"));
	tabs.Add(db.SizePos(), t_("Food Database"));
	tabs.Add(preset.SizePos(), t_("Meal Presets"));
	tabs.Add(supp.SizePos(), t_("Food Supplements"));
	tabs.Add(wish.SizePos(), t_("Food Wish-list"));
	tabs.Add(foodlog.SizePos(), t_("Food Log"));
	tabs.Add(shoplog.SizePos(), t_("Shop Log"));
	tabs.Add(receiptlog.SizePos(), t_("Receipt Log"));
	tabs.Add(storage.SizePos(), t_("Food Storage"));
	tabs.Add(prices.SizePos(), t_("Prices"));
	tabs.Add(conf.SizePos(), t_("Configuration"));
	tabs.Add(usage.SizePos(), t_("Usage"));
	tabs.WhenSet << THISBACK(Data);
	
	tc.Set(-500, THISBACK(Data));
	
	GetProfile().WhenUpdateReady << THISBACK(PostData);
}

void FoodTool::MainMenu(Bar& bar) {
	bar.Sub(t_("App"), [this](Bar& bar) {
		bar.Add(t_("Save Profile"), THISBACK(SaveProfile)).Key(K_CTRL_S);
		bar.Add(t_("Replan"), THISBACK(Replan)).Key(K_CTRL_R);
	});
	
}

void FoodTool::SaveProfile() {
	GetProfile().StoreThis();
}

void FoodTool::Replan() {
	GetProfile().Start(true);
}

void FoodTool::SetTodayTab() {
	tabs.Set(1);
	Data();
}

void FoodTool::Data() {
	bool is_updating = GetProfile().IsRunning();
	if (is_updating) {
		if (!was_updating) {
			tabs.Hide();
			updating_lbl.Show();
		}
	}
	else {
		if (was_updating) {
			tabs.Show();
			updating_lbl.Hide();
		}
		int tab = tabs.Get();
		bool tab_changed = tab != prev_tab;
		prev_tab = tab;
		
		int i = 0;
		if      (tab == i++)	motivation.Data();
		else if (tab == i++)	today.Data();
		else if (tab == i++)	status.Data();
		else if (tab == i++)	nutr.Data();
		else if (tab == i++)	fins.Data();
		else if (tab == i++)	activity.Data();
		else if (tab == i++)	graphs.Data();
		else if (tab == i++)	weight.Data();
		else if (tab == i++)	def.Data();
		else if (tab == i++)	exc.Data();
		else if (tab == i++)	notes.Data();
		else if (tab == i++)	db.Data();
		else if (tab == i++)	preset.Data();
		else if (tab == i++)	supp.Data();
		else if (tab == i++)	wish.Data();
		else if (tab == i++)	foodlog.Data();
		else if (tab == i++)	shoplog.Data();
		else if (tab == i++)	receiptlog.Data();
		else if (tab == i++)	storage.Data();
		else if (tab == i++)	prices.Data(tab_changed);
		else if (tab == i++)	conf.Data();
		else if (tab == i++)	usage.Data();
	}
	was_updating = is_updating;
}












#define TFILE <FoodTool/Translation.t>
#include <Core/t.h>
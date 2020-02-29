#ifndef _FoodTool_FoodTool_h
#define _FoodTool_FoodTool_h

#include <ActivityCtrl/ActivityCtrl.h>

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_header.h>






class FoodTool : public TopWindow {
	TabCtrl tabs;
	bool was_updating = true;
	Label updating_lbl;
	int prev_tab = -1;
	
	MenuBar menu;
	
	MotivationCtrl motivation;
	StatusCtrl status;
	DailyNutritionsCtrl nutr;
	FoodInstructionCtrl fins;
	ConfigurationCtrl conf;
	ExceptionsCtrl exc;
	NoteCtrl notes;
	UsageCtrl usage;
	WeightCtrl weight;
	GraphCtrl graphs;
	TodayScheduleCtrl today;
	HairSampleDeficitCtrl def;
	MealPresetCtrl preset;
	DatabaseCtrl db;
	FoodStorageCtrl storage;
	FoodWishCtrl wish;
	SupplementCtrl supp;
	FoodLogCtrl foodlog, shoplog, receiptlog;
	PriceCtrl prices;
	ActivityCtrl activity;
	ExerciseEditorCtrl exeredit;
	ExercisePlayerCtrl exerplayer;
	ActivityHistoryCtrl acthis;
	
	TimeCallback tc;
public:
	typedef FoodTool CLASSNAME;
	FoodTool();
	
	void MainMenu(Bar& bar);
	void SaveProfile();
	void Replan();
	void Data();
	void SetTodayTab();
	void PostData() {PostCallback(THISBACK(Data));}
};

void FillVector(Vector<double>& v);

#endif

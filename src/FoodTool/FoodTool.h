#ifndef _FoodTool_FoodTool_h
#define _FoodTool_FoodTool_h

#define int64 int64_fuck_opencv
#define uint64 uint64_fuck_opencv
#undef CPU_SSE2
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#undef int64
#undef uint64

#include <random>
#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>
#include <plugin/jpg/jpg.h>

using namespace Upp;

#define IMAGECLASS Images
#define IMAGEFILE <FoodTool/Images.iml>
#include <Draw/iml_header.h>

#include "Util.h"
#include "Database.h"
#include "Optimizer.h"
#include "StepOptimizer.h"
#include "Ingredient.h"
#include "Food.h"
#include "FoodStorage.h"
#include "Activity.h"
#include "Profile.h"
#include "VideoCapture.h"
#include "ScheduleCtrl.h"

#define LAYOUTFILE <FoodTool/FoodTool1.lay>
#include <CtrlCore/lay.h>

#include "DatabaseCtrl.h"
#include "MealPresetCtrl.h"
#include "WeightCtrl.h"
#include "FoodStorageCtrl.h"
#include "ActivityCtrl.h"

#define LAYOUTFILE <FoodTool/FoodTool2.lay>
#include <CtrlCore/lay.h>

#include "ProfileCreator.h"

Image GetSmiley(String s);
Image GetSmiley(double progress);

struct MotivationCtrl : public Ctrl {
	int motivation_i = -1, quote_i = -1;
	Image img;
	Vector<String> quotes;
	
	typedef MotivationCtrl CLASSNAME;
	MotivationCtrl();
	
	void Data() {Refresh();}
	void Paint(Draw& d);
};

struct StatusCtrl : public WithStatusLayout<ParentCtrl> {
	Date cur_date;
	
	typedef StatusCtrl CLASSNAME;
	StatusCtrl();
	
	void Data();
};

struct ConfigurationCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithConfigurationLayout<ParentCtrl> conf;
	
	typedef ConfigurationCtrl CLASSNAME;
	ConfigurationCtrl();
	
	void Data();
	void SelectConf();
	void AddConf();
};

struct ExceptionsCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithExceptionLayout<ParentCtrl> exc;
	
	typedef ExceptionsCtrl CLASSNAME;
	ExceptionsCtrl();
	
	void Data(bool force=false);
	void AddException();
	
};

struct NoteCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithNoteLayout<ParentCtrl> note;
	
	
	typedef NoteCtrl CLASSNAME;
	NoteCtrl();
	
	void Data();
	void AddNote();
	void SelectNote();
};

struct UsageCtrl : public ParentCtrl {
	ArrayCtrl list;
	Label total;
	
	typedef UsageCtrl CLASSNAME;
	UsageCtrl();
	
	void Data();
};

struct MultipurposeGraph : public Ctrl {
	enum {
		MEASURES_BEGIN = 14
	};
	
	struct Line : Moveable<Line> {
		String title;
		Color color;
		int width;
	};
	struct Src : Moveable<Src> {
		Vector<Line> lines;
		int vert_line = 0;
		
		Src& Add(String t, int w, Color c) {Line& l = lines.Add(); l.title = t; l.width = w; l.color = c; return *this;}
		Src& Vert(int y) {vert_line = y; return *this;}
	};
	
private:
	Vector<Vector<double>> tmp;
	Vector<Point> polyline;
	Vector<Src> src;
	
public:
	int new_src = 0;
	
	Src& Add() {return src.Add();}
	int GetLineCount(int src);
	Color GetLineColor(int src, int l);
	int GetLineWidth(int src, int l);
	String GetLineTitle(int src, int l);
	int GetCount(int src);
	int GetVertLine(int src);
	int GetHorzLine(int src);
	const Vector<double>& GetValue(int src, int l);
	void Paint(Draw& d);
};

struct GraphCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	MultipurposeGraph graph;
	
	
	typedef GraphCtrl CLASSNAME;
	GraphCtrl();
	
	void Data() {graph.Refresh();}
	void SelectSource();
};

struct TodayScheduleCtrl : public Ctrl {
	ScheduleToday sched;
	int prof_version;
	Color waking_top, waking_btm;
	Color eating_top, eating_btm;
	Color walking_top, walking_btm;
	Color running_top, running_btm;
	Color sleeping_top, sleeping_btm;
	Color muscletraining_top, muscletraining_btm;
	TimeCallback tc;
	
	
	typedef TodayScheduleCtrl CLASSNAME;
	TodayScheduleCtrl();
	void Paint(Draw& d);
	void Data() {Refresh();}
	void CheckAlerts();
	void Alert(String alert_str);
	
	Callback WhenAlert;
};

struct NutrientDeficitCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	ParentCtrl right;
	Button add;
	NutrientDeficitEdit edit;
	
	
	typedef NutrientDeficitCtrl CLASSNAME;
	NutrientDeficitCtrl();
	void Data();
	void SelectItem();
	void AddItem();
};

struct FoodInstructionCtrl : public ParentCtrl {
	ArrayCtrl list, meallist, inglist, nutrlist;
	Splitter vsplit0, vsplit1, hsplit;
	
	
	typedef FoodInstructionCtrl CLASSNAME;
	FoodInstructionCtrl();
	void Data();
	void SelectDate();
	void SelectMeal();
	void SelectIngredient();
};

struct DailyNutritionsCtrl : public ParentCtrl {
	ArrayCtrl list, srclist, nutrlist;
	Splitter vsplit, hsplit;
	ArrayMap<String, Ingredient> src;
	
	typedef DailyNutritionsCtrl CLASSNAME;
	DailyNutritionsCtrl();
	void Data();
	void SelectDate();
	void SelectSource();
};

struct FoodWishCtrl : public ParentCtrl {
	Splitter vsplit;
	ArrayCtrl list, ingredients;
	Array<EditIntSpin> edits;
	
	typedef FoodWishCtrl CLASSNAME;
	FoodWishCtrl();
	void Data();
	void SelectIngredient();
	void ValueChanged(int preset_i);
};

struct SupplementCtrl : public WithSupplementLayout<ParentCtrl> {
	Array<Option> is_weightloss, is_maintenance;
	
	typedef SupplementCtrl CLASSNAME;
	SupplementCtrl();
	void Data();
	void AddFood();
	void RemoveFood();
	void SelectNutrition();
	void ValueChanged(int nutr_i);
};

enum {
	FOODLOG,
	GROCERYLOG,
	RECEIPTLOG,
};

struct FoodLogCtrl : public ParentCtrl {
	Splitter hsplit, vsplit;
	ArrayCtrl queue, history;
	WithProductListLayout<ParentCtrl> products;
	Array<EditIntSpin> grams, servings, batch;
	Array<EditDoubleSpin> price;
	Array<EditString> shop;
	
	int mode = 0;
	
	
	typedef FoodLogCtrl CLASSNAME;
	FoodLogCtrl();
	void Data();
	void SelectHistory();
	void SelectQueue();
	void Add();
	void Zero();
	void Expand();
	void Reduce();
	void ApplyStore();
	void Load(const FoodPrice& p);
	ProductQueueHistory& GetData();
	void SetData(ArrayCtrl& list, Vector<FoodPrice>& data);
	void SetEditCount(int i);
	void DebugClear() {GetData().history.Clear(); Data();}
	void ClearQueue() {GetData().queue.Clear(); Data();}
	void MultiplyMass();
};

struct PriceCtrl : public ParentCtrl {
	Splitter hsplit;
	ArrayCtrl foodlist;
	WithQuoteListLayout<ParentCtrl> history;
	Array<EditIntSpin> grams, servings, batch;
	Array<EditDoubleSpin> price;
	Array<EditString> shop;
	
	typedef PriceCtrl CLASSNAME;
	PriceCtrl();
	void Data(bool force);
	void SelectFood();
	void Add();
	void ValueChanged(int quote);
};

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
	NutrientDeficitCtrl def;
	MealPresetCtrl preset;
	DatabaseCtrl db;
	FoodStorageCtrl storage;
	FoodWishCtrl wish;
	SupplementCtrl supp;
	FoodLogCtrl foodlog, shoplog, receiptlog;
	PriceCtrl prices;
	ActivityCtrl activity;
	
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

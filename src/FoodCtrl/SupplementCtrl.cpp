#include "FoodCtrl.h"


SupplementCtrl::SupplementCtrl() {
	CtrlLayout(*this);
	
	nutrlist.AddColumn(t_("Nutrition"));
	nutrlist.AddColumn(t_("Recommendation"));
	nutrlist.AddColumn(t_("Weight-Loss"));
	nutrlist.AddColumn(t_("Maintenance"));
	nutrlist.ColumnWidths("4 1 1 1");
	nutrlist <<= THISBACK(SelectNutrition);
	
	usedlist.AddIndex();
	usedlist.AddColumn(t_("Food Name"));
	usedlist.AddColumn(t_("Value /100g"));
	usedlist.AddColumn(t_("\% of recommendation"));
	usedlist.ColumnWidths("4 1 1");
	
	foodlist.AddIndex();
	foodlist.AddColumn(t_("Food Name"));
	foodlist.AddColumn(t_("Value /100g"));
	foodlist.AddColumn(t_("\% of recommendation"));
	foodlist.ColumnWidths("4 1 1");
	
	add <<= THISBACK(AddFood);
	remove <<= THISBACK(RemoveFood);
	
}

void SupplementCtrl::Data() {
	const Database& db = DB();
	Profile& prof = GetProfile();
	
	if (db.nutr_recom.GetCount() != nutrlist.GetCount()) {
		DailyPlan& today = *prof.GetTodayPlan();
		
		prof.supplements.SetCount(db.nutr_recom.GetCount());
		is_weightloss.SetCount(db.nutr_recom.GetCount());
		is_maintenance.SetCount(db.nutr_recom.GetCount());
		
		for(int i = 0; i < db.nutr_recom.GetCount(); i++) {
			const NutritionRecommendation& recom = db.nutr_recom[i];
			const NutritionType& nt = db.nutr_types[recom.nutr_no];
			Option& wl = is_weightloss[i];
			Option& ma = is_maintenance[i];
			const NutritionSupplement& s = prof.supplements[i];
			wl <<= THISBACK1(ValueChanged, i);
			ma <<= THISBACK1(ValueChanged, i);
			
			nutrlist.Set(i, 0, nt.nutr_desc);
			nutrlist.Set(i, 1, Format("%2n", today.food.nutr[recom.nutr_no]));
			nutrlist.SetCtrl(i, 2, wl);
			nutrlist.SetCtrl(i, 3, ma);
			nutrlist.Set(i, 2, s.is_weightloss);
			nutrlist.Set(i, 3, s.is_maintenance);
		}
		if (!nutrlist.IsCursor() && nutrlist.GetCount())
			nutrlist.SetCursor(0);
		
		SelectNutrition();
	}
}

void SupplementCtrl::SelectNutrition() {
	if (!nutrlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	int cursor = nutrlist.GetCursor();
	const NutritionSupplement& ns = prof.supplements[cursor];
	const NutritionRecommendation& recom = db.nutr_recom[cursor];
	const DailyPlan& today = *prof.GetTodayPlan();
	Ingredient ing;
	double recom_value = today.food.nutr[recom.nutr_no];
	
	for(int i = 0; i < ns.used_food.GetCount(); i++) {
		int db_no = ns.used_food[i];
		const FoodDescription& d = db.food_descriptions[db_no];
		ing.Set(100, d);
		usedlist.Set(i, 0, db_no);
		usedlist.Set(i, 1, d.long_desc);
		usedlist.Set(i, 2, Format("%2n", ing.nutr[recom.nutr_no]));
		//usedlist.Set(i, 3, Format("%1n", ing.nutr[nutr_i] / recom_value * 100);
		usedlist.Set(i, 3, ing.nutr[recom.nutr_no] / recom_value * 100);
	}
	usedlist.SetCount(ns.used_food.GetCount());
	usedlist.SetSortColumn(2, true);
	
	int row = 0;
	for(int i = 0; i < db.food_descriptions.GetCount(); i++) {
		const FoodDescription& d = db.food_descriptions[i];
		ing.Set(100, d);
		double value = ing.nutr[recom.nutr_no];
		
		if (value > 0) {
			foodlist.Set(row, 0, i);
			foodlist.Set(row, 1, d.long_desc);
			foodlist.Set(row, 2, Format("%2n", value));
			foodlist.Set(row, 3, value / recom_value * 100);
			row++;
		}
	}
	foodlist.SetCount(row);
	foodlist.SetSortColumn(2, true);
	
}

void SupplementCtrl::AddFood() {
	if (!nutrlist.IsCursor() || !foodlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	int ncursor = nutrlist.GetCursor();
	NutritionSupplement& ns = prof.supplements[ncursor];
	int fcursor = foodlist.GetCursor();
	int db_no = foodlist.Get(fcursor, 0);
	
	ns.used_food.FindAdd(db_no);
	
	SelectNutrition();
	
	if (fcursor+1 < foodlist.GetCount())
		foodlist.SetCursor(fcursor+1);
}

void SupplementCtrl::RemoveFood() {
	if (!nutrlist.IsCursor() || !usedlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	int ncursor = nutrlist.GetCursor();
	NutritionSupplement& ns = prof.supplements[ncursor];
	int ucursor = usedlist.GetCursor();
	int db_no = usedlist.Get(ucursor, 0);
	
	ns.used_food.RemoveKey(db_no);
	
	SelectNutrition();
}

void SupplementCtrl::ValueChanged(int nutr_i) {
	if (!nutrlist.IsCursor())
		return;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	NutritionSupplement& ns = prof.supplements[nutr_i];
	
	ns.is_weightloss  = is_weightloss[nutr_i].Get();
	ns.is_maintenance = is_maintenance[nutr_i].Get();
}

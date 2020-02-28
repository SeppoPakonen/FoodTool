#include "FoodCtrl.h"


FoodStorageCtrl::FoodStorageCtrl() {
	CtrlLayout(*this);
	
	list.AddColumn(t_("Added"));
	list.AddColumn(t_("Source"));
	list.AddColumn(t_("Mass"));
	list.AddColumn(t_("Carbs"));
	list.AddColumn(t_("Fat"));
	list.AddColumn(t_("Protein"));
	list.AddColumn(t_("Energy"));
	list.ColumnWidths("3 1 1 1 1 1 1");
	list.WhenAction << THISBACK(SelectSnap);
	list.WhenLeftClick << THISBACK(SelectSnap);
	
	food_list.AddColumn(t_("Food"));
	food_list.AddColumn(t_("Grams"));
	food_list.ColumnWidths("4 1");
	
	add_snap <<= THISBACK(AddSnap);
}
	
void FoodStorageCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.storage_snaps.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.storage_snaps.GetCount(); i++) {
			int row = prof.storage_snaps.GetCount() - 1 - i;
			const FoodStorageSnapshot& n = prof.storage_snaps[i];
			Ingredient ing;
			n.GetNutritions(ing);
			list.Set(row, 0, n.time);
			list.Set(row, 1, GetSnapshotSourceString(n.adder));
			list.Set(row, 2, (int)ing.grams);
			list.Set(row, 3, (int)ing.nutr[CARB]);
			list.Set(row, 4, (int)ing.nutr[FAT]);
			list.Set(row, 5, (int)ing.nutr[PROT]);
			list.Set(row, 6, (int)ing.nutr[KCAL]);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectSnap();
	}
}

void FoodStorageCtrl::AddSnap() {
	Profile& prof = GetProfile();
	FoodStorageSnapshot& snap = prof.storage_snaps.Add();
	snap.time = GetSysTime();
	if (prof.storage_snaps.GetCount() >= 2)
		snap.foods <<= prof.storage_snaps[prof.storage_snaps.GetCount()-2].foods;
	
	for(const MealPreset& mp : prof.presets)
		for(const MealIngredient& ing : mp.ingredients)
			snap.foods.GetAdd(ing.db_food_no, 0);
	SortByKey(snap.foods, StdLess<int>());
	
	Data();
	list.SetCursor(0);
}

void FoodStorageCtrl::SelectSnap() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int snap_i = list.GetCount() - 1 - cursor;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	const FoodStorageSnapshot& snap = prof.storage_snaps[snap_i];
	edits.SetCount(max(edits.GetCount(), snap.foods.GetCount()));
	for(int i = 0; i < snap.foods.GetCount(); i++) {
		int db_no = snap.foods.GetKey(i);
		int grams = snap.foods[i];
		EditIntNotNull& edit = edits[i];
		food_list.Set(i, 0, db.food_descriptions[db_no].long_desc);
		food_list.SetCtrl(i, 1, edit);
		food_list.Set(i, 1, grams);
		edit <<= THISBACK(SnapChanged);
	}
	food_list.SetCursor(snap.foods.GetCount());
	edits.SetCount(snap.foods.GetCount());
}

void FoodStorageCtrl::SnapChanged() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int snap_i = list.GetCount() - 1 - cursor;
	
	const Database& db = DB();
	Profile& prof = GetProfile();
	FoodStorageSnapshot& snap = prof.storage_snaps[snap_i];
	
	for(int i = 0; i < edits.GetCount(); i++) {
		snap.foods[i] = edits[i].GetData();
	}
	
	Ingredient ing;
	snap.GetNutritions(ing);
	list.Set(cursor, 1, (int)ing.grams);
	list.Set(cursor, 2, (int)ing.nutr[CARB]);
	list.Set(cursor, 3, (int)ing.nutr[FAT]);
	list.Set(cursor, 4, (int)ing.nutr[PROT]);
	list.Set(cursor, 5, (int)ing.nutr[KCAL]);
}


void FindSetFoodStorageSnapshot(Time time, FoodQuantity& food_grams) {
	const Profile& prof = GetProfile();
	/*
		[0] = 08:00
		[1] = 16:00
		time = 12:00
		1. cmp(12:00 >= 16:00) = false
		2. cmp(12:00 >= 08:00) = true
	*/
	for(int i = prof.storage_snaps.GetCount()-1; i >= 0; i--) {
		const FoodStorageSnapshot& snap = prof.storage_snaps[i];
		if (time >= snap.time && Date(time) == Date(snap.time)) {
			for(int j = 0; j < snap.foods.GetCount(); j++) {
				int db_no = snap.foods.GetKey(j);
				int grams = snap.foods[j];
				food_grams.GetAdd(db_no) = grams;
			}
			break;
		}
	}
}

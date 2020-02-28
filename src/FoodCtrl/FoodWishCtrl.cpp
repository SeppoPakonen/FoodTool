#include "FoodCtrl.h"

FoodWishCtrl::FoodWishCtrl() {
	Add(vsplit.SizePos());
	vsplit << list << ingredients;
	
	list.AddIndex();
	list.AddColumn(t_("Preset"));
	list.AddColumn(t_("Score (0-10)"));
	list.ColumnWidths("4 1");
	list <<= THISBACK(SelectIngredient);
	
	ingredients.AddColumn(t_("Ingredient"));
	ingredients.AddColumn(t_("Average mass"));
	ingredients.ColumnWidths("3 1");
}

void FoodWishCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(vsplit, 5000);
	
	if (prof.presets.GetCount() != list.GetCount()) {
		VectorMap<int, double> preset_scores;
		for(int i = 0; i < prof.presets.GetCount(); i++)
			preset_scores.Add(i, prof.presets[i].score);
		SortByValue(preset_scores, StdGreater<double>());
		
		edits.SetCount(max(edits.GetCount(), prof.presets.GetCount()));
		for(int i = 0; i < preset_scores.GetCount(); i++) {
			int preset_i = preset_scores.GetKey(i);
			const MealPreset& mp = prof.presets[preset_i];
			EditIntSpin& edit = edits[preset_i];
			edit.WhenAction = THISBACK1(ValueChanged, preset_i);
			edit.MinMax(0, 10);
			
			list.Set(i, 0, preset_i);
			list.Set(i, 1, mp.name);
			list.SetCtrl(i, 1, edit);
			list.Set(i, 2, mp.wished_factor);
		}
		list.SetCount(prof.presets.GetCount());
	}
	
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	
	SelectIngredient();
}

void FoodWishCtrl::SelectIngredient() {
	if (!list.IsCursor())
		return;
	
	int cursor = list.GetCursor();
	int preset_i = list.Get(cursor, 0);
	const Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[preset_i];
	const Database& db = DB();
	
	for(int i = 0; i < mp.ingredients.GetCount(); i++) {
		const MealIngredient& mi = mp.ingredients[i];
		ingredients.Set(i, 0, db.food_descriptions[mi.db_food_no].long_desc);
		ingredients.Set(i, 1, Format("%1n", (mi.min_grams + mi.max_grams) * 0.5));
	}
	ingredients.SetCount(mp.ingredients.GetCount());
}

void FoodWishCtrl::ValueChanged(int preset_i) {
	Profile& prof = GetProfile();
	int value = edits[preset_i].GetData();
	prof.presets[preset_i].wished_factor = value;
}

#include "FoodTool.h"

MealPresetCtrl::MealPresetCtrl() {
	CtrlLayout(*this);
	
	list_sources.Add("All foods");
	list_sources.Add("Used foods");
	list_sources.SetIndex(1);
	list_sources <<= THISBACK(Filter);
	
	list.AddIndex();
	list.AddColumn("Name");
	list.AddColumn("Mass");
	list.AddColumn("Taste");
	list.AddColumn("Score");
	list.ColumnWidths("3 1 1 1");
	list.WhenAction = THISBACK(SelectPreset);
	list.WhenLeftClick = THISBACK(SelectPreset);
	
	variants.AddColumn("Variant");
	variants.AddColumn("Mass");
	variants.AddColumn("Taste");
	variants.AddColumn("Score");
	variants.ColumnWidths("3 1 1 1");
	variants.WhenAction = THISBACK(SelectPreset);
	variants.WhenLeftClick = THISBACK(SelectPreset);
	
	inglist.AddColumn("Name");
	inglist.AddColumn("Min (g)");
	inglist.AddColumn("Max (g)");
	inglist.AddColumn("Variant");
	inglist.ColumnWidths("3 1 1 1");
	
	food_list.AddIndex();
	food_list.AddColumn("Long description");
	
	name <<= THISBACK(PresetChanged);
	instructions <<= THISBACK(PresetChanged);
	
	add <<= THISBACK(Add);
	remove <<= THISBACK(Remove);
	makevars <<= THISBACK(MakeVariants);
	add_ingredient <<= THISBACK(AddIngredient);
	remove_ingredient <<= THISBACK(RemoveIngredient);
	filter.WhenEnter << THISBACK(Filter);
	add_vitamins <<= THISBACK(AddVitamins);
	add_minerals <<= THISBACK(AddMinerals);
	add_aminoacids <<= THISBACK(AddAminoAcids);
	add_fattyacids <<= THISBACK(AddFattyAcids);
	
	prev.Hide();
	next.Hide();
}

void MealPresetCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (food_list.GetCount() == 0)
		Filter();
	
	if (prof.presets.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.presets.GetCount(); i++) {
			int row = prof.presets.GetCount() - 1 - i;
			MealPreset& mp = prof.presets[i];
			if (!mp.score)
				mp.UpdateFactors();
			if (mp.ingredients.GetCount() && mp.variants.IsEmpty())
				mp.MakeVariants();
			list.Set(row, 0, i);
			list.Set(row, 1, mp.name);
			list.Set(row, 2, Format("%1n", mp.mass_factor));
			list.Set(row, 3, Format("%1n", mp.taste_factor));
			list.Set(row, 4, Format("%1n", mp.score));
		}
		list.SetCount(prof.presets.GetCount());
		list.SetSortColumn(3, true);
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectPreset();
	}
}

void MealPresetCtrl::SelectPreset() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[note_i];
	
	for(int i = 0; i < mp.variants.GetCount(); i++) {
		const MealPresetVariant& var = mp.variants[i];
		variants.Set(i, 0, var.name);
		variants.Set(i, 1, Format("%1n", var.mass_factor));
		variants.Set(i, 2, Format("%1n", var.taste_factor));
		variants.Set(i, 3, Format("%1n", var.score));
	}
	variants.SetCount(mp.variants.GetCount());
	if (!variants.IsCursor() && variants.GetCount())
		variants.SetCursor(0);
	
	const MealPresetVariant* var = NULL;
	int var_i = variants.GetCursor();
	if (var_i >= 0 && var_i < mp.variants.GetCount())
		var = &mp.variants[var_i];
	
	name.SetData(mp.name);
	instructions.SetData(mp.instructions);
	
	min_mass.SetCount(mp.ingredients.GetCount());
	max_mass.SetCount(mp.ingredients.GetCount());
	for(int i = 0; i < mp.ingredients.GetCount(); i++) {
		const MealIngredient& mi = mp.ingredients[i];
		const FoodDescription& d = db.food_descriptions[mi.db_food_no];
		EditDouble& min_edit = min_mass[i];
		EditDouble& max_edit = max_mass[i];
		min_edit.WhenAction.Clear();
		max_edit.WhenAction.Clear();
		
		inglist.Set(i, 0, d.long_desc);
		inglist.SetCtrl(i, 1, min_edit);
		inglist.Set(i, 1, mi.min_grams);
		inglist.SetCtrl(i, 2, max_edit);
		inglist.Set(i, 2, mi.max_grams);
		
		if (var && i < var->ingredients.GetCount())
			inglist.Set(i, 3, (int)var->ingredients[i].max_grams);
		else
			inglist.Set(i, 3, "");
		
		min_edit.WhenAction << THISBACK(PresetChanged);
		max_edit.WhenAction << THISBACK(PresetChanged);
	}
	inglist.SetCount(mp.ingredients.GetCount());
}

void MealPresetCtrl::PresetChanged() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	MealPreset& mp = prof.presets[note_i];
	
	mp.name = name.GetData();
	mp.instructions = instructions.GetData();
	
	for(int i = 0; i < mp.ingredients.GetCount(); i++) {
		MealIngredient& mi = mp.ingredients[i];
		mi.min_grams = min_mass[i].GetData();
		mi.max_grams = max_mass[i].GetData();
	}
	
	mp.UpdateFactors();
	
	list.Set(cursor, 1, mp.name);
	list.Set(cursor, 2, Format("%1n", mp.mass_factor));
	list.Set(cursor, 3, Format("%1n", mp.taste_factor));
	list.Set(cursor, 4, Format("%1n", mp.score));
}

void MealPresetCtrl::AddIngredient() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	MealPreset& mp = prof.presets[note_i];
	
	if (!food_list.IsCursor())
		return;
	int food_cursor = food_list.GetCursor();
	int food_i = food_list.Get(food_cursor, 0);
	
	for(int i = 0; i < mp.ingredients.GetCount(); i++)
		if (mp.ingredients[i].db_food_no == food_i)
			return;
	
	MealIngredient& mi = mp.ingredients.Add();
	mi.db_food_no = food_i;
	mi.max_grams = 1.0;
	
	SelectPreset();
	inglist.ScrollEnd();
}

void MealPresetCtrl::RemoveIngredient() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	MealPreset& mp = prof.presets[note_i];
	
	if (!inglist.IsCursor())
		return;
	int ing_cursor = inglist.GetCursor();
	
	if (ing_cursor >= 0 && ing_cursor < mp.ingredients.GetCount()) {
		mp.ingredients.Remove(ing_cursor);
		SelectPreset();
	}
}

void MealPresetCtrl::Add() {
	Profile& prof = GetProfile();
	MealPreset& mp = prof.presets.Add();
	mp.MakeUnique();
	mp.added = GetSysTime();
	mp.name = t_("Unnamed");
	Data();
	list.SetCursor(list.GetCount()-1);
}

void MealPresetCtrl::Remove() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	if (note_i >= 0 && note_i < prof.presets.GetCount()) {
		prof.presets.Remove(note_i);
		Data();
	}
}

void MealPresetCtrl::MakeVariants() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	if (note_i >= 0 && note_i < prof.presets.GetCount()) {
		prof.presets[note_i].MakeVariants();
		SelectPreset();
	}
}

void MealPresetCtrl::Filter() {
	Database& db = DB();
	
	thread_local static Vector<int> list;
	int view = list_sources.GetIndex();
	if (view == 0) {
		list.SetCount(db.food_descriptions.GetCount());
		for(int i = 0; i < list.GetCount(); i++)
			list[i] = i;
	}
	else {
		list.SetCount(db.used_foods.GetCount());
		for(int i = 0; i < list.GetCount(); i++)
			list[i] = db.used_foods[i];
	}
	
	String str = filter.GetData();
	if (str.GetCount()) {
		Vector<String> words = Split(ToLower(str), "&");
		
		for(int i = 0; i < words.GetCount(); i++) {
			String w = words[i];
			thread_local static Vector<int> rem_list;
			rem_list.SetCount(0);
			for(int j = 0; j < list.GetCount(); j++) {
				const FoodDescription& d = db.food_descriptions[list[j]];
				String lower = ToLower(d.long_desc);
				int k = lower.Find(w);
				if (k < 0)
					rem_list.Add(j);
			}
			list.Remove(rem_list);
		}
	}
	
	for(int i = 0; i < list.GetCount(); i++) {
		int j = list[i];
		const FoodDescription& d = db.food_descriptions[j];
		food_list.Set(i, 0, j);
		food_list.Set(i, 1, d.long_desc);
	}
	food_list.SetCount(list.GetCount());
	
	if (!food_list.IsCursor() && food_list.GetCount())
		food_list.SetCursor(0);
	
}

void MealPresetCtrl::AddByNutritionGroup(int group) {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.Get(cursor, 0);
	
	Profile& prof = GetProfile();
	if (note_i >= 0 && note_i < prof.presets.GetCount()) {
		Index<int> to_add;
		for(const NutritionRecommendation& recom : db.nutr_recom) {
			if (recom.group != group || recom.value == 0.0)
				continue;
			
			
			VectorMap<int, double> food_values;
			for(int i = 0; i < food_list.GetCount(); i++) {
				int fd_i = food_list.Get(i, 0);
				const FoodDescription& d = db.food_descriptions[fd_i];
				for (const NutritionInfo& ni : d.nutr) {
					if (ni.nutr_no == recom.nutr_no) {
						if (d.long_desc.Find("formula,") >= 0 ||
							d.long_desc.Find("palm") >= 0 || // not sold separately
							d.long_desc.Find("Shortening") >= 0 || // not everywhere
							d.long_desc.Find("skin,") >= 0 || // not everywhere
							d.long_desc.Find("Lamb") >= 0 || // very expensive
							d.long_desc.Find("DONALD") >= 0 || // very expensive
							d.long_desc.Find("New Zeal") >= 0 || // very expensive
							d.long_desc.Find("beluga") >= 0 || // very rare
							d.long_desc.Find("industrial") >= 0) // seems bad
						{
							
						}
						else {
							food_values.Add(fd_i, ni.nutr_value);
						}
						break;
					}
				}
			}
			SortByValue(food_values, StdGreater<double>());
			for(int i = 0; i < food_values.GetCount() && i < 3; i++)
				to_add.FindAdd(food_values.GetKey(i));
		}
		SortIndex(to_add, StdLess<int>());
		MealPreset& mp = prof.presets[note_i];
		for(int i = 0; i < to_add.GetCount(); i++) {
			int fd_i = to_add[i];
			if (mp.FindIngredient(fd_i) < 0) {
				MealIngredient& mi = mp.ingredients.Add();
				mi.db_food_no = fd_i;
				mi.min_grams = 0;
				mi.max_grams = 100;
			}
		}
		SelectPreset();
		PresetChanged();
	}
}

void MealPresetCtrl::AddVitamins() {
	AddByNutritionGroup(VITAMIN);
}

void MealPresetCtrl::AddMinerals() {
	AddByNutritionGroup(MINERAL);
}

void MealPresetCtrl::AddAminoAcids() {
	AddByNutritionGroup(AMINOACID);
}

void MealPresetCtrl::AddFattyAcids() {
	AddByNutritionGroup(FATTYACID);
}

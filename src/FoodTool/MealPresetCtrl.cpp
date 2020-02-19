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
	list.ColumnWidths("3 1 1");
	list.WhenAction = THISBACK(SelectPreset);
	list.WhenLeftClick = THISBACK(SelectPreset);
	
	inglist.AddColumn("Name");
	inglist.AddColumn("Min (g)");
	inglist.AddColumn("Max (g)");
	inglist.ColumnWidths("2 1 1");
	
	food_list.AddIndex();
	food_list.AddColumn("Long description");
	
	name <<= THISBACK(PresetChanged);
	instructions <<= THISBACK(PresetChanged);
	
	add <<= THISBACK(Add);
	remove <<= THISBACK(Remove);
	add_ingredient <<= THISBACK(AddIngredient);
	remove_ingredient <<= THISBACK(RemoveIngredient);
	filter.WhenEnter << THISBACK(Filter);
	
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
			const MealPreset& mp = prof.presets[i];
			list.Set(row, 0, i);
			list.Set(row, 1, mp.name);
			list.Set(row, 2, mp.mass_factor);
			list.Set(row, 3, mp.taste_factor);
		}
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
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[note_i];
	
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
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	MealPreset& mp = prof.presets[note_i];
	
	mp.name = name.GetData();
	mp.instructions = instructions.GetData();
	
	for(int i = 0; i < mp.ingredients.GetCount(); i++) {
		MealIngredient& mi = mp.ingredients[i];
		mi.min_grams = min_mass[i].GetData();
		mi.max_grams = max_mass[i].GetData();
	}
	
	list.Set(cursor, 1, mp.name);
}

void MealPresetCtrl::AddIngredient() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
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
	
	SelectPreset();
	inglist.ScrollEnd();
}

void MealPresetCtrl::RemoveIngredient() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
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
	mp.added = GetSysTime();
	mp.name = t_("Unnamed");
	Data();
}

void MealPresetCtrl::Remove() {
	if (!list.IsCursor())
		return;
	
	const Database& db = DB();
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	if (note_i >= 0 && note_i < prof.presets.GetCount()) {
		prof.presets.Remove(note_i);
		Data();
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

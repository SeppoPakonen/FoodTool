#include "FoodTool.h"


DatabaseCtrl::DatabaseCtrl() {
	CtrlLayout(*this);
	
	list_sources.Add(t_("All foods"));
	list_sources.Add(t_("Used foods"));
	list_sources.SetIndex(1);
	
	food_list.AddIndex();
	food_list.AddColumn(t_("Long description"));
	
	Database& db = DB();
	for(int i = 0; i < db.food_groups.GetCount(); i++)
		food_group.Add(db.food_groups[i].name);
	food_group.SetIndex(0);
	
	for(int i = 0; i < db.nutr_types.GetCount(); i++)
		nutr_type.Add(db.nutr_types[i].nutr_desc);
	
	nutr_list.AddColumn(t_("Key"));
	nutr_list.AddColumn(t_("Value"));
	nutr_list.ColumnWidths("2 1");
	
	filter.WhenEnter = THISBACK(Filter);
	do_filter <<= THISBACK(Filter);
	list_sources <<= THISBACK(Data);
	food_list <<= THISBACK(SelectFood);
	switch_used <<= THISBACK(ToggleUsed);
	add_food <<= THISBACK(AddFood);
	
	long_desc.WhenEnter << THISBACK(DumpDebug);
	
	food_group <<= THISBACK(UpdateFood);
	long_desc <<= THISBACK(UpdateFood);
	short_desc <<= THISBACK(UpdateFood);
	company <<= THISBACK(UpdateFood);
	manufacturer <<= THISBACK(UpdateFood);
	survey <<= THISBACK(UpdateFood);
	inedible <<= THISBACK(UpdateFood);
	inedible_perc <<= THISBACK(UpdateFood);
	sci_name <<= THISBACK(UpdateFood);
	n_factor <<= THISBACK(UpdateFood);
	prot_factor <<= THISBACK(UpdateFood);
	fat_factor <<= THISBACK(UpdateFood);
	carb_factor <<= THISBACK(UpdateFood);
	nutr_type <<= THISBACK(UpdateFood);
	nutr_value <<= THISBACK(UpdateFood);
	
	nutr_list <<= THISBACK(SelectNutrition);
	add_nutr <<= THISBACK(AddNutrition);
	
}

void DatabaseCtrl::Data() {
	Filter();
}

void DatabaseCtrl::Filter() {
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
	
	SelectFood();
}

void DatabaseCtrl::SelectFood() {
	if (!food_list.IsCursor()) return;
	selected_food = food_list.Get(food_list.GetCursor(), 0);
	selected_nutr = -1;
	
	Database& db = DB();
	const FoodDescription& d = db.food_descriptions[selected_food];
	int fg_i = max(0, db.food_groups.Find(d.food_group_key));
	food_group.SetIndex(fg_i);
	
	long_desc.SetData(d.long_desc);
	short_desc.SetData(d.short_desc);
	company.SetData(d.company_name);
	manufacturer.SetData(d.manufacturer_name);
	survey.SetData(d.survey);
	inedible.SetData(d.ref_desc);
	inedible_perc.SetData(d.refuse);
	sci_name.SetData(d.sci_name);
	n_factor.SetData(d.n_factor);
	prot_factor.SetData(d.pro_factor);
	fat_factor.SetData(d.fat_factor);
	carb_factor.SetData(d.cho_factor);
	soak.Set(d.require_soaking);
	
	NutrientData();
}

void DatabaseCtrl::NutrientData() {
	Database& db = DB();
	const FoodDescription& d = db.food_descriptions[selected_food];
	
	for(int i = 0; i < d.nutr.GetCount(); i++) {
		const NutritionInfo& n = d.nutr[i];
		const NutritionType& t = db.nutr_types[n.nutr_no];
		nutr_list.Set(i, 0, t.nutr_desc);
		nutr_list.Set(i, 1, Format("%2n", n.nutr_value) + t.units);
	}
	nutr_list.SetCount(d.nutr.GetCount());
	
	if (!nutr_list.IsCursor() && nutr_list.GetCount())
		nutr_list.SetCursor(0);
	
	SelectNutrition();
}

void DatabaseCtrl::SelectNutrition() {
	if (!nutr_list.IsCursor()) return;
	selected_nutr = nutr_list.GetCursor();
	
	Database& db = DB();
	const FoodDescription& d = db.food_descriptions[selected_food];
	const NutritionInfo& n = d.nutr[selected_nutr];
	const NutritionType& t = db.nutr_types[n.nutr_no];
	nutr_type.SetIndex(n.nutr_no);
	nutr_value.SetData(n.nutr_value);
}

void DatabaseCtrl::ToggleUsed() {
	if (!food_list.IsCursor()) return;
	
	Database& db = DB();
	int food_i = food_list.Get(food_list.GetCursor(), 0);
	int view = list_sources.GetIndex();
	if (view == 0) {
		db.used_foods.FindAdd(food_i);
	}
	else {
		db.used_foods.RemoveKey(food_i);
		Data();
	}
	
}

void DatabaseCtrl::AddFood() {
	filter.Clear();
	list_sources.SetIndex(0);
	
	String key;
	for(int i = 0; i < 8; i++)
		key.Cat('A' + Random('Z' - 'A' + 1));
	
	Database& db = DB();
	FoodDescription& d = db.food_descriptions.Add(key);
	d.is_user_added = true;
	
	Data();
	
	food_list.SetCursor(food_list.GetCount()-1);
	SelectFood();
}

void DatabaseCtrl::UpdateFood() {
	if (selected_food < 0) return;
	
	Database& db = DB();
	FoodDescription& d = db.food_descriptions[selected_food];
	d.food_group_key = db.food_groups.GetKey(food_group.GetIndex());
	
	d.long_desc = long_desc.GetData();
	d.short_desc = short_desc.GetData();
	d.company_name = company.GetData();
	d.manufacturer_name = manufacturer.GetData();
	d.survey = survey.GetData();
	d.ref_desc = inedible.GetData();
	d.refuse = inedible_perc.GetData();
	d.sci_name = sci_name.GetData();
	d.n_factor = (double)n_factor.GetData();
	d.pro_factor = (double)prot_factor.GetData();
	d.fat_factor = (double)fat_factor.GetData();
	d.cho_factor = (double)carb_factor.GetData();
	d.require_soaking = soak.Get();
	
	if (selected_nutr >= 0) {
		NutritionInfo& n = d.nutr[selected_nutr];
		n.nutr_no = nutr_type.GetIndex();
		n.nutr_value = (double)nutr_value.GetData();
		
		NutrientData();
	}
}

void DatabaseCtrl::AddNutrition() {
	if (selected_food < 0) return;
	
	Database& db = DB();
	FoodDescription& d = db.food_descriptions[selected_food];
	NutritionInfo& n = d.nutr.Add();
	n.is_user_added = true;
	
	NutrientData();
	
	nutr_list.SetCursor(nutr_list.GetCount()-1);
	SelectNutrition();
}

String DatabaseCtrl::GetModificationCppCode() {
	String out;
	Database& db = DB();
	
	for(int i = 0; i < db.food_descriptions.GetCount(); i++) {
		const FoodDescription& f = db.food_descriptions[i];
		if (f.is_user_added) {
			String s;
			s	<< "AddFood("
				<< "\"" << f.food_group_key << "\", "
				<< "\"" << f.long_desc << "\", "
				<< "\"" << f.short_desc << "\", "
				<< "\"" << f.company_name << "\", "
				<< "\"" << f.manufacturer_name << "\", "
				<< "\"" << f.survey << "\", "
				<< "\"" << f.ref_desc << "\", "
				<< f.refuse << ", "
				<< "\"" << f.sci_name << "\", "
				<< f.n_factor << ", "
				<< f.pro_factor << ", "
				<< f.fat_factor << ", "
				<< f.cho_factor << ")";
			
			for(int j = 0; j < f.nutr.GetCount(); j++) {
				const NutritionInfo& n = f.nutr[j];
				s << "\n\t.AddNutrition(" << (int)n.nutr_no << ", " << n.nutr_value << ", " << n.std_error << ")";
			}
			
			out << s << ";\n";
		}
		else {
			bool has_user_nutr = false;
			for(int j = 0; j < f.nutr.GetCount(); j++) {
				const NutritionInfo& n = f.nutr[j];
				if (n.is_user_added)
					has_user_nutr = true;
			}
			
			if (has_user_nutr || f.require_soaking) {
				String s;
				s	<< "food_descriptions[" << i << "]";
				
				if (f.require_soaking)
					s << "\n\t.RequireSoaking()";
				
				for(int j = 0; j < f.nutr.GetCount(); j++) {
					const NutritionInfo& n = f.nutr[j];
					if (n.is_user_added)
						s << "\n\t.AddNutrition(" << (int)n.nutr_no << ", " << n.nutr_value << ", " << n.std_error << ")";
				}
				
				out << s << ";\n";
			}
		}
	}
	
	for(int i = 0; i < db.used_foods.GetCount(); i++) {
		out << "used_foods.Add(" << db.used_foods[i] << ");\n";
	}
	return out;
}

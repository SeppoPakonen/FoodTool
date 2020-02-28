#include "FoodCtrl.h"

FoodInstructionCtrl::FoodInstructionCtrl() {
	Add(hsplit.SizePos());
	hsplit.Horz() << vsplit0 << vsplit1;
	hsplit.SetPos(3333);
	vsplit0.Vert() <<    list << meallist;
	vsplit1.Vert() << inglist << nutrlist;
	
	list.AddIndex();
	list.AddColumn(t_("Date"));
	list.AddColumn(t_("Variant"));
	list <<= THISBACK(SelectDate);
	
	meallist.AddIndex();
	meallist.AddIndex();
	meallist.AddColumn("#");
	meallist.AddColumn(t_("Name"));
	meallist.AddColumn(t_("Grams"));
	meallist.ColumnWidths("1 5 1");
	meallist <<= THISBACK(SelectMeal);
	
	inglist.AddIndex();
	inglist.AddIndex();
	inglist.AddColumn(t_("Ingredient"));
	inglist.AddColumn(t_("Mass (g)"));
	inglist.AddColumn(t_("Energy"));
	inglist.AddColumn(t_("Protein"));
	inglist.AddColumn(t_("Carbs"));
	inglist.AddColumn(t_("Fat"));
	inglist.ColumnWidths("4 1 1 1 1");
	inglist <<= THISBACK(SelectIngredient);
	
	nutrlist.AddColumn(t_("Key"));
	nutrlist.AddColumn(t_("Value"));
	
}

void FoodInstructionCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(hsplit, 3333);
	
	if (prof.storage.days.GetCount() != list.GetCount()) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.storage.days.GetCount(); i++) {
			int row = prof.storage.days.GetCount() - 1 - i;
			const FoodDay& day = prof.storage.days[i];
			const DailyPlan& plan = prof.planned_daily[i];
			String txt = Format("%d.%d.%d", (int)day.date.day, (int)day.date.month, (int)day.date.year);
			
			list.Set(row, 0, (int)plan.variant_type);
			if (day.date == today)
				list.Set(row, 1, AttrText(txt).Paper(Color(28, 85, 0)).Ink(White()));
			else
				list.Set(row, 1, txt);
			list.Set(row, 2, GetVariantString(plan.variant_type));
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectDate();
	}
}

void FoodInstructionCtrl::SelectDate() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int day_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const FoodDay& day = prof.storage.days[day_i];
	
	VectorMap<int, int> mealtype_grams;
	for(int i = 0; i < day.meals.GetCount(); i++) {
		const Meal& m = day.meals[i];
		int j = prof.FindMealPreset(m.key);
		if (j >= 0)
			mealtype_grams.GetAdd(j, 0) += m.grams;
	}
	
	for(int i = 0; i < mealtype_grams.GetCount(); i++) {
		int preset_i = mealtype_grams.GetKey(i);
		int grams = mealtype_grams[i];
		const MealPreset& mp = prof.presets[preset_i];
		meallist.Set(i, 0, preset_i);
		meallist.Set(i, 1, grams);
		meallist.Set(i, 2, i+1);
		meallist.Set(i, 3, mp.name);
		meallist.Set(i, 4, grams);
	}
	meallist.SetCount(mealtype_grams.GetCount());
	
	if (!meallist.IsCursor() && meallist.GetCount())
		meallist.SetCursor(0);
	
	SelectMeal();
}

void FoodInstructionCtrl::SelectMeal() {
	if (!list.IsCursor() || !meallist.IsCursor())
		return;
	int cursor = list.GetCursor();
	int variant = list.Get(cursor, 0);
	int mcursor = meallist.GetCursor();
	int preset_i = meallist.Get(mcursor, 0);
	int grams = meallist.Get(mcursor, 1);
	
	const Database& db = DB();
	const Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[preset_i];
	const MealPresetVariant& var = mp.variants[variant];
	
	Ingredient ing;
	var.GetNutritions(ing);
	double mul = (double)grams / ing.grams;
	ing.ChangeGrams(grams);
	
	int row = 0;
	
	for(int i = 0; i < var.ingredients.GetCount(); i++) {
		const MealIngredient& mi = var.ingredients[i];
		const FoodDescription& d = db.food_descriptions[mi.db_food_no];
		int grams = mi.max_grams * mul;
		if (grams > 0) {
			Ingredient ing;
			ing.Set(grams, d);
			inglist.Set(row, 0, mi.db_food_no);
			inglist.Set(row, 1, grams);
			inglist.Set(row, 2, d.long_desc);
			inglist.Set(row, 3, grams);
			inglist.Set(row, 4, Format("%2n", ing.nutr[KCAL]));
			inglist.Set(row, 5, Format("%2n", ing.nutr[PROT]));
			inglist.Set(row, 6, Format("%2n", ing.nutr[CARB]));
			inglist.Set(row, 7, Format("%2n", ing.nutr[FAT]));
			row++;
		}
	}
	
	ing.ChangeGrams(grams);
	inglist.Set(row, 0, -1);
	inglist.Set(row, 1, -1);
	inglist.Set(row, 2, AttrText(t_("Total")).NormalPaper(Color(204, 206, 255)));
	inglist.Set(row, 3, ing.grams);
	inglist.Set(row, 4, Format("%2n", ing.nutr[KCAL]));
	inglist.Set(row, 5, Format("%2n", ing.nutr[PROT]));
	inglist.Set(row, 6, Format("%2n", ing.nutr[CARB]));
	inglist.Set(row, 7, Format("%2n", ing.nutr[FAT]));
	row++;
	
	inglist.SetCount(row);
	
	if (!inglist.IsCursor() && inglist.GetCount())
		inglist.SetCursor(0);
	
	SelectIngredient();
}

void FoodInstructionCtrl::SelectIngredient() {
	if (!list.IsCursor() || !meallist.IsCursor() || !inglist.IsCursor())
		return;
	int icursor = inglist.GetCursor();
	int db_no;
	
	int cursor = list.GetCursor();
	int variant = list.Get(cursor, 0);
	int mcursor = meallist.GetCursor();
	int preset_i = meallist.Get(mcursor, 0);
	int grams = meallist.Get(mcursor, 1);
	
	const Database& db = DB();
	const Profile& prof = GetProfile();
	const MealPreset& mp = prof.presets[preset_i];
	const MealPresetVariant& var = mp.variants[variant];
	
	if (icursor >= 0) {
		Ingredient ing;
		db_no = inglist.Get(icursor, 0);
		if (db_no >= 0) {
			const Database& db = DB();
			const FoodDescription& d = db.food_descriptions[db_no];
			int grams = inglist.Get(icursor, 1);
			
			ing.Set(grams, d);
		}
		else {
			var.GetNutritions(ing);
			ing.ChangeGrams(grams);
		}
		int row = 0;
		nutrlist.Set(row, 0, t_("Mass"));
		nutrlist.Set(row++, 1, ing.grams);
		for(int i = 0; i < ing.nutr.GetCount(); i++) {
			double value = ing.nutr[i];
			if (value) {
				nutrlist.Set(row, 0, db.nutr_types[i].nutr_desc);
				nutrlist.Set(row++, 1, Format("%4n", value));
			}
		}
		nutrlist.SetCount(row);
	}
	else nutrlist.SetCount(0);
}

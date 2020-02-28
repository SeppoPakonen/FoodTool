#include "FoodCtrl.h"


DailyNutritionsCtrl::DailyNutritionsCtrl() {
	Add(hsplit.SizePos());
	hsplit.Horz() << list << vsplit;
	hsplit.SetPos(3333);
	vsplit.Vert() << srclist << nutrlist;
	vsplit.SetPos(2000);
	
	list.AddIndex();
	list.AddIndex();
	list.AddColumn(t_("Date"));
	list.AddColumn(t_("Variant"));
	list.AddColumn(t_("Target kcal"));
	list.AddColumn(t_("Target weight"));
	list.AddColumn(t_("Target protein"));
	list.ColumnWidths("2 2 1 1 1");
	list <<= THISBACK(SelectDate);
	
	srclist.AddColumn(t_("Ingredient"));
	srclist.AddColumn(t_("Mass (g)"));
	srclist.AddColumn(t_("Energy"));
	srclist.AddColumn(t_("Protein"));
	srclist.AddColumn(t_("Carbs"));
	srclist.AddColumn(t_("Fat"));
	srclist.ColumnWidths("4 1 1 1 1");
	srclist <<= THISBACK(SelectSource);
	
	nutrlist.AddColumn(t_("Key"));
	nutrlist.AddColumn(t_("Value"));
	nutrlist.AddColumn(t_("Recommended"));
	nutrlist.AddColumn(t_("\% of recommended"));
	nutrlist.ColumnWidths("3 1 1 1");
}

void DailyNutritionsCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(hsplit, 2000);
	
	if (prof.storage.days.GetCount() != list.GetCount()) {
		Date today = GetSysTime();
		for(int i = 0; i < prof.storage.days.GetCount(); i++) {
			int row = prof.storage.days.GetCount() - 1 - i;
			const FoodDay& day = prof.storage.days[i];
			const DailyPlan& plan = prof.planned_daily[i];
			String txt = Format("%d.%d.%d", (int)day.date.day, (int)day.date.month, (int)day.date.year);
			
			list.Set(row, 0, i);
			list.Set(row, 1, (int)plan.variant_type);
			if (day.date == today)
				list.Set(row, 2, AttrText(txt).Paper(Color(28, 85, 0)).Ink(White()));
			else
				list.Set(row, 2, txt);
			list.Set(row, 3, GetVariantString(plan.variant_type));
			list.Set(row, 4, (int)plan.food.nutr[KCAL]);
			list.Set(row, 5, Format("%1n", plan.weight));
			list.Set(row, 6, Format("%1n", plan.food.nutr[PROT]));
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectDate();
	}
}

void DailyNutritionsCtrl::SelectDate() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int day_i = list.Get(cursor, 0);
	int variant = list.Get(cursor, 1);
	
	Profile& prof = GetProfile();
	const FoodDay& day = prof.storage.days[day_i];
	
	src.Clear();
	src.Add(t_("Supplements"), day.supplement_sum);
	src.Add(t_("Food"), day.food_sum);
	src.Add(t_("Total"), day.total_sum);
	
	for(int i = 0; i < src.GetCount(); i++) {
		const Ingredient& ing = src[i];
		srclist.Set(i, 0, src.GetKey(i));
		srclist.Set(i, 1, Format("%2n", ing.grams));
		srclist.Set(i, 2, Format("%2n", ing.nutr[KCAL]));
		srclist.Set(i, 3, Format("%2n", ing.nutr[PROT]));
		srclist.Set(i, 4, Format("%2n", ing.nutr[CARB]));
		srclist.Set(i, 5, Format("%2n", ing.nutr[FAT]));
	}
	srclist.SetCount(src.GetCount());
	
	if (!srclist.IsCursor() && srclist.GetCount())
		srclist.SetCursor(0);
	
	SelectSource();
}

void DailyNutritionsCtrl::SelectSource() {
	if (!list.IsCursor() || !srclist.IsCursor())
		return;
	
	int daycursor = list.GetCursor();
	int day_i = list.Get(daycursor, 0);
	int cursor = srclist.GetCursor();
	double weight = GetProfile().planned_daily[day_i].weight;
	
	if (cursor >= 0) {
		const Database& db = DB();
		Ingredient& ing = src[cursor];
		VectorMap<int, double> values;
		for(int i = 0; i < ing.nutr.GetCount(); i++) {
			double value = ing.nutr[i];
			if (value)
				values.Add(i, value);
		}
		
		int row = 0;
		nutrlist.Set(row, 0, t_("Mass"));
		nutrlist.Set(row++, 1, Format("%2n", ing.grams));
		for(const NutritionRecommendation& recom : db.nutr_recom) {
			double value = values.Get(recom.nutr_no, 0);
			if (value > 0.0) {
				double recom_value = recom.GetValue(weight);
				const NutritionType& nt = db.nutr_types[recom.nutr_no];
				nutrlist.Set(row, 0, nt.nutr_desc);
				nutrlist.Set(row, 1, Format("%2n%s", value, nt.units));
				nutrlist.Set(row, 2, Format("%2n%s", recom_value, nt.units));
				nutrlist.Set(row, 3, Format("%1n", value / recom_value * 100));
				row++;
			}
		}
		nutrlist.SetCount(row);
	}
	else nutrlist.SetCount(0);
}


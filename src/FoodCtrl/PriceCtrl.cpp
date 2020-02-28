#include "FoodCtrl.h"


PriceCtrl::PriceCtrl() {
	CtrlLayout(history);
	
	ParentCtrl::Add(hsplit.SizePos());
	hsplit.Horz() << foodlist << history;
	
	foodlist.AddIndex();
	foodlist.AddColumn(t_("Food name"));
	foodlist.AddColumn(t_("Latest quote age"));
	foodlist.AddColumn(t_("Latest price"));
	foodlist.AddColumn(t_("Used by meals"));
	foodlist.ColumnWidths("3 1 1 2");
	foodlist <<= THISBACK(SelectFood);
	
	history.list.AddIndex();
	history.list.AddColumn(t_("Time"));
	history.list.AddColumn(t_("Grams"));
	history.list.AddColumn(t_("Servings"));
	history.list.AddColumn(t_("Batch Size"));
	history.list.AddColumn(t_("Price (EUR)"));
	history.list.AddColumn(t_("Shop"));
	history.add <<= THISBACK(Add);
	
}

void PriceCtrl::Data(bool force) {
	const Database& db = DB();
	Profile& prof = GetProfile();
	Date today = GetSysTime();
	
	SplitterHotfix(hsplit, 5000);
	
	VectorMap<int, String> meal_str;
	for(const MealPreset& mp : prof.presets) {
		for(const MealIngredient& mi : mp.ingredients) {
			String& s = meal_str.GetAdd(mi.db_food_no);
			if (s.GetCount()) s << ", ";
			s << mp.name;
		}
	}
	
	if (foodlist.GetCount() != db.used_foods.GetCount() || force) {
		for(int i = 0; i < db.used_foods.GetCount(); i++) {
			int db_no = db.used_foods[i];
			const FoodDescription& d = db.food_descriptions[db_no];
			
			foodlist.Set(i, 0, db_no);
			foodlist.Set(i, 1, d.long_desc);
			
			const Vector<FoodPriceQuote>& quotes = prof.price.history.GetAdd(db_no);
			if (quotes.GetCount()) {
				const FoodPriceQuote& quote = quotes.Top();
				Date date = quote.time;
				int age = today.Get() - date.Get();
				
				foodlist.Set(i, 2, Format(t_("%d days"), age));
				foodlist.Set(i, 3, quote.GetPriceString());
			}
			else {
				foodlist.Set(i, 2, "");
				foodlist.Set(i, 3, "");
			}
			foodlist.Set(i, 4, meal_str.Get(db_no, ""));
		}
		foodlist.SetCount(db.used_foods.GetCount());
		foodlist.SetSortColumn(0);
		
		if (!foodlist.IsCursor())
			foodlist.SetCursor(0);
		SelectFood();
	}
}

void PriceCtrl::SelectFood() {
	if (!foodlist.IsCursor())
		return;
	
	Profile& prof = GetProfile();
	int cursor = foodlist.GetCursor();
	int db_no = foodlist.Get(cursor, 0);
	const Vector<FoodPriceQuote>& quotes = prof.price.history.Get(db_no);
	
	int resize = max(grams.GetCount(), quotes.GetCount());
	grams.SetCount(resize);
	servings.SetCount(resize);
	batch.SetCount(resize);
	price.SetCount(resize);
	shop.SetCount(resize);
	for(int i = 0; i < quotes.GetCount(); i++) {
		const FoodPriceQuote& q = quotes[i];
		int row = quotes.GetCount() - 1 - i;
		
		EditIntSpin& grams = this->grams[i];
		EditIntSpin& servings = this->servings[i];
		EditIntSpin& batch = this->batch[i];
		EditDoubleSpin& price = this->price[i];
		EditString& shop = this->shop[i];
		grams    <<= THISBACK1(ValueChanged, i);
		servings <<= THISBACK1(ValueChanged, i);
		batch    <<= THISBACK1(ValueChanged, i);
		price    <<= THISBACK1(ValueChanged, i);
		shop     <<= THISBACK1(ValueChanged, i);
		
		history.list.Set(row, 0, i);
		history.list.Set(row, 1, q.time);
		history.list.SetCtrl(row, 1, grams);
		history.list.SetCtrl(row, 2, servings);
		history.list.SetCtrl(row, 3, batch);
		history.list.SetCtrl(row, 4, price);
		history.list.SetCtrl(row, 5, shop);
		history.list.Set(row, 2, q.grams);
		history.list.Set(row, 3, q.servings);
		history.list.Set(row, 4, q.serving_batch);
		history.list.Set(row, 5, q.price);
		history.list.Set(row, 6, q.shop);
	}
	history.list.SetCount(quotes.GetCount());
}

void PriceCtrl::Add() {
	if (!foodlist.IsCursor())
		return;
	
	Profile& prof = GetProfile();
	int cursor = foodlist.GetCursor();
	int db_no = foodlist.Get(cursor, 0);
	Vector<FoodPriceQuote>& quotes = prof.price.history.Get(db_no);
	FoodPriceQuote& quote = quotes.Add();
	quote.time = GetSysTime();
	
	SelectFood();
	history.list.SetCursor(0);
}

void PriceCtrl::ValueChanged(int quote_i) {
	if (!foodlist.IsCursor())
		return;
	
	Profile& prof = GetProfile();
	int cursor = foodlist.GetCursor();
	int db_no = foodlist.Get(cursor, 0);
	Vector<FoodPriceQuote>& quotes = prof.price.history.Get(db_no);
	FoodPriceQuote& quote = quotes[quote_i];
	quote.grams = grams[quote_i].GetData();
	quote.servings = servings[quote_i].GetData();
	quote.serving_batch = batch[quote_i].GetData();
	quote.price = price[quote_i].GetData();
	quote.shop = shop[quote_i].GetData();
	
	foodlist.Set(cursor, 3, quote.GetPriceString());
}


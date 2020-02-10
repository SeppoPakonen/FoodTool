#include "FoodTool.h"

FoodStorage::FoodStorage() {
	int coco = 0.92 * 425;
	int oliv = 0.92 * 1000;
	
	AddFoodType("SALT",	9,1000,	"Salt",				0,		0,		0,		0,		100,	MINERAL);
	
	AddFoodType("COCO",	9,coco,	"Coconut oil",		900,	100,	0,		0,		0,		OIL,	5);
	AddFoodType("OLIV",	9,oliv,	"Olive oil",		900,	100,	0,		0,		0,		OIL,	1);
	
	AddFoodType("CR36",	7,500,	"Cream 36\%",		350,	36,		3,		0,		0.1,	MILK);
	
	AddFoodType("YO10",	8,1000,	"Yogurt 10\%",		124,	10,		4.4,	4.2,	0.12,	PROCESSED_MILK);
	AddFoodType("SMET",	8,350,	"Smetana",			204,	20,		3.2,	2.8,	0.1,	PROCESSED_MILK);
	AddFoodType("MOZA", 11,125,	"Mozarella",		225,	17,		1,		17,		0.6,	PROCESSED_MILK)
		.SetServing(125);
	AddFoodType("COTT", 11,400,	"Cottage cheese",	84,		2,		1.6,	15,		0.75,	PROCESSED_MILK);
	AddFoodType("CREM", 8,150,	"Creme fraiche",	190,	18,		3.5,	2.5,	0.1,	PROCESSED_MILK);
	AddFoodType("SOUR", 8,200,	"Sour cream",		139,	12,		4,		2.8,	0.1,	PROCESSED_MILK);
	AddFoodType("CHEE", 11,500,	"Grated cheese",	299,	23,		0,		23,		2.0,	PROCESSED_MILK);
	
	AddFoodType("CABB", 3,300,	"Cabbage",			21,		0.2,	3.4,	1.2,	0.05,	VEGETABLE);
	AddFoodType("BROC", 1,450,	"Broccoli",			34,		0.5,	3.5,	2.4,	0.03,	VEGETABLE);
	AddFoodType("CAUL", 1,450,	"Cauliflower",		20,		0.1,	2.7,	1.2,	0.1,	VEGETABLE);
	AddFoodType("PINA", 1,150,	"Spinach",			23,		0.5,	0.6,	2.8,	0.05,	VEGETABLE);
	AddFoodType("GOLI", 10,550,	"Green olives",		133,	14,		0,		0.7,	3.8,	VEGETABLE);
	
	AddFoodType("TOFU", 11,250,	"Tofu",				121,	7.2,	1.0,	12.7,	0.65,	PROCESSED_VEGETABLE);
	
	AddFoodType("EGGS", 4,1020,	"Egg",				144,	10.3,	0.3,	12.5,	0.34,	MEAT,	3)
				.SetServing(68);
	AddFoodType("COW17", 5,400,	"Cow meat 17\%",	227,	16.9,	0,		19.1,	0,		MEAT,	1);
	AddFoodType("MIX17", 5,700,	"Pig/Cow meat 17\%",221,	17,		0,		17.5,	0,		MEAT,	1);
	AddFoodType("PIG15", 5,400,	"Pig strip 15\%",	205,	15,		0,		18,		0.1,	MEAT,	1);
	AddFoodType("CHICK", 5,700,	"Chicken strip",	87,		1.2,	0,		18,		0.67,	MEAT,	3);
	AddFoodType("POLL", 2,400,	"Alaska Pollock",	75,		0.8,	0,		17,		0.3,	MEAT,	3)
		.SetServing(100);
	AddFoodType("SHRI", 2,200,	"Shrimp",			66,		0.7,	0,		15,		1.4,	MEAT,	3);
	
	AddFoodType("TURK", 6,400,	"Turkey sausage",	120,	7.8,	0.5,	11.9,	1.7,	PROCESSED_MEAT);
	AddFoodType("BACO", 6,280,	"Bacon",			277,	25,		0,		13,		2.5,	PROCESSED_MEAT);
	
	AddMealType("EGGBACON", "Eggs & Bacon", "SALT,EGGS,BACO", MORNING);
	AddMealType("FATMILK", "Fatty & Milky", "CR36,YO10,COCO,?SMET,?CREM,?SOUR", EARLY_DAY|SNACK, 5);
	AddMealType("CHEESE", "Cheesy", "?MOZA,?COTT,?CHEE", EARLY_DAY|SNACK, 2);
	AddMealType("OLIVE", "Olivy", "GOLI", EARLY_DAY|SNACK, 1);
	AddMealType("MEATVEGS", "Meat & Vegs",
		"SALT,"
		"OLIV,"
		"?SMET,?CREM,?SOUR,"
		"?CABB,?BROC,?CAUL,?PINA,"
		"?TOFU,?COW17,?MIX17,?PIG15,?CHICK,?POLL,?SHRI,?TURK", LUNCH);
}

FoodType& FoodStorage::AddFoodType(String code, int shop_order, int pkg_size, String name, float kcals, float fat, float carbs, float protein, float salt, int cat, int healthiness) {
	FoodType& t = food_types.Add(code);
	t.name = name;
	t.shop_order = shop_order;
	t.details.grams = pkg_size;
	t.details.kcals = kcals;
	t.details.fat = fat;
	t.details.carbs = carbs;
	t.details.protein = protein;
	t.details.salt = salt;
	t.cat = cat;
	t.healthiness = healthiness;
	return t;
}

MealType& FoodStorage::AddMealType(String code, String name, String food_str, int time_of_day, float frequency) {
	MealType& t = meal_types.Add(code);
	t.name = name;
	t.food_str = food_str;
	t.time_of_day = time_of_day;
	t.frequency = frequency;
	
	Vector<String> items = Split(food_str, ",");
	for(String item : items) {
		if (item[0] == '?')
			t.opt_foods.Add(item.Mid(1));
		else
			t.foods.Add(item);
	}
	return t;
}

void FoodStorage::Init(Date begin) {
	ASSERT(days.IsEmpty());
	FoodDay& first_day = days.Add();
	first_day.date = begin;
	first_day.is_shopping = true;
	first_day.total_consumed.Reset();
}

void FoodStorage::Update(const Vector<DailyPlan>& planned_daily) {
	ASSERT_(days.GetCount(), "Run Init first");
	while (!HasEnoughPreplanned())
		PlanWeek(planned_daily);
}

void FoodStorage::PlanWeek(const Vector<DailyPlan>& planned_daily) {
	Profile& prof = GetProfile();
	int shop_interval = prof.confs.Top().shop_interval;
	
	FoodDay& shopping_day = days.Top();
	ASSERT(shopping_day.is_shopping);
	
	Date d = shopping_day.date;
	d++;
	FoodDay* prev = &shopping_day;
	FoodQuantity total_usage;
	int zero_day_i = 0;
	
	FoodDetailed target_diff = prev->total_sum - prev->target_sum;
	
	for(int i = 0; i < shop_interval; i++) {
		int day_i = days.GetCount();
		
		if (!i) zero_day_i = days.GetCount();
		FoodDay& day = days.Add();
		day.date = d++;
		day.is_shopping = i == (shop_interval-1);
		day.used_food_amount <<= prev->used_food_amount;
		day.used_meal_amount <<= prev->used_meal_amount;
		day.total_consumed = prev->total_consumed;
		
		target_diff.grams = 0;
		target_diff.carbs = min(target_diff.carbs, 0.f);
		target_diff.salt = 0;
		day.target_sum = planned_daily[day_i].food;
		day.target_sum -= target_diff;
		day.target_sum.Limit(10, 1, 0);
		FoodDetailed orig_target_sum = day.target_sum;
		
		MakeMenu(day);
		
		target_diff = day.total_sum - orig_target_sum;
		day.total_consumed += day.total_sum;
		
		for(int j = 0; j < day.food_usage.GetCount(); j++) {
			String key = day.food_usage.GetKey(j);
			float usage = day.food_usage[j];
			total_usage.GetAdd(key, 0) += usage;
			day.used_food_amount.GetAdd(key, 0) += usage;
		}
		
		for(int j = 0; j < day.meals.GetCount(); j++)
			day.used_meal_amount.GetAdd(day.meals[j].key, 0)++;
		
		prev = &day;
	}
	
	FoodQuantity existing_food_grams;
	existing_food_grams <<= shopping_day.food_grams;
	for(int j = 0; j < shopping_day.food_usage.GetCount(); j++)
		existing_food_grams.Get(shopping_day.food_usage.GetKey(j)) -= shopping_day.food_usage[j];
	
	FoodQuantityInt& buy_amount = shopping_day.buy_amount;
	buy_amount.Clear();
	for(int i = 0; i < total_usage.GetCount(); i++) {
		String key = total_usage.GetKey(i);
		float usage = total_usage[i];
		float already = existing_food_grams.Get(key, 0);
		ASSERT(already >= 0.0 && usage >= 0.0);
		float must_buy = usage - already;
		int buy_count = 0;
		float bought_sum = 0;
		FoodType& t = food_types.Get(key);
		
		if (must_buy > 0) {
			do {
				buy_count++;
				bought_sum += t.details.grams;
			}
			while (bought_sum < must_buy);
			
			buy_amount.Add(key, buy_count);
		}
	}
	
	if (buy_amount.GetCount()) {
		FoodQuantitySorter sorter(this);
		SortByKey(buy_amount, sorter);
		DUMPM(buy_amount),
		
		shopping_day.shopping_list.Clear();
		for(int i = 0; i < buy_amount.GetCount(); i++) {
			String key = buy_amount.GetKey(i);
			int count = buy_amount[i];
			FoodType& t = food_types.Get(key);
			shopping_day.shopping_list << t.name << " *" << count << "\n";
		}
	}
	else
		shopping_day.shopping_list = "Nothing to buy at this time";
	
	prev = &shopping_day;
	for(int i = 0; i < shop_interval; i++) {
		FoodDay& day = days[zero_day_i + i];
		
		day.food_grams <<= prev->food_grams;
		for(int j = 0; j < prev->food_usage.GetCount(); j++)
			day.food_grams.Get(prev->food_usage.GetKey(j)) -= prev->food_usage[j];
		
		if (i == 0)
			AddFoodQuantity(shopping_day.buy_amount, day.food_grams);
		
		for(int j = 0; j < day.food_usage.GetCount(); j++) {
			String key = day.food_usage.GetKey(j);
			ASSERT(day.food_grams.Find(key) >= 0);
		}
		
		
		prev = &day;
	}
	ASSERT(days.Top().is_shopping && days.Top().food_grams.GetCount());
}

void FoodStorage::MakeMenu(FoodDay& d) {
	ASSERT(d.target_sum.fat >= 0 && d.target_sum.fat < 2000);
	ASSERT(d.target_sum.carbs >= 0 && d.target_sum.carbs < 2000);
	ASSERT(d.target_sum.protein >= 0 && d.target_sum.protein < 2000);
	d.meals.SetCount(4);
	
	float calorie_sum = 0.0;
	for(int i = 0; i < 4; i++) {
		int bit = 1 << i;
		Meal& m = d.meals[i];
		
		// These are just relative numbers in this point
		switch (i) {
			case 0: m.target_sum.kcals = 200; break;
			case 1: m.target_sum.kcals = 200; break;
			case 2: m.target_sum.kcals = 500; break;
			case 3: m.target_sum.kcals = 200; break;
		}
		calorie_sum += m.target_sum.kcals;
	}
	
	int food_item_count = 0;
	for(int i = 0; i < 4; i++) {
		int bit = 1 << i;
		Meal& m = d.meals[i];
		float perc = m.target_sum.kcals / calorie_sum;
		m.target_sum = d.target_sum;
		m.target_sum *= perc;
		
		
		// Select randomly
		int mtype_i = -1;
		if (Randomf() < 0.2) {
			int counter = Random(meal_types.GetCount());
			int j = 0;
			while (1) {
				MealType& t = meal_types[j];
				if (t.time_of_day & bit) {
					if (!counter) {
						mtype_i = j;
						break;
					}
					counter--;
				}
				j = (j + 1) % meal_types.GetCount();
			}
		}
		else {
			static thread_local Vector<int> allowed_meals;
			allowed_meals.SetCount(0);
			
			for(int j = 0; j < meal_types.GetCount(); j++) {
				MealType& t = meal_types[j];
				if (t.time_of_day & bit)
					allowed_meals.Add(j);
			}
			
			if (allowed_meals.GetCount() > 1) {
				MealQuantitySorter sorter(this, d);
				Sort(allowed_meals, sorter);
			}
			mtype_i = allowed_meals[0];
		}
		ASSERT(mtype_i >= 0);
		m.key = meal_types.GetKey(mtype_i);
		
		
		m.food.Clear();
		
		const MealType& t = meal_types.Get(m.key);
		for(int j = 0; j < t.foods.GetCount(); j++) {
			String key = t.foods[j];
			const FoodType& ft = food_types.Get(key);
			FoodDetailed& details = m.food.Add(key);
			details = ft.details;
			details.grams = 100;
		}
		
		if (t.opt_foods.GetCount()) {
			static thread_local VectorMap<String, double> usage;
			usage.Clear();
			for(int j = 0; j < t.opt_foods.GetCount(); j++) {
				String key = t.opt_foods[j];
				double food_usage = d.used_food_amount.Get(key, 0);
				usage.Add(key, food_usage);
			}
			SortByValue(usage, StdLess<double>());
		
			double max_usage = usage[0] * 1.25;
			for(int j = 0; j < t.opt_foods.GetCount(); j++) {
				String key = t.opt_foods[j];
				double food_usage = usage.Get(key);
				if (food_usage > max_usage)
					continue;
				const FoodType& ft = food_types.Get(key);
				FoodDetailed& details = m.food.Add(key);
				details = ft.details;
				details.grams = 100;
			}
		}
		
		food_item_count += m.food.GetCount();
	}
	
	
	
	Optimizer opt;
	opt.MinMax(0, 200);
	opt.SetMaxGenerations(100);
	opt.Init(food_item_count, 200);
	while (!opt.IsEnd()) {
		opt.Start();
		
		const Vector<double>& trial = opt.GetTrialSolution();
		d.SetMealGrams(trial, food_types);
		
		double invalid_sum = 0.0;
		for(int i = 0; i < d.meals.GetCount(); i++) {
			const Meal& m = d.meals[i];
			double kcal_diff = fabs(m.target_sum.kcals - m.food_sum.kcals);
			if (kcal_diff >= 100) {
				invalid_sum += kcal_diff;
			}
		}
		
		double energy;
		if (invalid_sum > 0.0)
			energy = -100000 * invalid_sum;
		else
			energy = d.GetOptimizerEnergy();
		
		//LOG(opt.GetRound() << ": " << energy);
		
		opt.Stop(energy);
	}
	
	const Vector<double>& best = opt.GetBestSolution();
	d.SetMealGrams(best, food_types, true);
	double energy = d.GetOptimizerEnergy();
	LOG("Final optimization result: " << energy);
	
	d.menu.Clear();
	d.food_usage.Clear();
	for(int i = 0; i < d.meals.GetCount(); i++) {
		const Meal& m = d.meals[i];
		
		switch (i) {
			case 0: d.menu << "Breakfast\n---------\n"; break;
			case 1: d.menu << "Early snack\n-----------\n"; break;
			case 2: d.menu << "Lunch\n-----\n"; break;
			case 3: d.menu << "Late snack\n----------\n"; break;
		}
		
		for(int j = 0; j < m.food.GetCount(); j++) {
			String key = m.food.GetKey(j);
			float grams = m.food[j].grams;
			ASSERT(grams >= 0.0);
			if (grams > 0.01) {
				d.food_usage.GetAdd(key, 0) += grams;
				
				const FoodType& t = food_types.Get(key);
				if (t.serving_grams > 0) {
					int servings = (int)((grams / (float)t.serving_grams) + 0.5);
					d.menu << t.name << ": " << servings << " pieces\n";
				}
				else {
					d.menu << t.name << ": " << Format("%1n", grams) << "g\n";
				}
			}
		}
		
		d.menu << "\n";
	}
}

void FoodDay::SetMealGrams(const Vector<double>& grams, const VectorMap<String, FoodType>& food_types, bool check) {
	int it = 0;
	total_sum.Reset();
	for(int i = 0; i < meals.GetCount(); i++) {
		Meal& m = meals[i];
		m.food_sum.Reset();
		for(int j = 0; j < m.food.GetCount(); j++) {
			String key = m.food.GetKey(j);
			const FoodType& t = food_types.Get(key);
			double g = grams[it++];
			if (t.serving_grams > 0) {
				int count = (g + t.serving_grams * 0.5) / t.serving_grams;
				g = count * t.serving_grams;
			}
			g = max(0.01, g);
			FoodDetailed& d = m.food[j];
			d.ChangeGrams(g);
			m.food_sum += d;
			if (check) {
				ASSERT(m.food_sum.fat >= 0 && m.food_sum.fat < 2000);
				ASSERT(m.food_sum.carbs >= 0 && m.food_sum.carbs < 2000);
				ASSERT(m.food_sum.protein >= 0 && m.food_sum.protein < 2000);
			}
		}
		total_sum += m.food_sum;
	}
	ASSERT(it == grams.GetCount());
}

double FoodDay::GetOptimizerEnergy() {
	// If weight loss mode
	if (1) {
		/*double abs_diff_sum =
			fabs(total_sum.grams - target_sum.grams) * 10 +
			fabs(total_sum.kcals - target_sum.kcals) * 10 +
			fabs(target_sum.salt - total_sum.salt) * 10 +
			fabs(total_sum.carbs - target_sum.carbs) +
			fabs(target_sum.fat - total_sum.fat) +
			fabs(target_sum.protein - total_sum.protein);*/
		
		double abs_diff_sum =
			fabs(total_sum.kcals - target_sum.kcals) + // ~100
			fabs(target_sum.salt - total_sum.salt) * 100 + // ~0.25
			fabs(total_sum.grams - target_sum.grams) * 0.1; // ~1000
		
		double carbs_loss = -(total_sum.carbs - target_sum.carbs) * 2; // ~7.8
		double fat_loss = -(total_sum.fat - target_sum.fat) * 2; // ~7.5
		double protein_loss = -(total_sum.protein - target_sum.protein) * 2; // ~40
		
		if (carbs_loss > 0)
			abs_diff_sum += carbs_loss;
		if (fat_loss > 0)
			abs_diff_sum += fat_loss;
		if (protein_loss > 0)
			abs_diff_sum += protein_loss;
			
		ASSERT(IsFin(abs_diff_sum));
		return -abs_diff_sum;
	}
}

void FoodStorage::AddFoodQuantity(const FoodQuantityInt& src, FoodQuantity& dst) {
	for(int i = 0; i < src.GetCount(); i++) {
		String code = src.GetKey(i);
		int count = src[i];
		double food_grams = food_types.Get(code).details.grams;
		ASSERT(food_grams > 0);
		dst.GetAdd(code, 0) += count * food_grams;
	}
}

bool FoodStorage::HasEnoughPreplanned() {
	Date today = GetSysTime();
	
	int today_i = -1;
	for(int i = days.GetCount()-1; i >= 0; i--) {
		FoodDay& day = days[i];
		if (day.date == today) {
			today_i = i;
			break;
		}
	}
	
	if (today_i < 0)
		return false;
	
	for(int i = today_i; i < days.GetCount(); i++) {
		FoodDay& day = days[i];
		if (day.is_shopping && i < days.GetCount()-1)
			return true;
	}
	
	return false;
}

Date FoodStorage::GetLastShopping() {
	Date today = GetSysTime();
	
	for(int i = days.GetCount()-1; i >= 0; i--) {
		if (days[i].date == today) {
			for(int j = i; j >= 0; j--) {
				if (days[j].is_shopping) {
					return days[j].date;
				}
			}
		}
	}
	
	return Time();
}

Date FoodStorage::GetNextShopping() {
	Date today = GetSysTime();
	
	for(int i = days.GetCount()-1; i >= 0; i--) {
		if (days[i].date == today) {
			for(int j = i+1; j < days.GetCount(); j++) {
				if (days[j].is_shopping) {
					return days[j].date;
				}
			}
		}
	}
	
	return Time();
}

String FoodStorage::GetTodaysMenu() {
	Date today = GetSysTime();
	
	for(int i = days.GetCount()-1; i >= 0; i--) {
		FoodDay& day = days[i];
		if (day.date == today)
			return day.menu;
	}
	
	return "<Error fetching today's menu>";
}

String FoodStorage::GetNextShoppingList() {
	Date today = GetSysTime();
	for(int i = days.GetCount()-2; i >= 0; i--) {
		FoodDay& day = days[i];
		if (day.date <= today)
			break;
		if (day.is_shopping)
			return day.shopping_list;
	}
	
	return "<Error fetching the next shopping list>";
}

bool FoodStorage::NeedShopping() {
	
	
	return false;
}

void FoodStorage::DoShop() {
	
}

void FoodStorage::ConsumeDay() {
	
}
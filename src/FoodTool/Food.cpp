#include "FoodTool.h"

FoodStorage::FoodStorage() {
	int coco = 0.92 * 425;
	int oliv = 0.92 * 1000;
	
	AddFoodType("COCO",	coco,	"Coconut oil",		900,	100,	0,		0,		0,		OIL,	5);
	AddFoodType("OLIV",	oliv,	"Olive oil",		900,	100,	0,		0,		0,		OIL,	1);
	
	AddFoodType("CR36",	500,	"Cream 36\%",		350,	36,		3,		0,		0.1,	MILK);
	
	AddFoodType("YO10",	1000,	"Yogurt 10\%",		124,	10,		4.4,	4.2,	0.12,	PROCESSED_MILK);
	AddFoodType("SMET",	350,	"Smetana",			204,	20,		3.2,	2.8,	0.1,	PROCESSED_MILK);
	AddFoodType("MOZA", 125,	"Mozarella",		225,	17,		1,		17,		0.6,	PROCESSED_MILK);
	AddFoodType("COTT", 400,	"Cottage cheese",	84,		2,		1.6,	15,		0.75,	PROCESSED_MILK);
	AddFoodType("CREM", 150,	"Creme fracihe",	190,	18,		3.5,	2.5,	0.1,	PROCESSED_MILK);
	AddFoodType("SOUR", 200,	"Sour cream",		139,	12,		4,		2.8,	0.1,	PROCESSED_MILK);
	AddFoodType("CHEE", 500,	"Grated cheese",	299,	23,		0,		23,		2.0,	PROCESSED_MILK);
	
	AddFoodType("CABB", 300,	"Cabbage",			21,		0.2,	3.4,	1.2,	0.05,	VEGETABLE);
	AddFoodType("BROC", 450,	"Broccoli",			34,		0.5,	3.5,	2.4,	0.03,	VEGETABLE);
	AddFoodType("CAUL", 450,	"Cauliflower",		20,		0.1,	2.7,	1.2,	0.1,	VEGETABLE);
	AddFoodType("PINA", 150,	"Spinach",			23,		0.5,	0.6,	2.8,	0.05,	VEGETABLE);
	AddFoodType("GOLI", 550,	"Green olives",		133,	14,		0,		0.7,	3.8,	VEGETABLE);
	
	AddFoodType("TOFU", 250,	"Tofu",				121,	7.2,	1.0,	12.7,	0.65,	PROCESSED_VEGETABLE);
	
	AddFoodType("EGGS", 1020,	"Egg",				144,	10.3,	0.3,	12.5,	0.34,	MEAT,	3)
				.SetServing(68);
	AddFoodType("COW17", 400,	"Cow meat 17\%",	227,	16.9,	0,		19.1,	0,		MEAT,	1);
	AddFoodType("MIX17", 700,	"Pig/Cow meat 17\%",221,	17,		0,		17.5,	0,		MEAT,	1);
	AddFoodType("PIG15", 400,	"Pig strip 15\%",	205,	15,		0,		18,		0.1,	MEAT,	1);
	AddFoodType("CHICK", 700,	"Chicken strip",	87,		1.2,	0,		18,		0.67,	MEAT,	3);
	AddFoodType("POLL", 400,	"Alaska Pollock",	75,		0.8,	0,		17,		0.3,	MEAT,	3);
	AddFoodType("SHRI", 200,	"Shrimp",			66,		0.7,	0,		15,		1.4,	MEAT,	3);
	
	AddFoodType("TURK", 400,	"Turkey sausage",	120,	7.8,	0.5,	11.9,	1.7,	PROCESSED_MEAT);
	AddFoodType("BACO", 280,	"Bacon",			277,	25,		0,		13,		2.5,	PROCESSED_MEAT);
	
	
	AddMealType("Eggs & Bacon", "EGGS,BACO", MORNING);
	AddMealType("Fatty & Milky", "CR36,YO10,COCO,?SMET,?CREM,?SOUR", EARLY_DAY|SNACK);
	AddMealType("Cheesy", "?MOZA,?COTT,?CHEE", EARLY_DAY|SNACK);
	AddMealType("Olivy", "GOLI", EARLY_DAY|SNACK);
	AddMealType("Meat & Vegs",
		"OLIV,"
		"?SMET,?CREM,?SOUR,"
		"?CABB,?BROC,?CAUL,?PINA,"
		"?TOFU,?COW17,?MIX17,?PIG15,?CHICK,?POLL,?SHRI,?TURK", LUNCH);
}

FoodType& FoodStorage::AddFoodType(String code, int pkg_size, String name, float kcals, float fat, float carbs, float protein, float salt, int cat, int healthiness) {
	FoodType& t = food_types.Add(code);
	t.name = name;
	t.pkg_size_grams = pkg_size;
	t.kcals = kcals;
	t.fat = fat;
	t.carbs = carbs;
	t.protein = protein;
	t.salt = salt;
	t.cat = cat;
	t.healthiness = healthiness;
	return t;
}

MealType& FoodStorage::AddMealType(String name, String food_str, int time_of_day) {
	MealType& t = meal_types.Add(name);
	t.name = name;
	t.food_str = food_str;
	t.time_of_day = time_of_day;
	return t;
}

void FoodStorage::Update() {
	
	// Generate from the beginning
	if (days.IsEmpty()) {
		FoodDay& first_day = days.Add();
		first_day.date = GetSysTime();
		first_day.is_shopping = true;
		PlanWeek();
	}
	else {
		while (!HasEnoughPreplanned())
			PlanWeek();
	}
}

void FoodStorage::PlanWeek() {
	FoodDay& shopping_day = days.Top();
	ASSERT(shopping_day.is_shopping);
	
	Date d = shopping_day.date;
	d++;
	FoodDay* prev = &shopping_day;
	
	for(int i = 0; i < shop_interval; i++) {
		FoodDay& day = days.Add();
		day.date = d++;
		day.is_shopping = i == (shop_interval-1);
		day.food_grams <<= prev->food_grams;
		for(int j = 0; j < prev->food_usage.GetCount(); j++)
			day.food_grams.Get(prev->food_usage.GetKey(j)) -= prev->food_usage[j];
		
		
		prev = &day;
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

Time FoodStorage::GetLastShopping() {
	return last_shopping;
}

Time FoodStorage::GetNextShopping() {
	return next_shopping;
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
	for(int i = days.GetCount()-1; i >= 0; i--) {
		FoodDay& day = days[i];
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
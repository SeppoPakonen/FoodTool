#ifndef _FoodTool_Food_h_
#define _FoodTool_Food_h_

enum {
	OIL,
	MILK,
	PROCESSED_MILK,
	VEGETABLE,
	PROCESSED_VEGETABLE,
	MEAT,
	PROCESSED_MEAT
};

enum {
	MORNING = 1,
	EARLY_DAY = 2,
	LUNCH = 4,
	SNACK = 8,
	ALL_DAY = 0xF,
	NOT_MORNING = 0xE
};

struct FoodType : Moveable<FoodType> {
	String name;
	float kcals, fat, carbs, protein, salt;
	int cat;
	int healthiness;
	int pkg_size_grams;
	int serving_grams = 0;
	
	
	FoodType& SetServing(int grams) {serving_grams = grams; return *this;}
	void Serialize(Stream& s) {s % name % kcals % fat % carbs % protein % salt % cat % healthiness % pkg_size_grams % serving_grams;}
};

struct MealType : Moveable<MealType> {
	String name, food_str;
	int time_of_day;
	
	void Serialize(Stream& s) {s % name % food_str % time_of_day;}
};

struct FoodDay : Moveable<FoodDay> {
	Date date;
	VectorMap<String, int> food_grams, food_usage;
	bool is_shopping = false;
	String menu, shopping_list;
	
	void Serialize(Stream& s) {s % date % food_grams % food_usage % is_shopping % menu % shopping_list;}
};

struct FoodStorage {
	Array<FoodDay> days;
	Time last_shopping, next_shopping;
	int shop_interval = 0;
	
	// Temporary
	VectorMap<String, FoodType> food_types;
	VectorMap<String, MealType> meal_types;
	
	
	FoodStorage();
	
	void Serialize(Stream& s) {s % days % last_shopping % next_shopping % shop_interval;}
	
	FoodType& AddFoodType(String code, int pkg_size, String name, float kcals, float fat, float carbs, float protein, float salt, int cat, int healthiness=1);
	MealType& AddMealType(String name, String food_str, int time_of_day);
	
	void Update();
	Time GetLastShopping();
	Time GetNextShopping();
	String GetTodaysMenu();
	String GetNextShoppingList();
	bool HasEnoughPreplanned();
	void PlanWeek();
	
	bool NeedShopping();
	void DoShop();
	void ConsumeDay();
};


#endif

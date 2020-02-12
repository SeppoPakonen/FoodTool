#ifndef _FoodTool_Food_h_
#define _FoodTool_Food_h_

#define DEFAULT_STEP_GRAMS 10

enum {
	MODE_WEIGHTLOSS,
	MODE_MAINTAIN,
	MODE_MUSCLEGAIN,
};

struct DailyPlan : Moveable<DailyPlan> {
	Date date;
	Ingredient food;
	double weight, prog;
	double fat_perc, fat_kgs, lean_body_kgs;
	double maintain_calories, allowed_calories, maintain_burned_calories;
	double walking_burned_calories, jogging_burned_calories, burned_calories;
	double burned_kgs;
	byte mode;
	
	void Serialize(Stream& s) {
		s	% date
			% food
			% weight % prog
			% fat_perc % fat_kgs % lean_body_kgs
			% maintain_calories % allowed_calories % maintain_burned_calories
			% walking_burned_calories % jogging_burned_calories % burned_calories
			% burned_kgs
			% mode;
	}
};

enum {
	BREAKFAST,
	BRUNCH,
	ELEVENSES,
	LUNCH,
	TEA,
	DINNER,
	SUPPER
};

struct MealIngredient : Moveable<MealIngredient> {
	VectorMap<int, String> pre_day_instructions;
	String instructions;
	double grams;
	int db_food_no = -1;
	
	void Serialize(Stream& s) {s % pre_day_instructions % instructions % grams % db_food_no;}
};

struct MealDebugger {
	VectorMap<int, OnlineAverage1> nutr_fabs_av;
	bool do_debug = false;
};

typedef VectorMap<int, float> FoodQuantity;
typedef VectorMap<int, int> FoodQuantityInt;

struct FoodDay;

struct MealPreset : Moveable<MealPreset> {
	VectorMap<int, String> pre_day_instructions;
	Vector<MealIngredient> ingredients;
	String instructions;
	String name;
	double serving_grams = 0;
	double score = 0;
	int type = 0;
	
	void Serialize(Stream& s) {s % pre_day_instructions % ingredients % instructions % name % serving_grams % score % type;}
	double GetOptimizerEnergy(const FoodDay& d, MealDebugger& dbg);
	void GetNutritions(Ingredient& ing) const;
};

struct Meal : Moveable<Meal> {
	String key;
	Time time;
	FoodQuantity food;
	float grams = 0;
	int gen_i = -1;
	
	void Serialize(Stream& s) {s % key % time % food % grams % gen_i;}
};

struct FoodDay : Moveable<FoodDay> {
	Date date;
	Time wake_time, sleep_time;
	Vector<Meal> meals;
	FoodQuantity food_grams, food_usage;
	FoodQuantityInt buy_amount;
	FoodQuantity used_food_amount;
	VectorMap<String, int> used_meal_amount;
	VectorMap<int, int> generated_meals;
	Ingredient target_sum, total_sum;
	IngredientDouble total_consumed;
	bool is_shopping = false;
	String menu, preparation, shopping_list;
	byte mode;
	
	void SetMealGrams(const Vector<double>& grams, bool check=false);
	double GetOptimizerEnergy();
	void Serialize(Stream& s) {
		s	% date
			% wake_time % sleep_time
			% meals
			% food_grams
			% food_usage
			% buy_amount
			% used_food_amount
			% used_meal_amount
			% generated_meals
			% target_sum
			% total_sum
			% total_consumed
			% is_shopping
			% menu
			% preparation
			% shopping_list
			% mode;
	}
};

struct FoodStorage {
	Array<FoodDay> days;
	
	// Temporary
	VectorMap<String, MealPreset> meal_types;
	
	
	FoodStorage();
	
	void Serialize(Stream& s) {s % days;}
	
	MealPreset& AddMealPreset(String code);
	
	void Init(Date begin);
	void Update(const Vector<DailyPlan>& planned_daily);
	Date GetLastShopping();
	Date GetNextShopping();
	String GetTodaysMenu();
	String GetNextShoppingList();
	bool HasEnoughPreplanned();
	void PlanWeek(const Vector<DailyPlan>& planned_daily);
	void MakeMenu(FoodDay& d);
	void AddFoodQuantity(const FoodQuantityInt& src, FoodQuantity& dst);
	
	bool NeedShopping();
	void DoShop();
	void ConsumeDay();
};


#endif

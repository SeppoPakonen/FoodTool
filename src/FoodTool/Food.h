#ifndef _FoodTool_Food_h_
#define _FoodTool_Food_h_

#define DEFAULT_STEP_GRAMS 10
#define MINIMUM_DAILY_KCAL 600.0

enum {
	MODE_WEIGHTLOSS,
	MODE_MAINTAIN,
	MODE_MUSCLEGAIN,
};

struct DailyPlan : Moveable<DailyPlan> {
	Date date;
	Ingredient food;
	double weight, prog;
	double fat_perc, fat_kgs, lean_kgs;
	double maintain_calories, allowed_calories, maintain_burned_calories;
	double walking_burned_calories, jogging_burned_calories, burned_calories;
	double burned_kgs;
	byte mode;
	bool is_easy_day;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			s	% date
				% food
				% weight % prog
				% fat_perc % fat_kgs % lean_kgs
				% maintain_calories % allowed_calories % maintain_burned_calories
				% walking_burned_calories % jogging_burned_calories % burned_calories
				% burned_kgs
				% mode
				% is_easy_day
				;
		}
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
	double min_grams = 0, max_grams = 0;
	int db_food_no = -1;
	
	MealIngredient() {}
	MealIngredient(const MealIngredient& s) {*this = s;}
	void operator=(const MealIngredient& s) {
		pre_day_instructions <<= s.pre_day_instructions;
		instructions = s.instructions;
		min_grams = s.min_grams;
		max_grams = s.max_grams;
		db_food_no = s.db_food_no;
	}
	void Serialize(Stream& s) {
		VER(1);
		FOR_VER(0) {s % pre_day_instructions % instructions % max_grams % db_food_no;}
		FOR_VER(1) {s % min_grams;}
	}
};

struct MealDebugger {
	VectorMap<int, OnlineAverage1> nutr_fabs_av;
	bool do_debug = false;
};

typedef VectorMap<int, float> FoodQuantity;
typedef VectorMap<int, int> FoodQuantityInt;

struct FoodDay;

struct MealPresetVariant : Moveable<MealPresetVariant> {
	Vector<MealIngredient> ingredients;
	String name;
	double score = 0, mass_factor = 0, taste_factor = 0;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % ingredients % name % score % mass_factor % taste_factor;}
	}
	void UpdateFactors();
};

struct MealPreset : Moveable<MealPreset> {
	VectorMap<int, String> pre_day_instructions;
	Vector<MealIngredient> ingredients;
	Vector<MealPresetVariant> variants;
	String instructions;
	String key, name;
	double serving_grams = 0;
	double score = 0, mass_factor = 0, taste_factor = 0;
	int type = 0;
	Time added;
	
	void Serialize(Stream& s) {
		VER(3);
		FOR_VER(0) {s % pre_day_instructions % ingredients % instructions % name % serving_grams % score % type;}
		FOR_VER(1) {s % added % mass_factor % taste_factor;}
		FOR_VER(2) {s % key;}
		FOR_VER(3) {s % variants;}
		
		//if (s.IsLoading()) variants.Clear();
		if (key.IsEmpty()) MakeUnique();
	}
	void MakeUnique() {key=""; for(int i = 0; i < 8; i++) key.Cat('a' + Random('z' - 'a' + 1));}
	double GetOptimizerEnergy(const Ingredient& target_sum, const Index<int>& nutr_idx, MealDebugger& dbg);
	void GetNutritions(Ingredient& ing) const;
	void UpdateFactors();
	void MakeVariants();
	int FindIngredient(int food_no) const;
};

struct Meal : Moveable<Meal> {
	String key;
	Time time;
	FoodQuantity food;
	float grams = 0;
	int gen_i = -1;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % key % time % food % grams % gen_i;}
	}
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
		VER(0);
		FOR_VER(0) {
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
	}
};

struct FoodStorage {
	Array<FoodDay> days;
	
	// Temporary
	VectorMap<String, MealPreset> meal_types;
	
	
	FoodStorage();
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % days;}
	}
	
	MealPreset& AddMealPreset(String code);
	
	void Init(Date begin);
	void Update(const Vector<DailyPlan>& planned_daily);
	Date GetLastShopping();
	Date GetNextShopping();
	String GetTodaysMenu();
	String GetNextShoppingList();
	bool HasEnoughPreplanned();
	void PlanWeek(const Vector<DailyPlan>& planned_daily);
	void MakeMenu(const DailyPlan& p, FoodDay& d);
	void AddFoodQuantity(const FoodQuantityInt& src, FoodQuantity& dst);
	
	bool NeedShopping();
	void DoShop();
	void ConsumeDay();
};


#endif

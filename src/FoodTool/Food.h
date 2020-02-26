#ifndef _FoodTool_Food_h_
#define _FoodTool_Food_h_

#define MINIMUM_DAILY_KCAL 600.0

enum {
	MODE_WEIGHTLOSS,
	MODE_MAINTAIN,
	MODE_MUSCLEGAIN,
};

enum {
	VARIANT_SCORE,
	VARIANT_EASIEST,
	VARIANT_AMINOACIDS,
	VARIANT_FATTYACIDS,
	VARIANT_WEIGHTLOSS,
	VARIANT_MUSCLEGAIN,
	VARIANT_MAINTENANCE,
	
	VARIANT_COUNT,
};

inline String GetVariantString(int i) {
	switch (i) {
		case VARIANT_SCORE:       return "Score";
		case VARIANT_EASIEST:     return "Easiest";
		case VARIANT_AMINOACIDS:  return "Amino Acids";
		case VARIANT_FATTYACIDS:  return "Fatty Acids";
		case VARIANT_WEIGHTLOSS:  return "Weight Loss";
		case VARIANT_MUSCLEGAIN:  return "Muscle Gain";
		case VARIANT_MAINTENANCE: return "Maintenance";
		default: return "Invalid variant";
	};
}

struct DailyPlan : Moveable<DailyPlan> {
	Date date;
	Ingredient food;
	double weight, prog;
	double fat_perc, fat_kgs, lean_kgs;
	double maintain_calories, allowed_calories, maintain_burned_calories;
	double walking_burned_calories, jogging_burned_calories, strength_burned_calories, burned_calories;
	double burned_kgs;
	byte mode, variant_type;
	bool is_easy_day;
	
	void Serialize(Stream& s) {
		VER(2);
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
		FOR_VER(1) {s % variant_type;}
		FOR_VER(2) {s % strength_burned_calories;}
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
	double score = 0, mass_factor = 0, taste_factor = 0;
	
	String removed0;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % ingredients % removed0 % score % mass_factor % taste_factor;}
		removed0.Clear();
	}
	void UpdateFactors();
	void GetNutritions(Ingredient& ing) const;
};

void GetIngredientNutritions(Ingredient& dst, const Vector<MealIngredient>& ingredients);

struct MealPreset : Moveable<MealPreset> {
	VectorMap<int, String> pre_day_instructions;
	Vector<MealIngredient> ingredients;
	Vector<MealPresetVariant> variants;
	String instructions;
	String key, name;
	double serving_grams = 0;
	double score = 0, mass_factor = 0, taste_factor = 0;
	int type = 0;
	int wished_factor = 0;
	Time added;
	
	void Serialize(Stream& s) {
		VER(4);
		FOR_VER(0) {s % pre_day_instructions % ingredients % instructions % name % serving_grams % score % type;}
		FOR_VER(1) {s % added % mass_factor % taste_factor;}
		FOR_VER(2) {s % key;}
		FOR_VER(3) {s % variants;}
		FOR_VER(4) {s % wished_factor;}
		
		//if (s.IsLoading()) variants.Clear();
		if (key.IsEmpty()) MakeUnique();
	}
	void MakeUnique() {key=""; for(int i = 0; i < 8; i++) key.Cat('a' + Random('z' - 'a' + 1));}
	void GetNutritions(Ingredient& ing) const;
	void UpdateFactors();
	void MakeVariants();
	int FindIngredient(int food_no) const;
};

struct Meal : Moveable<Meal> {
	String key;
	Time time;
	float grams = 0;
	
	int removed0 = -1;
	FoodQuantity removed1;
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % key % time % removed1 % grams % removed0;}
	}
};

struct FoodDay : Moveable<FoodDay> {
	Date date;
	Time wake_time, sleep_time;
	Vector<Meal> meals;
	FoodQuantity food_grams, food_usage, supplement_usage;
	FoodQuantityInt buy_amount;
	FoodQuantity used_food_amount;
	VectorMap<String, int> used_meal_amount;
	Ingredient target_sum, total_sum, supplement_sum, food_sum;
	IngredientDouble total_consumed;
	bool is_shopping = false;
	String menu, preparation;
	byte mode;
	
	VectorMap<int, int> removed0;
	String removed1;
	
	void Serialize(Stream& s) {
		VER(1);
		FOR_VER(0) {
			s	% date
				% wake_time % sleep_time
				% meals
				% food_grams
				% food_usage
				% buy_amount
				% used_food_amount
				% used_meal_amount
				% removed0
				% target_sum
				% total_sum
				% total_consumed
				% is_shopping
				% menu
				% preparation
				% removed1
				% mode;
		}
		FOR_VER(1) {s % supplement_usage % supplement_sum % food_sum;}
	}
	void ResetPlan() {
		total_sum.Reset();
		menu.Clear();
		preparation.Clear();
		meals.Clear();
		food_usage.Clear();
		supplement_usage.Clear();
		supplement_sum.Reset();
		food_sum.Reset();
	}
};


#endif

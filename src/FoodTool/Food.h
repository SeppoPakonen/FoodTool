#ifndef _FoodTool_Food_h_
#define _FoodTool_Food_h_

enum {
	MINERAL,
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

template <class T>
struct FoodDetailedT : Moveable<FoodDetailedT<T>> {
	T grams = 0, kcals = 0, fat = 0, carbs = 0, protein = 0, salt = 0;
	
	void Reset() {grams = 0, kcals = 0, fat = 0, carbs = 0, protein = 0, salt = 0;}
	void Serialize(Stream& s) {s % grams % kcals % fat % carbs % protein % salt;}
	void Limit(T kcals, T macro, T salt) {
		this->kcals = max(this->kcals, kcals);
		this->fat = max(this->fat, macro);
		this->carbs = max(this->carbs, macro);
		this->protein = max(this->protein, macro);
		this->grams = max(this->grams, fat+carbs+protein);
		this->salt = max(this->salt, salt);
	}
	template <class K>
	void operator +=(const K& d) {
		grams += d.grams;
		kcals += d.kcals;
		fat += d.fat;
		carbs += d.carbs;
		protein += d.protein;
		salt += d.salt;
	}
	template <class K>
	void operator -=(const K& d) {
		grams -= d.grams;
		kcals -= d.kcals;
		fat -= d.fat;
		carbs -= d.carbs;
		protein -= d.protein;
		salt -= d.salt;
	}
	void operator*=(double mul) {
		grams *= mul;
		kcals *= mul;
		fat *= mul;
		carbs *= mul;
		protein *= mul;
		salt *= mul;
	}
	void ChangeGrams(double new_grams) {
		double mul = new_grams / grams;
		kcals *= mul;
		fat *= mul;
		carbs *= mul;
		protein *= mul;
		salt *= mul;
		ASSERT(IsFin(grams) && IsFin(kcals) && IsFin(fat) && IsFin(carbs) && IsFin(protein) && IsFin(salt));
		grams = new_grams;
	}
};

template <class T>
inline FoodDetailedT<T> operator-(const FoodDetailedT<T>& a, const FoodDetailedT<T>& b) {
	FoodDetailedT<T> out;
	out.grams = a.grams - b.grams;
	out.kcals = a.kcals - b.kcals;
	out.fat = a.fat - b.fat;
	out.carbs = a.carbs - b.carbs;
	out.protein = a.protein - b.protein;
	out.salt = a.salt - b.salt;
	return out;
}

typedef FoodDetailedT<float> FoodDetailed;
typedef FoodDetailedT<double> FoodDetailedDouble;

struct DailyPlan : Moveable<DailyPlan> {
	Date date;
	FoodDetailed food;
	double weight, prog;
	double fat_perc, fat_kgs, lean_body_kgs;
	double maintain_calories, allowed_calories, maintain_burned_calories;
	double walking_burned_calories, jogging_burned_calories, burned_calories;
	double burned_kgs;
	
	void Serialize(Stream& s) {
		s	% date
			% food
			% weight % prog
			% fat_perc % fat_kgs % lean_body_kgs
			% maintain_calories % allowed_calories % maintain_burned_calories
			% walking_burned_calories % jogging_burned_calories % burned_calories
			% burned_kgs;
	}
};


struct FoodType : Moveable<FoodType> {
	String name;
	FoodDetailed details;
	int shop_order;
	int cat;
	int healthiness;
	int serving_grams = 0;
	
	
	FoodType& SetServing(int grams) {serving_grams = grams; return *this;}
	void Serialize(Stream& s) {s % name % details % shop_order % cat % healthiness % serving_grams;}
	bool operator()(const FoodType& a, const FoodType& b) const {return a.shop_order < b.shop_order;}
};

struct MealType : Moveable<MealType> {
	String name, food_str;
	int time_of_day;
	float frequency;
	Vector<String> foods, opt_foods;
	
	void Serialize(Stream& s) {s % name % food_str % time_of_day % frequency % foods % opt_foods;}
};

typedef VectorMap<String, FoodDetailed> DetailedFoodQuantity;
typedef VectorMap<String, float> FoodQuantity;
typedef VectorMap<String, int> FoodQuantityInt;

struct Meal : Moveable<Meal> {
	String key;
	DetailedFoodQuantity food;
	FoodDetailed food_sum;
	FoodDetailed target_sum;
	
	void Serialize(Stream& s) {s % key % food % food_sum % target_sum;}
};

struct FoodDay : Moveable<FoodDay> {
	Date date;
	Vector<Meal> meals;
	FoodQuantity food_grams, food_usage;
	FoodQuantityInt buy_amount;
	FoodQuantity used_food_amount;
	FoodQuantityInt used_meal_amount;
	FoodDetailed target_sum, total_sum;
	FoodDetailedDouble total_consumed;
	bool is_shopping = false;
	String menu, shopping_list;
	
	void SetMealGrams(const Vector<double>& grams, const VectorMap<String, FoodType>& food_types, bool check=false);
	double GetOptimizerEnergy();
	void Serialize(Stream& s) {
		s	% date
			% meals
			% food_grams
			% food_usage
			% buy_amount
			% used_food_amount
			% used_meal_amount
			% target_sum
			% total_sum
			% total_consumed
			% is_shopping
			% menu
			% shopping_list;
	}
};

struct FoodStorage {
	Array<FoodDay> days;
	
	// Temporary
	VectorMap<String, FoodType> food_types;
	VectorMap<String, MealType> meal_types;
	
	
	FoodStorage();
	
	void Serialize(Stream& s) {s % days;}
	
	FoodType& AddFoodType(String code, int shop_order, int pkg_size, String name, float kcals, float fat, float carbs, float protein, float salt, int cat, int healthiness=1);
	MealType& AddMealType(String code, String name, String food_str, int time_of_day, float frequency=1);
	
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

struct FoodQuantitySorter {
	const FoodStorage& storage;
	const FoodQuantitySorter(FoodStorage* s) : storage(*s) {}
	bool operator()(String a, String b) const {return storage.food_types.Get(a).shop_order < storage.food_types.Get(b).shop_order;}
};

struct MealQuantitySorter {
	const FoodStorage& storage;
	const FoodDay& day;
	MealQuantitySorter(FoodStorage* s, FoodDay& day) : storage(*s), day(day) {}
	bool operator()(int a, int b) const {
		return
			(day.used_meal_amount.Get(storage.meal_types.GetKey(a), 0) /
			storage.meal_types[a].frequency)
			<
			(day.used_meal_amount.Get(storage.meal_types.GetKey(b), 0) /
			storage.meal_types[b].frequency);
	}
};

#endif

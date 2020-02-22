#ifndef _FoodTool_FoodStorage_h_
#define _FoodTool_FoodStorage_h_


struct FoodStorageSnapshotRemoved : Moveable<FoodStorageSnapshotRemoved> {
	Date date;
	FoodQuantity food_grams;
	
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % date % food_grams;}
	}
};

struct FoodStorage {
	Array<FoodDay> days;
	
	Array<FoodStorageSnapshotRemoved> removed0;
	
	
	// Temporary
	VectorMap<String, MealPreset> meal_types;
	
	
	FoodStorage();
	
	void Serialize(Stream& s) {
		VER(1);
		FOR_VER(0) {s % days;}
		FOR_VER(1) {s % removed0;}
	}
	
	MealPreset& AddMealPreset(String code);
	
	void Init(Date begin);
	void Update(bool replan, const Vector<DailyPlan>& planned_daily);
	Date GetLastShopping();
	Date GetNextShopping();
	String GetTodaysMenu();
	String GetNextShoppingList();
	void PlanDay(int i, const Vector<DailyPlan>& planned_daily);
	void PlanShopping(int i, const Vector<DailyPlan>& planned_daily);
	void MakeMenu(const DailyPlan& p, FoodDay& d);
	void AddFoodQuantity(const FoodQuantityInt& src, FoodQuantity& dst);
	
	bool NeedShopping();
	void DoShop();
	void ConsumeDay();
};

int FindBestMeal(double weight, double kcal, int variant_type, const FoodQuantity& food_left, const VectorMap<String, int>& used_meal_amount, const Ingredient& target_sum);

void FindSetFoodStorageSnapshot(Date date, FoodQuantity& food_grams);

#endif

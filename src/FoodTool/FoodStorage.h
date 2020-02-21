#ifndef _FoodTool_FoodStorage_h_
#define _FoodTool_FoodStorage_h_


struct FoodStorageSnapshot : Moveable<FoodStorageSnapshot> {
	Date date;
	FoodQuantity food_grams;
	
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % date % food_grams;}
	}
};

struct FoodStorage {
	Array<FoodDay> days;
	Array<FoodStorageSnapshot> snaps;
	
	
	// Temporary
	VectorMap<String, MealPreset> meal_types;
	
	
	FoodStorage();
	
	void Serialize(Stream& s) {
		VER(1);
		FOR_VER(0) {s % days;}
		FOR_VER(1) {s % snaps;}
	}
	
	MealPreset& AddMealPreset(String code);
	
	void Init(Date begin);
	void Update(bool replan, const Vector<DailyPlan>& planned_daily);
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


struct FoodStorageCtrl : public ParentCtrl {
	
	
	typedef FoodStorageCtrl CLASSNAME;
	FoodStorageCtrl();
	
	void Data();
	
};

#endif

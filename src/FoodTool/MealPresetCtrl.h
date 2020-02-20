#ifndef _FoodTool_MealPresetCtrl_h_
#define _FoodTool_MealPresetCtrl_h_


struct MealPresetCtrl : WithMealPresetLayout<ParentCtrl> {
	Array<EditDouble> min_mass, max_mass;
	
	
	typedef MealPresetCtrl CLASSNAME;
	MealPresetCtrl();
	
	void Data();
	void Add();
	void Remove();
	void MakeVariants();
	void SelectPreset();
	void PresetChanged();
	void AddIngredient();
	void RemoveIngredient();
	void Filter();
	void AddByNutritionGroup(int group);
	void AddVitamins();
	void AddMinerals();
	void AddAminoAcids();
	void AddFattyAcids();
	
	
};

#endif

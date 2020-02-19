#ifndef _FoodTool_MealPresetCtrl_h_
#define _FoodTool_MealPresetCtrl_h_


struct MealPresetCtrl : WithMealPresetLayout<ParentCtrl> {
	Array<EditDouble> min_mass, max_mass;
	
	
	typedef MealPresetCtrl CLASSNAME;
	MealPresetCtrl();
	
	void Data();
	void Add();
	void Remove();
	void SelectPreset();
	void PresetChanged();
	void AddIngredient();
	void RemoveIngredient();
	void Filter();
	
	
};

#endif

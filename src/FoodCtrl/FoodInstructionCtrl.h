#ifndef _FoodTool_FoodInstructionCtrl_h_
#define _FoodTool_FoodInstructionCtrl_h_


struct FoodInstructionCtrl : public ParentCtrl {
	ArrayCtrl list, meallist, inglist, nutrlist;
	Splitter vsplit0, vsplit1, hsplit;
	
	
	typedef FoodInstructionCtrl CLASSNAME;
	FoodInstructionCtrl();
	void Data();
	void SelectDate();
	void SelectMeal();
	void SelectIngredient();
};


#endif

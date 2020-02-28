#ifndef _FoodTool_DailyNutritionsCtrl_h_
#define _FoodTool_DailyNutritionsCtrl_h_


struct DailyNutritionsCtrl : public ParentCtrl {
	ArrayCtrl list, srclist, nutrlist;
	Splitter vsplit, hsplit;
	ArrayMap<String, Ingredient> src;
	
	typedef DailyNutritionsCtrl CLASSNAME;
	DailyNutritionsCtrl();
	void Data();
	void SelectDate();
	void SelectSource();
};


#endif

#ifndef _FoodTool_SupplementCtrl_h_
#define _FoodTool_SupplementCtrl_h_

struct SupplementCtrl : public WithSupplementLayout<ParentCtrl> {
	Array<Option> is_weightloss, is_maintenance;
	
	typedef SupplementCtrl CLASSNAME;
	SupplementCtrl();
	void Data();
	void AddFood();
	void RemoveFood();
	void SelectNutrition();
	void ValueChanged(int nutr_i);
};

#endif

#ifndef _FoodTool_FoodWishCtrl_h_
#define _FoodTool_FoodWishCtrl_h_

struct FoodWishCtrl : public ParentCtrl {
	Splitter vsplit;
	ArrayCtrl list, ingredients;
	Array<EditIntSpin> edits;
	
	typedef FoodWishCtrl CLASSNAME;
	FoodWishCtrl();
	void Data();
	void SelectIngredient();
	void ValueChanged(int preset_i);
};

#endif

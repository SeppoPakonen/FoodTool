#ifndef _FoodTool_FoodStorageCtrl_h_
#define _FoodTool_FoodStorageCtrl_h_


struct FoodStorageCtrl : public WithFoodStorageLayout<ParentCtrl> {
	Array<EditIntNotNull> edits;
	
	typedef FoodStorageCtrl CLASSNAME;
	FoodStorageCtrl();
	
	void Data();
	void AddSnap();
	void SelectSnap();
	void SnapChanged();
	
};


#endif

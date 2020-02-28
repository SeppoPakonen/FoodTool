#ifndef _FoodTool_DatabaseCtrl_h_
#define _FoodTool_DatabaseCtrl_h_


struct DatabaseCtrl : public WithDatabaseCtrlLayout<ParentCtrl> {
	int selected_food = -1;
	int selected_nutr = -1;
	
	typedef DatabaseCtrl CLASSNAME;
	DatabaseCtrl();
	
	void Data();
	void NutrientData();
	void Filter();
	void SelectFood();
	void ToggleUsed();
	void AddFood();
	void UpdateFood();
	void SelectNutrition();
	void AddNutrition();
	String GetModificationCppCode();
	
	void DumpDebug() {LOG("\n" + GetModificationCppCode());}
};

#endif

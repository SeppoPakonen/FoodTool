#ifndef _FoodTool_ProfileCreator_h_
#define _FoodTool_ProfileCreator_h_

class ProfileCreator : public TopWindow {
	WithWelcomeLayout<ParentCtrl> tab0;
	WithConfigurationLayout<ParentCtrl> tab1;
	WeightCtrl tab2;
	WithDatabasePresetLayout<ParentCtrl> tab3;
	WithDatabaseEditLayout<ParentCtrl> tab4;
	MealPresetCtrl tab5;
	int tab = 0;
	
	VectorMap<int, double> height_bmis;
	
public:
	typedef ProfileCreator CLASSNAME;
	ProfileCreator();
	
	void Previous();
	void Next();
};


#endif

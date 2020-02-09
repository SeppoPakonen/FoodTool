#ifndef _FoodTool_ProfileCreator_h_
#define _FoodTool_ProfileCreator_h_

struct FatPercentageReferenceWindow : public TopWindow {
	ImageCtrl img;
	Button close;
	
	typedef FatPercentageReferenceWindow CLASSNAME;
	FatPercentageReferenceWindow();
	void Close0() {Close();Close();}
};

class ProfileCreator : public WithConfigurationLayout<TopWindow> {
	VectorMap<int, double> height_bmis;
	FatPercentageReferenceWindow iw;
	
public:
	typedef ProfileCreator CLASSNAME;
	ProfileCreator();
	
	void Next();
	void UpdateTargetWeight();
	void ShowWeightReference();
};


#endif

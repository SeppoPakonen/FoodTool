#ifndef _FoodTool_HairSampleDeficitCtrl_h_
#define _FoodTool_HairSampleDeficitCtrl_h_


struct HairSampleDeficitEdit : public Ctrl {
	HairSampleDeficitProfile prof;
	Vector<double> x_to_percentile;
	double x25, x160, x450, x500, x550, x840, x975;
	
	Color green, yellow, red;
	
	HairSampleDeficitEdit();
	virtual void Paint(Draw& d);
	virtual void LeftDown(Point p, dword keyflags);
};

struct HairSampleDeficitCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	ParentCtrl right;
	Button add;
	HairSampleDeficitEdit edit;
	
	
	typedef HairSampleDeficitCtrl CLASSNAME;
	HairSampleDeficitCtrl();
	void Data();
	void SelectItem();
	void AddItem();
};


#endif

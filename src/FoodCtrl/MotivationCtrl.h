#ifndef _FoodTool_MotivationCtrl_h_
#define _FoodTool_MotivationCtrl_h_


struct MotivationCtrl : public Ctrl {
	int motivation_i = -1, quote_i = -1;
	Image img;
	Vector<String> quotes;
	
	typedef MotivationCtrl CLASSNAME;
	MotivationCtrl();
	
	void Data() {Refresh();}
	void Paint(Draw& d);
};


#endif

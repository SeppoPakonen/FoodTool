#ifndef _FoodTool_UsageCtrl_h_
#define _FoodTool_UsageCtrl_h_


struct UsageCtrl : public ParentCtrl {
	ArrayCtrl list;
	Label total;
	
	typedef UsageCtrl CLASSNAME;
	UsageCtrl();
	
	void Data();
};


#endif

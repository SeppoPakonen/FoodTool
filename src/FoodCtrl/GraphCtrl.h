#ifndef _FoodTool_GraphCtrl_h_
#define _FoodTool_GraphCtrl_h_


struct GraphCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	MultipurposeGraph graph;
	
	
	typedef GraphCtrl CLASSNAME;
	GraphCtrl();
	
	void Data();
	void SelectSource();
};


#endif

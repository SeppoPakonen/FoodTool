#ifndef _FoodTool_ExceptionsCtrl_h_
#define _FoodTool_ExceptionsCtrl_h_


struct ExceptionsCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithExceptionLayout<ParentCtrl> exc;
	
	typedef ExceptionsCtrl CLASSNAME;
	ExceptionsCtrl();
	
	void Data(bool force=false);
	void AddException();
	
};


#endif

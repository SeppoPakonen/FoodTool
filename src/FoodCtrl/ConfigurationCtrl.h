#ifndef _FoodTool_ConfigurationCtrl_h_
#define _FoodTool_ConfigurationCtrl_h_


struct ConfigurationCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl list;
	WithConfigurationLayout<ParentCtrl> conf;
	
	typedef ConfigurationCtrl CLASSNAME;
	ConfigurationCtrl();
	
	void Data();
	void SelectConf();
	void AddConf();
};


#endif

#ifndef _FoodTool_PriceCtrl_h_
#define _FoodTool_PriceCtrl_h_


struct PriceCtrl : public ParentCtrl {
	Splitter hsplit;
	ArrayCtrl foodlist;
	WithQuoteListLayout<ParentCtrl> history;
	Array<EditIntSpin> grams, servings, batch;
	Array<EditDoubleSpin> price;
	Array<EditString> shop;
	
	typedef PriceCtrl CLASSNAME;
	PriceCtrl();
	void Data(bool force);
	void SelectFood();
	void Add();
	void ValueChanged(int quote);
};


#endif

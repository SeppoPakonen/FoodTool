#ifndef _FoodTool_FoodLogCtrl_h_
#define _FoodTool_FoodLogCtrl_h_

enum {
	FOODLOG,
	GROCERYLOG,
	RECEIPTLOG,
};

struct FoodLogCtrl;

struct PortableListCtrl : public TopWindow {
	FoodLogCtrl* log = NULL;
	TableDrawer table;
	
	PortableListCtrl();
	void Paint(Draw& d) override;
};

struct FoodLogCtrl : public ParentCtrl {
	Splitter hsplit, vsplit;
	ArrayCtrl queue, history;
	WithProductListLayout<ParentCtrl> products;
	Array<EditIntSpin> grams, servings, batch;
	Array<EditDoubleSpin> price;
	Array<EditString> shop;
	PortableListCtrl portable;
	
	int mode = 0;
	
	
	typedef FoodLogCtrl CLASSNAME;
	FoodLogCtrl();
	void Data();
	void SelectHistory();
	void SelectQueue();
	void Add();
	void Zero();
	void Expand();
	void Reduce();
	void ApplyStore();
	void Load(const FoodPrice& p);
	ProductQueueHistory& GetData();
	void SetData(ArrayCtrl& list, Vector<FoodPrice>& data);
	void SetEditCount(int i);
	void DebugClear() {GetData().history.Clear(); Data();}
	void ClearQueue() {GetData().queue.Clear(); Data();}
	void MultiplyMass();
	//void ShowPortableList() {easy.PopUp(this);}
	void ShowPortableList() {portable.Open(this);}
};


#endif

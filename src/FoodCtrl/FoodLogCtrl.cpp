#include "FoodCtrl.h"


FoodLogCtrl::FoodLogCtrl() {
	CtrlLayout(products);
	
	portable.log = this;
	
	ParentCtrl::Add(hsplit.SizePos());
	hsplit.Horz() << vsplit << products;
	hsplit.SetPos(2500);
	vsplit.Vert() << queue << history;
	
	queue.AddIndex();
	queue.AddColumn(t_("Queue Time"));
	queue.AddColumn(t_("Products"));
	queue.AddColumn(t_("Price"));
	queue.ColumnWidths("2 1 1");
	queue.WhenAction = THISBACK(SelectQueue);
	queue.WhenLeftClick = THISBACK(SelectQueue);
	
	history.AddIndex();
	history.AddColumn(t_("History Time"));
	history.AddColumn(t_("Products"));
	history.AddColumn(t_("Price"));
	history.ColumnWidths("2 1 1");
	history.WhenAction = THISBACK(SelectHistory);
	history.WhenLeftClick = THISBACK(SelectHistory);
	
	products.list.AddIndex();
	products.list.AddColumn(t_("Food"));
	products.list.AddColumn(t_("Grams"));
	products.list.AddColumn(t_("Servings"));
	products.list.AddColumn(t_("Batch Size"));
	products.list.AddColumn(t_("Price (EUR)"));
	products.list.AddColumn(t_("Shop"));
	
	products.store.WhenEnter = THISBACK(ApplyStore);
	products.showportable <<= THISBACK(ShowPortableList);
	products.add <<= THISBACK(Add);
	products.zero <<= THISBACK(Zero);
	products.expand <<= THISBACK(Expand);
	products.reduce <<= THISBACK(Reduce);
	products.mul.WhenEnter = THISBACK(MultiplyMass);
	//products.dbgclear <<= THISBACK(DebugClear);
	products.dbgclear <<= THISBACK(ClearQueue);
}

void FoodLogCtrl::MultiplyMass() {
	double mul = products.mul.GetData();
	for(int i = 0; i < products.list.GetCount(); i++) {
		double mass = products.list.Get(i, 2);
		if (mass > 0) {
			double new_mass = mass * mul;
			products.list.Set(i, 2, max(1, (int)new_mass));
		}
	}
}

void FoodLogCtrl::ApplyStore() {
	String store = products.store.GetData();
	for(int i = 0; i < products.list.GetCount(); i++) {
		products.list.Set(i, 6, store);
	}
}

ProductQueueHistory& FoodLogCtrl::GetData() {
	Profile& prof = GetProfile();
	switch (mode) {
		case FOODLOG:		return prof.foodlog;
		case GROCERYLOG:	return prof.shoplog;
		case RECEIPTLOG:	return prof.receiptlog;
	}
	Panic("Invalid mode");
	throw Exc();
}

void FoodLogCtrl::SetData(ArrayCtrl& list, Vector<FoodPrice>& data) {
	Date today = GetSysTime();
	
	if (data.GetCount() != list.GetCount()) {
		for(int i = 0; i < data.GetCount(); i++) {
			FoodPrice& p = data[i];
			int row = data.GetCount() - 1 - i;
			list.Set(row, 0, i);
			list.Set(row, 1, AttrText(GetTimeString(p.time)).NormalPaper(Date(p.time) == today ? Color(170, 255, 150) : White()));
			list.Set(row, 2, p.values.GetCount());
			double price_sum = 0;
			for(const FoodPriceQuote& q : p.values)
				price_sum += q.price;
			list.Set(row, 3, Format("%2n EUR", price_sum));
		}
		list.SetCount(data.GetCount());
	}
	
}

void FoodLogCtrl::Data() {
	Profile& prof = GetProfile();
	ProductQueueHistory& data = GetData();
	
	SplitterHotfix(hsplit, 2500);
	
	SetData(queue, data.queue);
	SetData(history, data.history);
	
}

void FoodLogCtrl::SelectQueue() {
	if (!queue.IsCursor())
		return;
	
	int cursor = queue.GetCursor();
	int data_i = queue.Get(cursor, 0);
	const FoodPrice& p = GetData().queue[data_i];
	Load(p);
}

void FoodLogCtrl::SelectHistory() {
	if (!history.IsCursor())
		return;
	
	int cursor = history.GetCursor();
	int data_i = history.Get(cursor, 0);
	const FoodPrice& p = GetData().history[data_i];
	Load(p);
}

void FoodLogCtrl::Add() {
	if (queue.IsCursor()) {
		int cursor = queue.GetCursor();
		int i = queue.Get(cursor, 0);
		GetData().queue.Remove(i);
	}
	
	ArrayCtrl& list = products.list;
	FoodPrice& p = GetData().history.Add();
	p.time = GetSysTime();
	for(int i = 0; i < list.GetCount(); i++) {
		int db_no = list.Get(i, 0);
		double grams = list.Get(i, 2);
		if (grams > 0.0) {
			FoodPriceQuote& q = p.values.Add(db_no);
			q.time = p.time;
			q.grams = grams;
			q.servings = max(1, (int)list.Get(i, 3));
			q.serving_batch = max(1, (int)list.Get(i, 4));
			q.price = list.Get(i, 5);
			q.shop = list.Get(i, 6);
		}
	}
	
	Profile& prof = GetProfile();
	if (mode == FOODLOG) {
		FoodStorageSnapshot& snap = prof.storage_snaps.Add();
		if (prof.storage_snaps.GetCount() >= 2) {
			FoodStorageSnapshot& prev = prof.storage_snaps[prof.storage_snaps.GetCount()-2];
			snap = prev;
		}
		snap.time = GetSysTime();
		snap.adder = SNAPSRC_FOODLOG;
		for(int i = 0; i < p.values.GetCount(); i++) {
			int db_no = p.values.GetKey(i);
			const FoodPriceQuote& quote = p.values[i];
			int& grams = snap.foods.GetAdd(db_no, 0);
			grams -= quote.grams;
			if (grams < 0) grams = 0;
		}
	}
	else if (mode == GROCERYLOG) {
		prof.receiptlog.queue.Add(p);
	}
	else if (mode == RECEIPTLOG) {
		FoodStorageSnapshot& snap = prof.storage_snaps.Add();
		if (prof.storage_snaps.GetCount() >= 2) {
			FoodStorageSnapshot& prev = prof.storage_snaps[prof.storage_snaps.GetCount()-2];
			snap = prev;
		}
		snap.time = GetSysTime();
		snap.adder = SNAPSRC_RECEIPTLOG;
		for(int i = 0; i < p.values.GetCount(); i++) {
			int db_no = p.values.GetKey(i);
			const FoodPriceQuote& quote = p.values[i];
			ASSERT(quote.grams >= 0 && quote.grams <= 100000);
			snap.foods.GetAdd(db_no, 0) += quote.grams;
			
			prof.price.history.GetAdd(db_no).Add(quote);
		}
	}
	
	prof.StoreThis();
	
	Data();
}

void FoodLogCtrl::Zero() {
	ArrayCtrl& list = products.list;
	const Database& db = DB();
	
	SetEditCount(db.used_foods.GetCount());
	
	for(int i = 0; i < db.used_foods.GetCount(); i++) {
		int db_no = db.used_foods[i];
		const FoodDescription& d = db.food_descriptions[db_no];
		
		EditIntSpin& grams = this->grams[i];
		EditIntSpin& servings = this->servings[i];
		EditIntSpin& batch = this->batch[i];
		EditDoubleSpin& price = this->price[i];
		EditString& shop = this->shop[i];
		
		list.Set(i, 0, db_no);
		list.Set(i, 1, d.long_desc);
		list.SetCtrl(i, 1, grams);
		list.SetCtrl(i, 2, servings);
		list.SetCtrl(i, 3, batch);
		list.SetCtrl(i, 4, price);
		list.SetCtrl(i, 5, shop);
		list.Set(i, 2, 0);
		list.Set(i, 3, 0);
		list.Set(i, 4, 0);
		list.Set(i, 5, 0);
		list.Set(i, 6, "");
	}
	list.SetCount(db.used_foods.GetCount());
	list.SetSortColumn(0);
}

void FoodLogCtrl::Expand() {
	ArrayCtrl& list = products.list;
	const Database& db = DB();
	
	SetEditCount(db.used_foods.GetCount());
	
	Index<int> existing_foods;
	for(int i = 0; i < list.GetCount(); i++)
		existing_foods.Add(list.Get(i, 0));
	
	int row = list.GetCount();
	for(int i = 0; i < db.used_foods.GetCount(); i++) {
		int db_no = db.used_foods[i];
		if (existing_foods.Find(db_no) >= 0)
			continue;
		
		const FoodDescription& d = db.food_descriptions[db_no];
		
		EditIntSpin& grams = this->grams[row];
		EditIntSpin& servings = this->servings[row];
		EditIntSpin& batch = this->batch[row];
		EditDoubleSpin& price = this->price[row];
		EditString& shop = this->shop[row];
		
		list.Set(row, 0, db_no);
		list.Set(row, 1, d.long_desc);
		list.SetCtrl(row, 1, grams);
		list.SetCtrl(row, 2, servings);
		list.SetCtrl(row, 3, batch);
		list.SetCtrl(row, 4, price);
		list.SetCtrl(row, 5, shop);
		list.Set(row, 2, 0);
		list.Set(row, 3, 0);
		list.Set(row, 4, 0);
		list.Set(row, 5, 0);
		list.Set(row, 6, "");
		row++;
	}
	list.SetCount(row);
	list.SetSortColumn(0);
}

void FoodLogCtrl::Reduce() {
	ArrayCtrl& list = products.list;
	for(int i = 0; i < list.GetCount(); i++) {
		double grams = list.Get(i, 2);
		if (grams <= 0.0)
			list.Remove(i--);
	}
}

void FoodLogCtrl::SetEditCount(int i) {
	int count = max(grams.GetCount(), i);
	grams.SetCount(count);
	servings.SetCount(count);
	batch.SetCount(count);
	price.SetCount(count);
	shop.SetCount(count);
	for(int i = 0; i < count; i++) {
		grams[i].MinMax(0, 10000);
		servings[i].MinMax(0, 100000);
		batch[i].MinMax(0, 100000);
		price[i].MinMax(0, 100000);
		price[i].SetInc(0.01);
	}
}

void FoodLogCtrl::Load(const FoodPrice& p) {
	ArrayCtrl& list = products.list;
	const Database& db = DB();
	
	SetEditCount(p.values.GetCount());
	
	for(int i = 0; i < p.values.GetCount(); i++) {
		int db_no = p.values.GetKey(i);
		const FoodPriceQuote& q = p.values[i];
		const FoodDescription& d = db.food_descriptions[db_no];
		
		EditIntSpin& grams = this->grams[i];
		EditIntSpin& servings = this->servings[i];
		EditIntSpin& batch = this->batch[i];
		EditDoubleSpin& price = this->price[i];
		EditString& shop = this->shop[i];
		
		list.Set(i, 0, db_no);
		list.Set(i, 1, d.long_desc);
		list.SetCtrl(i, 1, grams);
		list.SetCtrl(i, 2, servings);
		list.SetCtrl(i, 3, batch);
		list.SetCtrl(i, 4, price);
		list.SetCtrl(i, 5, shop);
		list.Set(i, 2, q.grams < 1.0 ? 1 : (int)(q.grams + 0.5));
		list.Set(i, 3, q.servings);
		list.Set(i, 4, q.serving_batch);
		list.Set(i, 5, q.price);
		list.Set(i, 6, q.shop);
	}
	list.SetCount(p.values.GetCount());
	list.SetSortColumn(0);
}


















PortableListCtrl::PortableListCtrl() {
	SetRect(0,0,480,640);
	Title(t_("Portable List"));
	Sizeable().MaximizeBox().MinimizeBox();
	
	
}

void PortableListCtrl::Paint(Draw& d) {
	USize sz(GetSize());
	
	d.DrawRect(sz, White());
	if (!log) return;
	
	ArrayCtrl& list = log->products.list;
	if (!list.GetCount()) return;
	
	table.SetSize(list.GetCount(), 3);
	table.Reset();
	
	int rows = 0;
	for(int i = 0; i < list.GetCount(); i++) {
		double grams = list.Get(i, 2);
		if (grams > 0) {
			FoodPriceQuote q;
			q.grams = grams;
			q.servings = max(1, (int)list.Get(i, 3));
			q.serving_batch = max(1, (int)list.Get(i, 4));
			q.price = list.Get(i, 5);
			
			table.Set(rows, 0, list.Get(i, 1));
			table.Set(rows, 1, q.GetMassString());
			table.Set(rows, 2, q.GetPriceString());
			
			rows++;
		}
	}
	table.SetSize(rows, 3);
	table.Paint(sz, d);
}


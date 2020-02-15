#ifndef _TimelineCtrl_Common_h_
#define _TimelineCtrl_Common_h_


struct ComponentBase {
	int class_id = -1;
	bool removed = false;
	
	virtual void Serialize(Stream& s) = 0;
	
};


#define BASE_COMPONENT(x) \
	static const std::type_info& GetBase() {return typeid(x);} \
	x& GetRef() {return dynamic_cast<x&>(*ref);}

struct ComponentEdit : public ParentCtrl {
	ComponentBase* ref = NULL;
	
};

struct ComponentView : public ParentCtrl {
	ComponentBase* ref = NULL;
	
};










struct DummyComponent : public ComponentBase {
	int value = -1;
	
	DummyComponent() {value = Random(10);}
	void Serialize(Stream& s) override {s % value; }
};

struct DummyEdit : public ComponentEdit {
	EditIntSpin e;
	
	DummyEdit() {Add(e.VCenterPos(30).HCenterPos(150));}
	BASE_COMPONENT(DummyComponent);
	
};

struct DummyView : public ComponentView {
	
	BASE_COMPONENT(DummyComponent);
	virtual void Paint(Draw& d) {
		Size sz(GetSize());
		d.DrawRect(sz, White());
		DummyComponent& comp = GetRef();
		Font fnt = SansSerif(50);
		String txt = IntStr(comp.value);
		Size txt_sz = GetTextSize(txt, fnt);
		d.DrawText((sz.cx-txt_sz.cx)/2, (sz.cy-txt_sz.cy)/2, txt, fnt);
	}
};


#endif

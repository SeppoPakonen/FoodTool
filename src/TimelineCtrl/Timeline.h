#ifndef _TimelineCtrl_Timeline_h_
#define _TimelineCtrl_Timeline_h_


struct TimelineEvent : Moveable<TimelineEvent> {
	VectorMap<int, int> components;
	Time time;
	int prev = -1, next = -1;
	
	
	void Serialize(Stream& s) {
		s	% components
			% time
			% prev % next
			;
	}
};


struct Timeline {
	typedef ComponentBase* (*ComponentBaseFactory)();
	typedef ComponentEdit* (*ComponentEditFactory)();
	typedef ComponentView* (*ComponentViewFactory)();
	
	struct Factories : Moveable<Factories> {
		ComponentBaseFactory base = NULL;
		ComponentEditFactory edit = NULL;
		ComponentViewFactory view = NULL;
	};
	
	template <class T> static ComponentBase* ClassFactory() {return new T();}
	template <class T> static ComponentEdit* EditFactory() {return new T();}
	template <class T> static ComponentView* ViewFactory() {return new T();}
	
	VectorMap<String, Factories> class_factories;
	Mutex lock;
	
	
	// Persistent
	Array<TimelineEvent> events;
	Array<ComponentBase> components;
	Index<int> removed_events;
	int begin = -1, end = -1;
	
	
	Timeline();
	void StoreThis();
	void LoadThis();
	void StoreToFile(String s);
	void LoadFromFile(String s);
	int RealizeEvent(Time t, int hint=-1);
	void RemoveEvent(int i);
	
	template <class T> void RegisterComponentBase() {
		String key = typeid(T).name();
		Factories& f = class_factories.GetAdd(key);
		if (f.base >= 0)
			Panic("Key " + key + " has already registered base.");
		f.base = &ClassFactory<T>;
	}
	
	template <class T> void RegisterComponentEdit() {
		String key = T::GetBase().name();
		Factories& f = class_factories.GetAdd(key);
		if (f.edit >= 0)
			Panic("Key " + key + " has already registered edit.");
		f.edit = &EditFactory<T>;
	}
	
	template <class T> void RegisterComponentView() {
		String key = T::GetBase().name();
		Factories& f = class_factories.GetAdd(key);
		if (f.view >= 0)
			Panic("Key " + key + " has already registered view.");
		f.view = &ViewFactory<T>;
	}
	
};


inline Timeline& GetTimeline() {return Single<Timeline>();}
inline Timeline& TL() {return GetTimeline();}


#endif

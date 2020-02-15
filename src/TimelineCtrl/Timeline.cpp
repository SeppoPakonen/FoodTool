#include "TimelineCtrl.h"



Timeline::Timeline() {
	
}

void Timeline::StoreThis() {
	String backup_dir = ConfigFile("backups");
	RealizeDirectory(backup_dir);
	
	Date d = GetSysTime();
	String file = Format("backup_%d_%d_%d.bin", (int)d.year, (int)d.month, (int)d.day);
	String path = AppendFileName(backup_dir, file);
	StoreToFile(path);
	
	path = ConfigFile("timeline.db");
	StoreToFile(path);
}

void Timeline::LoadThis() {
	String file = ConfigFile("timeline.db");
	LoadFromFile(file);
}

void Timeline::StoreToFile(String s) {
	FileOut fout(s);
	
	fout % events % removed_events % begin % end;
	
	int comp_count = components.GetCount();
	fout.Put(&comp_count, sizeof(int));
	
	for(int i = 0; i < comp_count; i++) {
		ComponentBase& comp = components[i];
		fout.Put(&comp.class_id, sizeof(comp.class_id));
		fout.Put(&comp.removed, sizeof(comp.removed));
		if (!comp.removed)
			comp.Serialize(fout);
	}
}

void Timeline::LoadFromFile(String s) {
	if (!FileExists(s))
		return;
	
	FileIn fin(s);
	
	fin % events % removed_events % begin % end;
	if (events.IsEmpty()) return;
	
	int comp_count = 0;
	fin.Get(&comp_count, sizeof(int));
	
	components.Reserve(comp_count);
	for(int i = 0; i < comp_count; i++) {
		int class_id = -1;
		bool removed = false;
		fin.Get(&class_id, sizeof(class_id));
		fin.Get(&removed, sizeof(removed));
		
		if (class_id < 0 || class_id >= class_factories.GetCount())
			Panic("Loading failed");
		
		ComponentBase& comp = components.Add(class_factories[class_id].base());
		comp.class_id = class_id;
		comp.removed = removed;
		
		if (!removed) {
			comp.Serialize(fin);
		}
	}
}

int Timeline::RealizeEvent(Time t, int hint) {
	if (events.IsEmpty()) {
		events.Add().time = t;
		begin = 0;
		end = 0;
		return 0;
	}
	else {
		if (hint < 0 || hint >= events.GetCount()) {
			hint = 0;
			if (events.GetCount() >= 2) {
				int64 begin_dist = abs(t.Get() - events[begin].time.Get());
				int64 end_dist = abs(t.Get() - events[end].time.Get());
				if (begin_dist <= end_dist)
					hint = begin;
				else
					hint = end;
			}
		}
		
		TimelineEvent& e = events[hint];
		if (e.time == t)
			return hint;
		
		if (e.time < t) {
			TimelineEvent* prev = &e;
			int prev_i = hint;
			int next_i = e.next;
			while(true) {
				if (next_i < 0) {
					int i = events.GetCount();
					end = i;
					prev->next = i;
					TimelineEvent& e = events.Add();
					e.time = t;
					e.prev = prev_i;
					return i;
				}
				
				TimelineEvent& e = events[next_i];
				if (e.time == t)
					return next_i;
				else if (e.time > t) {
					int i = events.GetCount();
					TimelineEvent& n = events.Add();
					n.time = t;
					n.prev = prev_i;
					n.next = next_i;
					prev->next = i;
					e.prev = i;
					return i;
				}
				
				prev = &e;
				prev_i = next_i;
				next_i = e.next;
			}
		}
		else {
			TimelineEvent* next = &e;
			int next_i = hint;
			int prev_i = e.prev;
			while(true) {
				if (prev_i < 0) {
					int i = events.GetCount();
					begin = i;
					next->prev = i;
					TimelineEvent& e = events.Add();
					e.time = t;
					e.next = next_i;
					return i;
				}
				
				TimelineEvent& e = events[prev_i];
				if (e.time == t)
					return prev_i;
				else if (e.time < t) {
					int i = events.GetCount();
					TimelineEvent& n = events.Add();
					n.time = t;
					n.prev = prev_i;
					n.next = next_i;
					next->prev = i;
					e.next = i;
					return i;
				}
				
				next = &e;
				next_i = prev_i;
				prev_i = e.prev;
			}
		}
		
	}
}

void Timeline::RemoveEvent(int i) {
	Panic("TODO");
}

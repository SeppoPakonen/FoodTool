#include "FoodCtrl.h"


NoteCtrl::NoteCtrl() {
	CtrlLayout(note);
	Add(split.SizePos());
	split.Horz() << list << note;
	split.SetPos(2500);
	
	list.AddColumn(t_("Added"));
	list.AddColumn(t_("Title"));
	list.ColumnWidths("1 2");
	list.WhenAction << THISBACK(SelectNote);
	list.WhenLeftClick << THISBACK(SelectNote);
	
	note.add <<= THISBACK(AddNote);
}

void NoteCtrl::Data() {
	Profile& prof = GetProfile();
	
	SplitterHotfix(split, 2500);
	
	if (prof.notes.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.notes.GetCount(); i++) {
			int row = prof.notes.GetCount() - 1 - i;
			const Note& n = prof.notes[i];
			list.Set(row, 0, n.added);
			list.Set(row, 1, n.title);
		}
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectNote();
		split.SetPos(2500);
	}
}

void NoteCtrl::AddNote() {
	Profile& prof = GetProfile();
	Note& n = prof.notes.Add();
	n.added = GetSysTime();
	n.title = note.title.GetData();
	n.content = note.note.GetData();
	
	Data();
}

void NoteCtrl::SelectNote() {
	if (!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	int note_i = list.GetCount() - 1 - cursor;
	
	Profile& prof = GetProfile();
	const Note& n = prof.notes[note_i];
	note.title.SetData(n.title);
	note.note.SetData(n.content);
}

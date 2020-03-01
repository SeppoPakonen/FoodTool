#include "ActivityCtrl.h"

ExerciseEditorCtrl::ExerciseEditorCtrl() {
	CtrlLayout(*this);
	
	list.AddIndex();
	list.AddColumn("Execise");
	list.AddColumn("Main Area");
	list <<= THISBACK(SelectExercise);
	
	muscle_areas.AddColumn("Area");
	muscle_areas.AddColumn("Trained (0-10)");
	muscle_areas.ColumnWidths("3 1");
	muscle_areas <<= THISBACK(SelectMuscleArea);
	
	name <<= THISBACK(ValueChanged);
	instructions <<= THISBACK(ValueChanged);
	add <<= THISBACK(Add);
	name.WhenEnter = THISBACK(DbgDumpExercises);
	
	GetMuscleGroups(muscle_groups);
	trained.SetCount(muscle_groups.GetCount());
}

void ExerciseEditorCtrl::DbgDumpExercises() {
	Profile& prof = GetProfile();
	
	FileOut fout(GetDataFile("Exercises.txt"));
	
	for(int i = 0; i < prof.exercises.GetCount(); i++) {
		const ExerciseType& et = prof.exercises[i];
		if (et.name == "Unnamed") {prof.exercises.Remove(i--); continue;}
		fout << "name: " << ToUtf8(et.name.ToWString()) << "\n";
		fout << "bpm: " << et.av_heartrate.GetMean() << "\n";
		fout << "kcal: " << et.av_kcal.GetMean() << "\n";
		for(int j = 0; j < et.muscle_areas.GetCount(); j++) {
			int value = et.muscle_areas[j];
			if (value) {
				fout << et.muscle_areas.GetKey(j) << ": " << value << "\n";
			}
		}
		fout << "instruction: \"\"\"\n" << ToUtf8(et.instructions.ToWString()) << "\n\"\"\"\n";
		fout << "\n";
	}
	
	fout.Close();
	
	Data();
}

void ExerciseEditorCtrl::Data() {
	Profile& prof = GetProfile();
	
	if (prof.exercises.GetCount() != list.GetCount()) {
		for(int i = 0; i < prof.exercises.GetCount(); i++) {
			const ExerciseType& e = prof.exercises[i];
			list.Set(i, 0, i);
			list.Set(i, 1, e.name);
			list.Set(i, 2, e.primary_trained);
		}
		list.SetCount(prof.exercises.GetCount());
		list.SetSortColumn(1);
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectExercise();
	}
}

void ExerciseEditorCtrl::SelectExercise() {
	if (!list.IsCursor())
		return;
	
	Profile& prof = GetProfile();
	int cursor = list.GetCursor();
	int exercise_i = list.Get(cursor, 0);
	const ExerciseType& e = prof.exercises[exercise_i];
	
	name.SetData(e.name);
	instructions.SetData(e.instructions);
	
	for(int i = 0; i < muscle_groups.GetCount(); i++) {
		muscle_areas.Set(i, 0, muscle_groups[i]);
		
		EditIntSpin& edit = trained[i];
		edit.MinMax(0, 10);
		edit <<= THISBACK(ValueChanged);
		muscle_areas.SetCtrl(i, 1, edit);
		muscle_areas.Set(i, 1, e.muscle_areas.Get(muscle_groups.GetKey(i), 0));
	}
}

void ExerciseEditorCtrl::SelectMuscleArea() {
	
}

void ExerciseEditorCtrl::ValueChanged() {
	Profile& prof = GetProfile();
	int cursor = list.GetCursor();
	int exercise_i = list.Get(cursor, 0);
	ExerciseType& e = prof.exercises[exercise_i];
	
	e.name = name.GetData();
	e.instructions = instructions.GetData();
	
	String correct_format = UpperBegin(e.name);
	if (e.name.GetCount() && e.name != correct_format) {
		e.name = correct_format;
		name.SetData(correct_format);
	}
	
	VectorMap<String, int> primary_trained;
	for(int i = 0; i < muscle_areas.GetCount(); i++) {
		String key = muscle_groups.GetKey(i);
		int value = muscle_areas.Get(i, 1);
		e.muscle_areas.GetAdd(key) = value;
		primary_trained.Add(muscle_groups[i], value);
	}
	SortByValue(primary_trained, StdGreater<int>());
	e.primary_trained = primary_trained.GetKey(0);
	
	list.Set(cursor, 1, e.name);
	list.Set(cursor, 2, e.primary_trained);
}

void ExerciseEditorCtrl::Add() {
	Profile& prof = GetProfile();
	ExerciseType& e = prof.exercises.Add();
	
	e.added = GetSysTime();
	e.name = "Unnamed";
	Data();
}

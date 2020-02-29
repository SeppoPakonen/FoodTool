#ifndef _ActivityCtrl_ExerciseEditorCtrl_h_
#define _ActivityCtrl_ExerciseEditorCtrl_h_


struct ExerciseEditorCtrl : WithExerciseDetails<ParentCtrl> {
	Array<EditIntSpin> trained;
	VectorMap<String, String> muscle_groups;
	
	typedef ExerciseEditorCtrl CLASSNAME;
	ExerciseEditorCtrl();
	void Data();
	void SelectExercise();
	void SelectMuscleArea();
	void ValueChanged();
	void Add();
};

#endif

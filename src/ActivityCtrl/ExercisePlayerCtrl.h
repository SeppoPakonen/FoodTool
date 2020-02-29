#ifndef _ActivityCtrl_ExercisePlayerCtrl_h_
#define _ActivityCtrl_ExercisePlayerCtrl_h_

struct ExercisePlayerCtrl : public ParentCtrl {
	WithExerciseSelector<ParentCtrl> selector;
	WithExercisePlayer<ParentCtrl> player;
	WithHeartrateMeter<ParentCtrl> heartrate;
	int view = -1;
	ActivityGroupItem current;
	
	VectorMap<String, String> primary_types;
	VectorMap<String, String> muscle_groups;
	VectorMap<String, double> primary_weights;
	
	typedef enum {
		MODE_IDLE,
		MODE_EXERCISE,
		MODE_INTERVAL,
		MODE_TIMEOUT,
		MODE_HEARTRATE,
	} Mode;
	
	RunningFlag flag;
	TimeStop timeout_timer, exercise_timer, heartrate_timer;
	int timeout = 0, pulse_count = 0;
	Mode mode = MODE_IDLE, push_mode = MODE_IDLE;
	
	enum {
		SELECTOR,
		PLAYER,
		HEARTRATE
	};
	
	typedef ExercisePlayerCtrl CLASSNAME;
	ExercisePlayerCtrl();
	~ExercisePlayerCtrl() {Stop();}
	void Data(bool force);
	void SelectExercise();
	void SetView(int i);
	void PlaySound();
	String GetPrimaryType(const ActivityGroupItem& it);
	String GetPrimaryType(String exer_name);
	void Reset();
	void Start();
	void Timeout();
	void Stop();
	void AddExercise();
	void Pulse();
	void ProcessExercise();
	void PostSetView(int i) {PostCallback(THISBACK1(SetView, i));}
};

#endif

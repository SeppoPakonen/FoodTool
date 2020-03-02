#ifndef _ActivityCtrl_ExercisePlayerCtrl_h_
#define _ActivityCtrl_ExercisePlayerCtrl_h_

struct ExercisePlayerCtrl : public ParentCtrl {
	WithExerciseSelector<ParentCtrl> selector;
	WithExercisePlayer<ParentCtrl> player;
	WithHeartrateMeter<ParentCtrl> heartrate;
	WithTimeout<ParentCtrl> timeout;
	WithInterval<ParentCtrl> ival;
	int active_exer_i = -1, active_exer_item = 0;
	ActivityGroupItem current;
	
	VectorMap<String, String> primary_types;
	VectorMap<String, String> muscle_groups;
	VectorMap<String, double> primary_weights;
	
	static const int min_seconds = 30;
	static const int timeout_seconds = 15;
	static const int heartrate_pulses = 8;
	static const int default_kcal_per_hour = 964;
	static const int default_heartrate_per_hour = 160;
	
	typedef enum {
		MODE_IDLE,
		MODE_EXERCISE,
		MODE_INTERVAL,
		MODE_TIMEOUT,
		MODE_HEARTRATE,
	} Mode;
	
	RunningFlag flag;
	TimeStop timeout_timer, exercise_timer, heartrate_timer, interval_timer;
	int cur_timeout_seconds = 0, pulse_count = 0;
	Mode view_mode = MODE_IDLE, mode = MODE_IDLE, push_mode = MODE_IDLE;
	
	typedef ExercisePlayerCtrl CLASSNAME;
	ExercisePlayerCtrl();
	~ExercisePlayerCtrl() {Stop();}
	void Data(bool force);
	void SelectExercise();
	void SetView(Mode i);
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
	void PostSetView(Mode i) {PostCallback(THISBACK1(SetView, i));}
	void SetMode(Mode m) {mode = m; PostSetView(m);}
	void SetExercise(String title, String ins) {player.title.SetLabel(title); player.instructions.SetLabel(ins);}
	void SetUpcomingExercise(String title, String ins) {ival.upcoming.SetLabel(title); ival.instructions.SetLabel(ins);}
	void SetExerciseCountdown(int i) {player.seconds.SetLabel(Format(t_("%d seconds"), i));}
	void SetIntervalCountdown(int i) {ival.seconds.SetLabel(Format(t_("%d seconds"), i));}
	void SetTimeoutCountdown(int i) {timeout.seconds.SetLabel(Format(t_("%d seconds"), i));}
	void SetHeartrate(int bpm) {heartrate.bpm.SetLabel(bpm > 0 ? IntStr(bpm) : "-");}
	void DbgDumpExerciseWords();
};

String Translate(String s);
String TranslateExercise(String s);

#endif

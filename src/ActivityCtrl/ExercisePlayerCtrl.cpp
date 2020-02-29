#include "ActivityCtrl.h"

ExercisePlayerCtrl::ExercisePlayerCtrl() {
	CtrlLayout(selector);
	CtrlLayout(player);
	CtrlLayout(heartrate);
	
	Add(selector.SizePos());
	Add(player.SizePos());
	Add(heartrate.SizePos());
	
	SetView(SELECTOR);
	
	
	selector.list.AddIndex();
	selector.list.AddColumn("Primary type");
	selector.list.AddColumn("Kcal");
	selector.list.AddColumn("Average BPM");
	selector.list.ColumnWidths("3 1 1");
	selector.list <<= THISBACK(SelectExercise);
	selector.list.WhenLeftClick = THISBACK(SelectExercise);
	
	selector.items.AddIndex();
	selector.items.AddColumn("Primary Type");
	selector.items.AddColumn("Muscle Group");
	selector.items.AddColumn("Name");
	selector.items.AddColumn("Kcal");
	selector.items.AddColumn("BPM");
	selector.items.AddColumn("Length (s)");
	selector.items.ColumnWidths("2 2 3 1 1 1");
	
	selector.start <<= THISBACK(Start);
	selector.reset <<= THISBACK(Reset);
	
	player.timeout <<= THISBACK(Timeout);
	player.stop <<= THISBACK(Stop);
	
	heartrate.pulse <<= THISBACK(Pulse);
	
	GetMuscleGroups(muscle_groups);
	GetPrimaryTypes(primary_types);
	GetPrimaryTypeWeights(primary_weights);
}

void ExercisePlayerCtrl::Data(bool forced) {
	ArrayCtrl& list = selector.list;
	Profile& prof = GetProfile();
	Date today = GetSysTime();
	
	if (prof.planned_exercises.IsEmpty() || Date(prof.planned_exercises[0].main.begin) != today)
		Reset();
	
	if (prof.planned_exercises.GetCount() != selector.list.GetCount() || forced) {
		for(int i = 0; i < prof.planned_exercises.GetCount(); i++) {
			const ActivityGroupItem& it = prof.planned_exercises[i];
			list.Set(i, 0, i);
			list.Set(i, 1, GetPrimaryType(it));
			list.Set(i, 2, it.main.kcal);
			list.Set(i, 3, it.main.heartrate);
		}
		list.SetCount(prof.planned_exercises.GetCount());
		list.SetSortColumn(0);
		if (!list.IsCursor() && list.GetCount())
			list.SetCursor(0);
		SelectExercise();
	}
}

void ExercisePlayerCtrl::SelectExercise() {
	ArrayCtrl& list = selector.list;
	if (!list.IsCursor())
		return;
	
	const Profile& prof = GetProfile();
	int cursor = list.GetCursor();
	int pe_i = list.Get(cursor, 0);
	const ActivityGroupItem& it = prof.planned_exercises[pe_i];
	
	selector.primarytype.SetData(GetPrimaryType(it));
	selector.kcal = it.main.kcal;
	selector.heartrate = it.main.heartrate;
	selector.duration.SetData(GetTimeDurationString(it.main.begin, it.main.end));
	
	int row = 0;
	for(int i = 0; i < it.items.GetCount(); i++) {
		const ActivityItem& ai = it.items[i];
		if (ai.type != ACT_EXERCISE)
			continue;
		int exer_i = prof.FindExercise(ai.what);
		if (exer_i < 0) {
			selector.items.Set(i, 1, t_("ERROR"));
			continue;
		}
		const ExerciseType& et = prof.exercises[exer_i];
		
		selector.items.Set(row, 0, i);
		selector.items.Set(row, 1, GetPrimaryType(ai.what));
		
		VectorMap<String, int> primary_trained;
		for(int i = 0; i < et.muscle_areas.GetCount(); i++) {
			String key = et.muscle_areas.GetKey(i);
			int value = et.muscle_areas[i];
			primary_trained.Add(muscle_groups.Get(key, t_("ERROR")), value);
		}
		SortByValue(primary_trained, StdGreater<int>());
		selector.items.Set(row, 2, !primary_trained.IsEmpty() ? primary_trained.GetKey(0) : "");
		
		selector.items.Set(row, 3, et.name);
		selector.items.Set(row, 4, ai.kcal);
		selector.items.Set(row, 5, ai.heartrate);
		selector.items.Set(row, 6, GetTimeDurationString(ai.begin, ai.end));
		row++;
	}
	selector.items.SetCount(row);
	
}

void ExercisePlayerCtrl::SetView(int i) {
	selector.Hide();
	player.Hide();
	heartrate.Hide();
	
	switch (i) {
		case SELECTOR:
			selector.Show();
			selector.list.SetFocus();
			break;
		case PLAYER:
			player.Show();
			player.timeout.SetFocus();
			break;
		case HEARTRATE:
			heartrate.Show();
			heartrate.pulse.SetFocus();
			break;
	}
}

void ExercisePlayerCtrl::PlaySound() {
	PlayAlert();
}

String ExercisePlayerCtrl::GetPrimaryType(const ActivityGroupItem& it) {
	const Profile& prof = GetProfile();
	VectorMap<String, double> type_len;
	for(const ActivityItem& ai : it.items) {
		if (ai.type == ACT_EXERCISE) {
			int exer_i = prof.FindExercise(ai.what);
			if (exer_i < 0) continue;
			const ExerciseType& et = prof.exercises[exer_i];
			int train_sum = 0;
			for(int i = 0; i < et.muscle_areas.GetCount(); i++) train_sum += et.muscle_areas[i];
			for(int i = 0; i < et.muscle_areas.GetCount(); i++) {
				String key = et.muscle_areas.GetKey(i);
				int train = et.muscle_areas[i];
				if (train) {
					double kcal = (double)train / train_sum * ai.kcal;
					String ptype = primary_types.Get(key, t_("ERROR"));
					type_len.GetAdd(ptype, 0) += kcal;
				}
			}
		}
	}
	SortByValue(type_len, StdGreater<double>());
	if (type_len.IsEmpty())
		return t_("ERROR");
	return type_len.GetKey(0);
}

String ExercisePlayerCtrl::GetPrimaryType(String exer_name) {
	const Profile& prof = GetProfile();
	VectorMap<String, double> type_len;
	int exer_i = prof.FindExercise(exer_name);
	if (exer_i < 0)
		return t_("ERROR");
	const ExerciseType& et = prof.exercises[exer_i];
	int train_sum = 0;
	for(int i = 0; i < et.muscle_areas.GetCount(); i++) train_sum += et.muscle_areas[i];
	for(int i = 0; i < et.muscle_areas.GetCount(); i++) {
		String key = et.muscle_areas.GetKey(i);
		int train = et.muscle_areas[i];
		if (train) {
			double kcal = (double)train / train_sum;
			String ptype = primary_types.Get(key, t_("ERROR"));
			type_len.GetAdd(ptype, 0) += kcal;
		}
	}
	SortByValue(type_len, StdGreater<double>());
	if (type_len.IsEmpty())
		return t_("ERROR");
	return type_len.GetKey(0);
}

void ExercisePlayerCtrl::Reset() {
	Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	prof.planned_exercises.Clear();
	if (!conf.tgt_exercise_count || !conf.tgt_exercise_kcal || !conf.tgt_exercise_min)
		return;
	
	double weight_sum = 0;
	for(int i = 0; i < primary_weights.GetCount(); i++)
		weight_sum += primary_weights[i];
	
	VectorMap<String, Vector<String>> primary_muscles;
	for(int i = 0; i < primary_types.GetCount(); i++)
		primary_muscles.GetAdd(primary_types[i]).Add(primary_types.GetKey(i));
	
	Vector<double> tgt_muscle_kcals;
	tgt_muscle_kcals.SetCount(muscle_groups.GetCount(), 0);
	for(int i = 0; i < primary_muscles.GetCount(); i++) {
		String key = primary_muscles.GetKey(i);
		const Vector<String>& muscles = primary_muscles[i];
		double w = primary_weights.Get(key);
		double rel = w / weight_sum;
		double kcal_sum = conf.tgt_exercise_kcal * rel;
		double kcal_per_muscle = kcal_sum / muscles.GetCount();
		for(int j = 0; j < muscles.GetCount(); j++)
			tgt_muscle_kcals[muscle_groups.Find(muscles[j])] = kcal_per_muscle;
	}
	if (1) {
		for(int i = 0; i < tgt_muscle_kcals.GetCount(); i++) {
			LOG(muscle_groups[i] << ": " << tgt_muscle_kcals[i]);
		}
	}
	
	
	int max_seconds = 0;
	VectorMap<String, int> exercise_seconds;
	for(const ActivityGroupItem& gr : prof.activity) {
		for(const ActivityItem& ai : gr.items) {
			if (ai.type == ACT_EXERCISE) {
				int s = ai.end.Get() - ai.begin.Get();
				exercise_seconds.GetAdd(ai.what, 0) += s;
				if (s > max_seconds)
					max_seconds = s;
			}
		}
	}
	Vector<double> exercises_mul;
	exercises_mul.SetCount(prof.exercises.GetCount(), 1);
	if (max_seconds) {
		for(int i = 0; i < prof.exercises.GetCount(); i++)
			exercises_mul[i] = 1.0 - (double)exercise_seconds.Get(prof.exercises[i].name, 0) / max_seconds;
	}
	
	const int min_seconds = 15;
	
	Optimizer opt;
	opt.MinMax(0, 10*60);
	opt.Init(prof.exercises.GetCount(), 100);
	
	Vector<double> muscle_kcals;
	muscle_kcals.SetCount(muscle_groups.GetCount());
	
	Vector<double> best_exercises;
	double best_mul = 0, best_score = -DBL_MAX;
	while(!opt.IsEnd()) {
		opt.Start();
		const Vector<double>& trial = opt.GetTrialSolution();
		for(auto& d : muscle_kcals) d = 0;
		
		double total_kcal = 0;
		for(int i = 0; i < prof.exercises.GetCount(); i++) {
			const ExerciseType& et = prof.exercises[i];
			double av_kcal = et.av_kcal.GetMean();
			if (!av_kcal)
				av_kcal = 180;
			
			int seconds = trial[i];
			double exer_mul = exercises_mul[i];
			seconds *= exer_mul;
			if (seconds < min_seconds)
				continue;
			
			double kcal = av_kcal / 3600 * seconds;
			total_kcal += kcal;
			
			int training_sum = 0;
			for(int j = 0; j < et.muscle_areas.GetCount(); j++)
				training_sum += et.muscle_areas[j];
			if (!training_sum)
				continue;
			
			for(int j = 0; j < et.muscle_areas.GetCount(); j++) {
				int train = et.muscle_areas[j];
				if (train) {
					int group_i = muscle_groups.Find(et.muscle_areas.GetKey(j));
					ASSERT(group_i >= 0);
					muscle_kcals[group_i] += (double)train / training_sum * kcal;
				}
			}
		}
		
		double mul = conf.tgt_exercise_kcal / total_kcal;
		for(auto& d : muscle_kcals) d *= mul;
		
		double score = 0;
		for(int i = 0; i < muscle_kcals.GetCount(); i++) {
			double value = muscle_kcals[i];
			double target = tgt_muscle_kcals[i];
			double rel = value / target;
			score -= fabs(rel - 1.0);
		}
		
		opt.Stop(score);
		
		if (score > best_score) {
			best_score = score;
			best_exercises <<= trial;
			best_mul = mul;
		}
	}
	
	struct TimeSlot : Moveable<TimeSlot> {
		String exercise;
		int seconds;
	};
	Vector<TimeSlot> timeslots;
	timeslots.Reserve(1000);
	
	for(int i = 0; i < best_exercises.GetCount(); i++) {
		int seconds = best_exercises[i];
		double exer_mul = exercises_mul[i];
		seconds *= exer_mul * best_mul;
		if (seconds < min_seconds)
			continue;
		
		while (true) {
			TimeSlot& ts = timeslots.Add();
			ts.exercise = prof.exercises[i].name;
			if (seconds >= min_seconds * 2) {
				ts.seconds = min_seconds;
				seconds -= min_seconds;
			}
			else {
				ts.seconds = seconds;
				seconds = 0;
				break;
			}
		}
	}
	
	for(int i = 0; i < timeslots.GetCount(); i++)
		Swap(timeslots[i], timeslots[Random(timeslots.GetCount())]);
	
	Date today = GetSysTime();
	Time wake(today.year, today.month, today.day, conf.waking_hour, conf.waking_minute, 0);
	Time sleep(today.year, today.month, today.day, conf.sleeping_hour, conf.sleeping_minute, 0);
	int seconds_per_exer = conf.tgt_exercise_min * 60 / conf.tgt_exercise_count;
	int between_exer_seconds = (sleep.Get() - wake.Get() - seconds_per_exer) / (conf.tgt_exercise_count-1);
	int ts_i = 0;
	int per_exercise = timeslots.GetCount() / conf.tgt_exercise_count;
	int mod_exercise = timeslots.GetCount() % conf.tgt_exercise_count;
	for(int i = 0; i < conf.tgt_exercise_count; i++) {
		ActivityGroupItem& gr = prof.planned_exercises.Add();
		int per_this_exercise = per_exercise + (i < mod_exercise ? 1 : 0);
		
		gr.main.type = ACT_EXERCISE;
		gr.main.begin = wake + i * between_exer_seconds;
		
		VectorMap<String, int> exer_seconds;
		for(int j = 0; j < per_this_exercise; j++) {
			const TimeSlot& ts = timeslots[ts_i++];
			exer_seconds.GetAdd(ts.exercise, 0) += ts.seconds;
		}
		
		Time t = gr.main.begin;
		OnlineAverage1 total_av_heartrate;
		
		double total_kcal = 0;
		bool total_real_values = true;
		while (exer_seconds.GetCount()) {
			for(int j = 0; j < exer_seconds.GetCount(); j++) {
				String exercise = exer_seconds.GetKey(j);
				int& es = exer_seconds[j];
				int seconds = min(6 * min_seconds, es);
				es -= seconds;
				if (!es)
					exer_seconds.Remove(j--);
				
				const ExerciseType& et = prof.exercises[prof.FindExercise(exercise)];
				
				bool real_values = true;
				double av_kcal = et.av_kcal.GetMean();
				if (!av_kcal) {
					av_kcal = 180;
					real_values = false;
				}
				
				double av_heartrate = et.av_heartrate.GetMean();
				if (!av_heartrate) {
					av_heartrate = 120;
					real_values = false;
				}
				
				ActivityItem& it = gr.items.Add();
				it.type = ACT_EXERCISE;
				it.begin = t;
				t += seconds;
				it.end = t;
				it.what = exercise;
				it.msg = "";
				it.kcal = (double)av_kcal / 3600 * seconds;
				it.heartrate = av_heartrate;
				it.real_values = real_values;
				total_kcal += it.kcal;
				
				ActivityItem& ival = gr.items.Add();
				ival.type = ACT_INTERVAL;
				ival.begin = t;
				t += min_seconds;
				ival.end = t;
				ival.msg = "";
				ival.kcal = 0.5 * (double)av_kcal / 3600 * seconds; // pulse is still quite high
				ival.heartrate = 0.9 * av_heartrate;
				ival.real_values = real_values;
				total_kcal += ival.kcal; // ???
				
				total_av_heartrate.Add(av_heartrate);
				if (!real_values)
					total_real_values = false;
			}
		}
		
		gr.main.end = t;
		gr.main.what = "";
		gr.main.msg = "";
		gr.main.kcal = total_kcal;
		gr.main.heartrate = total_av_heartrate.GetMean();
		gr.main.real_values = total_real_values;
	}
	ASSERT(ts_i == timeslots.GetCount());
	
	Data(true);
}

void ExercisePlayerCtrl::Start() {
	Stop();
	
	flag.Start(1);
	current.items.Clear();
	current.main.type = ACT_EXERCISE;
	current.main.begin = GetSysTime();
	
	SetView(PLAYER);
	Thread::Start(THISBACK(ProcessExercise));
}

void ExercisePlayerCtrl::Timeout() {
	if (mode != MODE_TIMEOUT) {
		push_mode = mode;
		mode = MODE_TIMEOUT;
		timeout = 15;
		timeout_timer.Reset();
	}
	else {
		timeout += 15;
	}
}

void ExercisePlayerCtrl::Stop() {
	if (!flag.IsRunning())
		return;
	flag.Stop();
	
	if (current.items.GetCount()) {
		Profile& prof = GetProfile();
		AddExercise();
	}
}

void ExercisePlayerCtrl::AddExercise() {
	current.main.end = GetSysTime();
	current.main.what = "";
	current.main.msg = "";
	current.main.kcal = 0;
	current.main.real_values = true;
	OnlineAverage1 av;
	for(const ActivityItem& ai : current.items) {
		current.main.kcal += ai.kcal;
		av.Add(ai.heartrate);
		if (!ai.real_values)
			current.main.real_values = false;
	}
	current.main.heartrate = av.GetMean();
	
	Profile& prof = GetProfile();
	Swap(prof.activity.Add(), current);
	prof.StoreThis();

}

void ExercisePlayerCtrl::Pulse() {
	if (mode != MODE_HEARTRATE) return;
	
	if (!pulse_count) {
		heartrate_timer.Reset();
	}
	pulse_count++;
	
	if (pulse_count >= 8) {
		const Profile& prof = GetProfile();
		const Configuration& conf = prof.confs.Top();
		
		Date today = GetSysTime();
		const DailyPlan* plan = NULL, *it, *end;
		it = prof.planned_daily.Begin();
		end = prof.planned_daily.End();
		while (it != end) {
			if (it->date == today) {
				plan = it;
				break;
			}
		}
		if (!plan) return;
		
		double seconds = heartrate_timer.Elapsed() * 0.001;
		double pulse_duration = seconds / (pulse_count - 1);
		double bpm = 60.0 / pulse_duration;
		
		int c = current.items.GetCount();
		ASSERT(c >= 2);
		ActivityItem& prev_exer = current.items[c-2];
		ActivityItem& cur_ival = current.items[c-1];
		
		prev_exer.heartrate = bpm;
		prev_exer.kcal = GetHeartrateCalories(prof.is_male, plan->weight, conf.age, bpm, prev_exer.end.Get() - prev_exer.begin.Get());
		prev_exer.real_values = true;
		
		cur_ival.heartrate = bpm;
		cur_ival.kcal = GetHeartrateCalories(prof.is_male, plan->weight, conf.age, bpm, cur_ival.end.Get() - cur_ival.begin.Get());
		cur_ival.real_values = true;
		
		mode = MODE_INTERVAL;
		pulse_count = 0;
	}
}

void ExercisePlayerCtrl::ProcessExercise() {
	/*mode = MODE_IDLE;
	
		
		ActivityItem& ai = current.items.Add();
		ai.begin = now;
		ai.kcal = 0;
		ai.heartrate = 0;
		ai.real_values = false;
		etc.
	*/	
}

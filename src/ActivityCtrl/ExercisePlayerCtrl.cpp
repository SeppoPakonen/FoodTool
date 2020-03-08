#include "ActivityCtrl.h"

ExercisePlayerCtrl::ExercisePlayerCtrl() {
	CtrlLayout(selector);
	CtrlLayout(player);
	CtrlLayout(heartrate);
	CtrlLayout(timeout);
	CtrlLayout(ival);
	
	Add(selector.SizePos());
	Add(player.SizePos());
	Add(heartrate.SizePos());
	Add(timeout.SizePos());
	Add(ival.SizePos());
	
	SetView(MODE_IDLE);
	
	
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
	
	timeout.timeout <<= THISBACK(Timeout);
	timeout.stop <<= THISBACK(Stop);
	
	ival.timeout <<= THISBACK(Timeout);
	ival.stop <<= THISBACK(Stop);
	
	GetMuscleGroups(muscle_groups);
	GetPrimaryTypes(primary_types);
	GetPrimaryTypeWeights(primary_weights);
	
	DbgDumpExerciseWords();
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
			LOG(Format("%d : %X", i, (int64)&it));
			list.Set(i, 0, i);
			list.Set(i, 1, Translate(GetPrimaryType(it)));
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
	LOG(Format("Selected %d : %X", pe_i, (int64)&it));
	DUMPC(it.items);
	
	selector.primarytype.SetData(Translate(GetPrimaryType(it)));
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
		selector.items.Set(row, 1, Translate(GetPrimaryType(ai.what)));
		
		VectorMap<String, int> primary_trained;
		for(int i = 0; i < et.muscle_areas.GetCount(); i++) {
			String key = et.muscle_areas.GetKey(i);
			int value = et.muscle_areas[i];
			primary_trained.Add(muscle_groups.Get(key, "ERROR"), value);
		}
		SortByValue(primary_trained, StdGreater<int>());
		selector.items.Set(row, 2, !primary_trained.IsEmpty() ? Translate(primary_trained.GetKey(0)) : "");
		
		selector.items.Set(row, 3, TranslateExercise(et.name));
		selector.items.Set(row, 4, ai.kcal);
		selector.items.Set(row, 5, ai.heartrate);
		selector.items.Set(row, 6, GetTimeDurationString(ai.begin, ai.end));
		row++;
	}
	selector.items.SetCount(row);
	
}

void ExercisePlayerCtrl::SetView(Mode i) {
	selector.Hide();
	player.Hide();
	heartrate.Hide();
	timeout.Hide();
	ival.Hide();
	
	switch (i) {
		case MODE_IDLE:
			selector.Show();
			selector.list.SetFocus();
			break;
		case MODE_EXERCISE:
			player.Show();
			player.timeout.SetFocus();
			break;
		case MODE_HEARTRATE:
			heartrate.Show();
			heartrate.pulse.SetFocus();
			break;
		case MODE_TIMEOUT:
			timeout.Show();
			timeout.timeout.SetFocus();
			break;
		case MODE_INTERVAL:
			ival.Show();
			ival.timeout.SetFocus();
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
					String ptype = primary_types.Get(key, ("ERROR"));
					type_len.GetAdd(ptype, 0) += kcal;
				}
			}
		}
	}
	SortByValue(type_len, StdGreater<double>());
	if (type_len.IsEmpty())
		return ("ERROR");
	//LOG("\nGetPrimaryType Group");
	//DUMPM(type_len);
	return type_len.GetKey(0);
}

String ExercisePlayerCtrl::GetPrimaryType(String exer_name) {
	const Profile& prof = GetProfile();
	VectorMap<String, double> type_len;
	int exer_i = prof.FindExercise(exer_name);
	if (exer_i < 0)
		return ("ERROR");
	const ExerciseType& et = prof.exercises[exer_i];
	int train_sum = 0;
	for(int i = 0; i < et.muscle_areas.GetCount(); i++) train_sum += et.muscle_areas[i];
	for(int i = 0; i < et.muscle_areas.GetCount(); i++) {
		String key = et.muscle_areas.GetKey(i);
		int train = et.muscle_areas[i];
		if (train) {
			double kcal = (double)train / train_sum;
			String ptype = primary_types.Get(key, "ERROR");
			type_len.GetAdd(ptype, 0) += kcal;
		}
	}
	SortByValue(type_len, StdGreater<double>());
	if (type_len.IsEmpty())
		return ("ERROR");
	//LOG("\nGetPrimaryType Exercise " << exer_name);
	//DUMPM(type_len);
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
	
	
	int high_seconds = 0, low_seconds = INT_MAX;
	VectorMap<String, int> exercise_seconds;
	for(const ActivityGroupItem& gr : prof.activity) {
		for(const ActivityItem& ai : gr.items) {
			if (ai.type == ACT_EXERCISE) {
				int s = ai.GetSeconds();
				exercise_seconds.GetAdd(ai.what, 0) += s;
				if (s > high_seconds)
					high_seconds = s;
				if (s < low_seconds)
					low_seconds = s;
			}
		}
	}
	Vector<double> exercises_mul;
	exercises_mul.SetCount(prof.exercises.GetCount(), 1);
	if (high_seconds > low_seconds) {
		for(int i = 0; i < prof.exercises.GetCount(); i++)
			exercises_mul[i] = 1.0 - (double)(exercise_seconds.Get(prof.exercises[i].name, 0) - low_seconds) / (double)(high_seconds - low_seconds);
	}
	
	
	Optimizer opt;
	opt.MinMax(-10*60, 10*60);
	opt.Init(prof.exercises.GetCount(), 100);
	
	Vector<double> muscle_kcals;
	muscle_kcals.SetCount(muscle_groups.GetCount());
	
	int min_exer_and_ival = min_ival_seconds + min_exer_seconds;
	Vector<double> best_exercises;
	double best_mul = 0;
	double best_score = -DBL_MAX;
	while(!opt.IsEnd()) {
		opt.Start();
		const Vector<double>& trial = opt.GetTrialSolution();
		for(auto& d : muscle_kcals) d = 0;
		
		int active_exer_count = 0;
		double total_seconds = 0;
		double total_kcal = 0;
		for(int i = 0; i < prof.exercises.GetCount(); i++) {
			const ExerciseType& et = prof.exercises[i];
			double av_kcal = et.av_kcal.GetMean();
			if (!av_kcal)
				av_kcal = default_kcal_per_hour;
			
			int seconds = trial[i];
			double exer_mul = exercises_mul[i];
			seconds *= exer_mul;
			if (seconds < min_exer_and_ival *7)
				continue;
			
			double kcal = av_kcal / 3600 * seconds;
			
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
			
			total_kcal += kcal;
			total_seconds += seconds;
			active_exer_count++;
		}
		
		double av_kcal = total_kcal / total_seconds;
		int ival_seconds = active_exer_count * min_ival_seconds;
		double ival_kcal = ival_seconds * av_kcal;
		double mul = max(0.000001, (conf.tgt_exercise_kcal - ival_kcal) / total_kcal);
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
		seconds *= exer_mul;
		seconds *= best_mul;
		if (seconds < min_exer_and_ival)
			continue;
		
		while (true) {
			TimeSlot& ts = timeslots.Add();
			ts.exercise = prof.exercises[i].name;
			if (seconds >= min_exer_and_ival * 2) {
				ts.seconds = min_exer_and_ival;
				seconds -= min_exer_and_ival;
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
	VectorMap<String, String> exer_ptypes;
	for(int i = 0; i < conf.tgt_exercise_count; i++) {
		ActivityGroupItem& gr = prof.planned_exercises.Add();
		int per_this_exercise = per_exercise + (i < mod_exercise ? 1 : 0);
		
		gr.main.type = ACT_EXERCISE;
		gr.main.begin = wake + i * between_exer_seconds;
		
		VectorMap<String, int> exer_seconds;
		for(int j = 0; j < per_this_exercise; j++) {
			const TimeSlot& ts = timeslots[ts_i++];
			exer_seconds.GetAdd(ts.exercise, 0) += ts.seconds;
			if (exer_ptypes.Find(ts.exercise) < 0)
				exer_ptypes.Add(ts.exercise, GetPrimaryType(ts.exercise));
		}
		
		SortByValue(exer_seconds, StdLess<int>());
		
		String prev_ptype;
		for(int j = exer_seconds.GetCount()-1; j >= 0; j--) {
			String exer = exer_seconds.GetKey(j);
			int sec = exer_seconds[j];
			String ptype = exer_ptypes.Get(exer);
			if (prev_ptype == ptype) {
				bool found = false;
				for(int k = j-1; k >= 0; k--) {
					String repl_exer = exer_seconds.GetKey(k);
					String repl_ptype = exer_ptypes.Get(repl_exer);
					if (repl_ptype != prev_ptype) {
						int repl_sec = exer_seconds[k];
						
						// Swap
						exer_seconds.Remove(j);
						exer_seconds.Remove(k);
						exer_seconds.Insert(k, exer, sec);
						exer_seconds.Insert(j, repl_exer, repl_sec);
						
						prev_ptype = repl_ptype;
						found = true;
						break;
					}
				}
				if (!found)
					prev_ptype = ptype;
			}
			else
				prev_ptype = ptype;
		}
		
		
		Time t = gr.main.begin;
		
		// Add initial interval to prepare for the exercise
		ActivityItem& ival = gr.items.Add();
		ival.type = ACT_INTERVAL;
		ival.begin = t;
		t += min_ival_seconds;
		ival.end = t;
		ival.msg = "";
		ival.kcal = 0;
		ival.heartrate = 70;
		ival.real_values = false;
		
		OnlineAverage1 total_av_heartrate;
		double total_kcal = 0;
		while (exer_seconds.GetCount()) {
			for(int j = exer_seconds.GetCount()-1; j >= 0; j--) {
				String exercise = exer_seconds.GetKey(j);
				int& es = exer_seconds[j];
				int seconds;
				if (es >= min_exer_and_ival * 3)
					seconds = min_exer_and_ival * 1.5;
				else if (es >= min_exer_and_ival * 2)
					seconds = min_exer_and_ival;
				//else if (es >= min_exer_and_ival * 1.5)
				//	seconds = min_exer_and_ival * 0.75;
				else
					seconds = es;
				es -= seconds;
				if (!es)
					exer_seconds.Remove(j);
				
				const ExerciseType& et = prof.exercises[prof.FindExercise(exercise)];
				
				double av_kcal = et.av_kcal.GetMean();
				if (!av_kcal)
					av_kcal = default_kcal_per_hour;
				
				double av_heartrate = et.av_heartrate.GetMean();
				if (!av_heartrate)
					av_heartrate = default_heartrate_per_hour;
				
				int exer_seconds = seconds - min_ival_seconds;
				int ival_seconds = min_ival_seconds;
				ASSERT(exer_seconds >= min_exer_seconds);
				ASSERT(ival_seconds >= min_ival_seconds);
				
				ActivityItem& it = gr.items.Insert(1);
				it.type = ACT_EXERCISE;
				it.begin = t;
				t += exer_seconds;
				it.end = t;
				it.what = exercise;
				it.msg = "";
				it.kcal = (double)av_kcal / 3600 * exer_seconds;
				it.heartrate = av_heartrate;
				it.real_values = false;
				total_kcal += it.kcal;
				
				ActivityItem& ival = gr.items.Insert(2);
				ival.type = ACT_INTERVAL;
				ival.begin = t;
				t += ival_seconds;
				ival.end = t;
				ival.msg = "";
				ival.kcal = (double)av_kcal / 3600 * ival_seconds;
				ival.heartrate = av_heartrate;
				ival.real_values = false;
				total_kcal += ival.kcal; // ???
				
				total_av_heartrate.Add(av_heartrate);
			}
		}
		
		double tgt_per_exer = conf.tgt_exercise_kcal / conf.tgt_exercise_count;
		while (total_kcal > tgt_per_exer && gr.items.GetCount() >= 3) {
			gr.items.Remove(1, 2);
			
			total_kcal = 0;
			t = gr.main.begin;
			for(int i = 0; i < gr.items.GetCount(); i++) {
				ActivityItem& ai = gr.items[i];
				int s = ai.GetSeconds();
				ai.begin = t;
				t += s;
				ai.end = t;
				total_kcal += ai.kcal;
			}
		}
		
		gr.main.end = t;
		gr.main.what = Translate(GetPrimaryType(gr));
		gr.main.msg = "";
		gr.main.kcal = total_kcal;
		gr.main.heartrate = total_av_heartrate.GetMean();
		gr.main.real_values = false;
		
		
		// Dbg
		//gr.items.SetCount(min(3, gr.items.GetCount()));
	}
	ASSERT(ts_i == timeslots.GetCount());
	
	
	Data(true);
}

void ExercisePlayerCtrl::Start() {
	Stop();
	
	if (!selector.list.IsCursor())
		return;
	
	active_exer_i = selector.list.Get(selector.list.GetCursor(), 0);
	active_exer_item = 0;
	
	flag.Start(1);
	current.items.Clear();
	current.main.type = ACT_EXERCISE;
	current.main.begin = GetSysTime();
	
	Thread::Start(THISBACK(ProcessExercise));
}

void ExercisePlayerCtrl::Timeout() {
	if (mode != MODE_TIMEOUT) {
		push_mode = mode;
		cur_timeout_seconds = timeout_seconds;
		timeout_timer.Reset();
		SetMode(MODE_TIMEOUT);
	}
	else {
		cur_timeout_seconds += timeout_seconds;
	}
}

void ExercisePlayerCtrl::Stop() {
	if (!flag.IsRunning())
		return;
	flag.Stop();
	
	if (current.items.GetCount()) {
		int c = current.items.GetCount();
		if (c >= 3) {
			const ActivityItem& ai0 = current.items[c-1];
			const ActivityItem& ai1 = current.items[c-2];
			if (ai0.type == ACT_EXERCISE)
				current.items.Remove(c-1);
			else if (ai0.type == ACT_INTERVAL && ai1.type == ACT_EXERCISE && ai0.real_values == false)
				current.items.Remove(c-2, 2);
		}
		AddExercise();
	}
}

void ExercisePlayerCtrl::AddExercise() {
	if (current.items.IsEmpty()) return;
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
	
	if (prof.planned_exercises[active_exer_i].items.GetCount() == current.items.GetCount())
		prof.planned_exercises.Remove(active_exer_i);
	
	Swap(prof.activity.Add(), current);
	prof.StoreThis();
	
	PostCallback(THISBACK1(Data, true));
}

void ExercisePlayerCtrl::Pulse() {
	if (mode != MODE_HEARTRATE) return;
	
	if (!pulse_count) {
		heartrate_timer.Reset();
	}
	pulse_count++;
	
	if (pulse_count <= 1)
		PostCallback(THISBACK1(SetHeartrate, 0));
	else {
		double seconds = heartrate_timer.Elapsed() * 0.001;
		double pulse_duration = seconds / (pulse_count - 1);
		double bpm = 60.0 / pulse_duration;
		PostCallback(THISBACK1(SetHeartrate, (int)(bpm + 0.5)));
		
		if (pulse_count >= heartrate_pulses) {
			Profile& prof = GetProfile();
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
				it++;
			}
			if (!plan) return;
			
			
			int c = current.items.GetCount();
			
			if (c >= 2 && current.items[c-2].type == ACT_EXERCISE) {
				ActivityItem& prev_exer = current.items[c-2];
				int prev_exer_seconds = prev_exer.GetSeconds();
				prev_exer.heartrate = bpm;
				prev_exer.kcal = GetHeartrateCalories(prof.is_male, plan->weight, conf.age, bpm, prev_exer_seconds);
				prev_exer.real_values = true;
				
				double hour_kcal = prev_exer.kcal * 60*60 / prev_exer_seconds;
				ExerciseType& et = prof.exercises[prof.FindExercise(prev_exer.what)];
				et.av_heartrate.Add(bpm);
				et.av_kcal.Add(hour_kcal);
			}
			
			if (c >= 1 && current.items[c-1].type == ACT_INTERVAL) {
				ActivityItem& cur_ival = current.items[c-1];
				int ival_seconds = cur_ival.GetSeconds();
				cur_ival.heartrate = bpm;
				cur_ival.kcal = GetHeartrateCalories(prof.is_male, plan->weight, conf.age, bpm, ival_seconds);
				cur_ival.real_values = true;
			}
			
			pulse_count = 0;
			SetMode(MODE_INTERVAL);
		}
	}
}

void ExercisePlayerCtrl::ProcessExercise() {
	Profile& prof = GetProfile();
	ActivityGroupItem& plan = prof.planned_exercises[active_exer_i];
	mode = MODE_IDLE; // no gui change
	bool success = false;
	cur_timeout_seconds = 0;
	
	while (flag.IsRunning()) {
		
		if (active_exer_item >= plan.items.GetCount()) {
			success = true;
			break;
		}
		
		if (mode == MODE_IDLE) {
			ActivityItem& it = plan.items[active_exer_item];
			if (it.type == ACT_EXERCISE)
				SetMode(MODE_EXERCISE);
			else if (it.type == ACT_INTERVAL)
				SetMode(MODE_INTERVAL);
			else
				break;
		}
		else if (mode == MODE_EXERCISE) {
			ActivityItem& plan_it = plan.items[active_exer_item];
			if (plan_it.type != ACT_EXERCISE)
				break;
			
			int seconds = plan_it.GetSeconds() + cur_timeout_seconds;
			int left = max<int>(0, seconds - exercise_timer.Elapsed() / 1000);
			PostCallback(THISBACK1(SetExerciseCountdown, left));
			
			if (active_exer_item >= current.items.GetCount()) {
				ASSERT(active_exer_item == current.items.GetCount());
				ActivityItem& it = current.items.Add();
				it.type = plan_it.type;
				it.begin = GetSysTime();
				it.what = plan_it.what;
				it.msg = "";
				it.end = it.begin + plan_it.GetSeconds();
				it.heartrate = plan_it.heartrate;
				it.kcal = plan_it.kcal;
				it.real_values = false;
				exercise_timer.Reset();
				PlaySound();
				
				const ExerciseType& et = prof.exercises[prof.FindExercise(plan_it.what)];
				PostCallback(THISBACK2(SetExercise, TranslateExercise(et.name), et.instructions));
			}
			else {
				ActivityItem& it = current.items[active_exer_item];
				ASSERT(it.type == ACT_EXERCISE);
				
				if (left <= 0) {
					cur_timeout_seconds = 0;
					it.end = GetSysTime();
					active_exer_item++;
					mode = MODE_IDLE; // no gui change
					continue;
				}
			}
		}
		else if (mode == MODE_INTERVAL) {
			ActivityItem& plan_it = plan.items[active_exer_item];
			if (plan_it.type != ACT_INTERVAL)
				break;
			
			int seconds = plan_it.GetSeconds() + cur_timeout_seconds;
			int left = max<int>(0, seconds - interval_timer.Elapsed() / 1000);
			PostCallback(THISBACK1(SetIntervalCountdown, left));
			
			if (active_exer_item >= current.items.GetCount()) {
				ASSERT(active_exer_item == current.items.GetCount());
				ActivityItem& it = current.items.Add();
				it.type = plan_it.type;
				it.begin = GetSysTime();
				it.what = "";
				it.msg = "";
				it.end = it.begin + plan_it.GetSeconds();
				it.heartrate = plan_it.heartrate;
				it.kcal = plan_it.kcal;
				it.real_values = false;
				interval_timer.Reset();
				PlaySound();
				
				int upcoming_i = -1;
				if (active_exer_item + 1 < plan.items.GetCount()) {
					const ActivityItem& upcoming_it = plan.items[active_exer_item + 1];
					upcoming_i = prof.FindExercise(upcoming_it.what);
					if (upcoming_i >= 0) {
						const ExerciseType& et = prof.exercises[upcoming_i];
						PostCallback(THISBACK2(SetUpcomingExercise, TranslateExercise(et.name), et.instructions));
					}
				}
				if (upcoming_i < 0)
					PostCallback(THISBACK2(SetUpcomingExercise, "", ""));
				
				//int exer_i = prof.FindExercise(plan_it.what);
				//if (exer_i >= 0) {
				//	const ExerciseType& et = prof.exercises[exer_i];
				//	if (et.av_heartrate.count < 6) {
				heartrate_timer.Reset();
				PostCallback(THISBACK1(SetHeartrate, 0));
				SetMode(MODE_HEARTRATE);
				continue;
				//	}
				//}
			}
			else {
				ActivityItem& it = current.items[active_exer_item];
				ASSERT(it.type == ACT_INTERVAL);
				
				if (left <= 0) {
					cur_timeout_seconds = 0;
					it.end = GetSysTime();
					active_exer_item++;
					mode = MODE_IDLE; // no gui change
					continue;
				}
			}
		}
		else if (mode == MODE_HEARTRATE) {
			if (heartrate_timer.Elapsed() / 1000 > 60) {
				interval_timer.Reset();
				pulse_count = 0;
				SetMode(MODE_INTERVAL);
			}
		}
		else if (mode == MODE_TIMEOUT) {
			int left = max<int>(0, cur_timeout_seconds - timeout_timer.Elapsed() / 1000);
			PostCallback(THISBACK1(SetTimeoutCountdown, left));
			
			if (left <= 0) {
				PlaySound();
				SetMode(push_mode);
				continue;
			}
		}
		
		Sleep(100);
	}
	SetMode(MODE_IDLE);
	
	if (success)
		AddExercise();
	
	flag.IncreaseStopped();
}

void ExercisePlayerCtrl::DbgDumpExerciseWords() {
	const Profile& prof = GetProfile();
	Index<String> idx;
	for (const ExerciseType& et : prof.exercises) {
		Vector<String> v = Split(et.name, " ");
		for(String s : v)
			idx.FindAdd(s);
	}
	for(int i = 0; i < idx.GetCount(); i++) {
		LOG("t_(\"" << idx[i] << "\");");
	}
}

void DummyExerTrans() {
	t_("Anterior Deltoids");
	t_("Biceps");
	t_("Biceps Femoris");
	t_("Brachialis");
	t_("Deltoids");
	t_("Forearm Extensors");
	t_("Forearm Flexors");
	t_("Gastrocnemius");
	t_("Gluteus Maximus");
	t_("Inner Quadriceps");
	t_("Intercostals");
	t_("Latissimus Dorsi");
	t_("Lower Pectorals");
	t_("Lower Rectus Abdominis");
	t_("Medial Deltoids");
	t_("Obliques");
	t_("Outer Quadriceps");
	t_("Pectorals");
	t_("Quadriceps");
	t_("Rear Deltoids");
	t_("Rectus Abdominis");
	t_("Soleus");
	t_("Spinal Erectors");
	t_("Thighs");
	t_("Trapezius");
	t_("Triceps");
	t_("Upper Pectorals");
	t_("Upper Rectus Abdominis");
	
	t_("Shoulder");
	t_("Arm");
	t_("Leg");
	t_("Front");
	t_("Back");
	
	t_("Front");
	t_("Dumbbell");
	t_("Raises");
	t_("30");
	t_("Degree");
	t_("Seated");
	t_("Alternating");
	t_("Scaption");
	t_("Kneeling");
	t_("Prone");
	t_("On");
	t_("Swiss");
	t_("Ball");
	t_("Standing");
	t_("Alternate");
	t_("Curls");
	t_("Hammer");
	t_("Twisting");
	t_("Concentration");
	t_("Incline");
	t_("Single");
	t_("Leg");
	t_("Stiff-legged");
	t_("Deadlifts");
	t_("Romanian");
	t_("Reverse");
	t_("Shoulder");
	t_("Press");
	t_("Plate");
	t_("Pickup");
	t_("Overhead");
	t_("Lift");
	t_("Arnold");
	t_("Cross");
	t_("Punch");
	t_("Pec");
	t_("Deck");
	t_("Flyes");
	t_("Abduction");
	t_("Lateral");
	t_("Raise");
	t_("And");
	t_("Rear");
	t_("Barbell");
	t_("Wrist");
	t_("Hold");
	t_("Pinch");
	t_("Calf");
	t_("Butt");
	t_("Side");
	t_("In-place");
	t_("Lunge");
	t_("Fire");
	t_("Hydrant");
	t_("Sumo");
	t_("Squat");
	t_("Kettlebell");
	t_("Squats");
	t_("Step");
	t_("Ups");
	t_("With");
	t_("Walking");
	t_("Straight");
	t_("Kicks");
	t_("Donkey");
	t_("One");
	t_("Hip");
	t_("Shuffle");
	t_("Bench");
	t_("Flutter");
	t_("Teardrop");
	t_("Inner");
	t_("Quads");
	t_("Medicine");
	t_("Rotations");
	t_("Jump");
	t_("Rope");
	t_("Roman");
	t_("Chair");
	t_("Oblique");
	t_("Twists");
	t_("Thrusts");
	t_("Sprint");
	t_("Run");
	t_("Arm");
	t_("Bent-over");
	t_("Row");
	t_("Rows");
	t_("Pullovers");
	t_("Fly");
	t_("Pull-over");
	t_("Decline");
	t_("Knee");
	t_("Tucks");
	t_("Crunches");
	t_("To");
	t_("Chest");
	t_("Glute");
	t_("Bridge");
	t_("Plank");
	t_("Inclined");
	t_("Extension");
	t_("Rolling");
	t_("L-sit");
	t_("Legs");
	t_("Drive");
	t_("Diagonal");
	t_("Crunch");
	t_("Suitcase");
	t_("Rocky");
	t_("Solo");
	t_("Bends");
	t_("Jackknifes");
	t_("Side-ups");
	t_("Windshield");
	t_("Wiper");
	t_("High");
	t_("Side-to-side");
	t_("Hops");
	t_("Flat");
	t_("Pushups");
	t_("Pushups:");
	t_("In");
	t_("&");
	t_("Out");
	t_("Bulgarian");
	t_("Split");
	t_("Jumps");
	t_("Wall");
	t_("Sits");
	t_("Curl");
	t_("Rotation");
	t_("Slide");
	t_("Lunges");
	t_("Floor");
	t_("Work");
	t_("Lying");
	t_("Bicycle");
	t_("Sit-ups");
	t_("Sit-up");
	t_("V");
	t_("Knee-up");
	t_("Mountain");
	t_("Climbers");
	t_("Weighted");
	t_("Closed");
	t_("Toe");
	t_("Touches");
	t_("Lifts");
	t_("Ab");
	t_("Coaster");
	t_("Single-leg");
	t_("Ankle");
	t_("Mobilization");
	t_("Back");
	t_("Extension:");
	t_("Ground");
	t_("Clean");
	t_("Good");
	t_("Morning");
	t_("Snatch");
	t_("Jumping");
	t_("Jacks");
	t_("Step-ups");
	t_("Upright");
	t_("Shrugs");
	t_("Neck");
	t_("Extender");
	t_("Flexor");
	t_("Sideways");
	t_("One-arm");
	t_("Kickbacks");
	t_("Extensions");
	t_("Triceps");
	t_("Hindu");
	t_("Pushup");

}

// Translation uses "const char*" match, so keep permanent pointer during entire process
// lifecycle
String Translate(String s) {
	static ArrayMap<String, String> constant;
	String& c = constant.GetAdd(s, s);
	return t_GetLngString(c);
}

String TranslateExercise(String s) {
	Vector<String> v = Split(s, " ");
	String o;
	for(int i = 0; i < v.GetCount(); i++) {
		if (i) o << " ";
		o << Translate(v[i]);
	}
	return o;
}

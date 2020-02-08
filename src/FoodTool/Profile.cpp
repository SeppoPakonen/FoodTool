#include "FoodTool.h"

ProfileCreator::ProfileCreator() {
	CtrlLayout(*this, "Profile Creator");
	
	gender.SetData(1);
	height.SetData(176);
	weight.SetData(100);
	age.SetData(30);
	bodyfat.SetData(40);
	activity.SetData(0);
	walking_dist.SetData(0);
	tgt_weight.SetData(65);
	months.SetData(6);
	tgt_walking_dist.SetData(0);
	tgt_jogging_dist.SetData(0);
	shop_interval.SetData(5);
	
	UpdateTargetWeight();
	
	height <<= THISBACK(UpdateTargetWeight);
	next <<= THISBACK(Next);
}

void ProfileCreator::UpdateTargetWeight() {
	double height = (double)this->height.GetData() * 0.01;
	double bmi = 19;
	int weight = floorr(bmi * height * height, 3);
	tgt_weight.SetData(weight);
}

void ProfileCreator::Next() {
	Profile& prof = GetProfile();
	
	prof.storage.shop_interval = shop_interval.GetData();
	
	prof.is_male = gender.GetData();
	prof.height = height.GetData();
	prof.begin_weight = weight.GetData();
	prof.age = age.GetData();
	prof.bodyfat = bodyfat.GetData();
	prof.activity = activity.GetData();
	prof.walking_dist = walking_dist.GetData();
	prof.tgt_weight = tgt_weight.GetData();
	prof.months = months.GetData();
	prof.tgt_walking_dist = tgt_walking_dist.GetData();
	prof.tgt_jogging_dist = tgt_jogging_dist.GetData();
	prof.storage.shop_interval = shop_interval.GetData();
	
	double year_seconds = 31556926;
	double years = (double)prof.months / 12.0;
	int64 seconds = year_seconds * years;
	prof.begin_time = GetSysTime();
	prof.begin_time -= 24*60*60;
	prof.end_time = prof.begin_time + seconds;
	
	int days = (prof.end_time.Get() - prof.begin_time.Get()) / (24*60*60) + 1;
	
	double max_calory_deficit = 0.75;
	for (prof.calory_deficit = max_calory_deficit; prof.calory_deficit >= 0.01;) {
		if (prof.UpdatePlan(days+1))
			break;
		prof.calory_deficit -= 0.01;
	}
	prof.end_time = prof.begin_time + prof.planned_daily.GetCount() * 24*60*60;
	LOG("Calory-deficit: " << (prof.calory_deficit * 100) << "\%");
	LOG("Started " << Format("%", prof.begin_time));
	LOG("Stopping " << Format("%", prof.end_time));
	
	
	prof.is_initialised = true;
	
	prof.storage.Init(prof.begin_time);
	prof.storage.Update(prof.planned_daily);
	
	prof.StoreThis();
	
	Close();
}














Profile::Profile() {
	LoadThis();
	tmp_usage_start = GetSysTime();
	
	if (is_initialised) {
		if (storage.days.IsEmpty())
			storage.Init(begin_time);
		storage.Update(planned_daily);
	}
}

void Profile::AddWeightStat(int kgs, String jpg_file_path) {
	WeightLossStat& wl = weight.Add();
	wl.weight = kgs;
	wl.time = GetSysTime();
	wl.jpg_file = jpg_file_path;
	
	StoreThis();
}

Time Profile::GetCurrentTotalBegin() {
	return begin_time;
}

Time Profile::GetCurrentTotalEnd() {
	return end_time;
}

Time Profile::GetCurrentWeekBegin() {
	Time t = begin_time;
	Time now = GetSysTime();
	Time prev = t;
	while (1) {
		t += GetWeekSeconds();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

Time Profile::GetCurrentMonthBegin() {
	Time t = begin_time;
	Time now = GetSysTime();
	Time prev = t;
	while (1) {
		t += GetMonthSeconds();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

Time Profile::GetCurrentQuarterBegin() {
	Time t = begin_time;
	Time now = GetSysTime();
	Time prev = t;
	while (1) {
		t += 3 * GetMonthSeconds();
		if (t > now)
			break;
		prev = t;
	}
	return prev;
}

double Profile::GetBMR(double weight) {
	// Male: BMR = (10 × weight in kg) + (6.25 × height in cm) - (5 × age in years) + 5
	// Female: BMR = (10 × weight in kg) + (6.25 × height in cm) - (5 × age in years) - 161
	double bmr = (10.0 * weight) + (6.25 * height) - (5.0 * age);
	if (is_male)
		bmr += 5;
	else
		bmr -= 161;
	
	return bmr;
}

double Profile::GetTDEE() {
	switch (activity) {
		case 0: return 1.200;
		case 1: return 1.375;
		case 2: return 1.550;
		case 3: return 1.725;
		case 4: return 1.900;
		case 5: return 2.250;
		default: Panic("Invalid activity");
	}
	return 1.0;
}

double Profile::GetCaloriesMaintainWeight(double weight) {
	double bmr = GetBMR(weight);
	double tdee = GetTDEE();
	return bmr * tdee;
}

double Profile::GetCaloriesWalking(double weight_kg, double speed, double hours) {
	double bmr = GetBMR(weight_kg);
	
	// 0.76m/s = 2.3 MET
	// 1.34m/s = 3.3 MET
	double met = 2.3 + ((speed - 0.76) / (1.34 - 0.76)) * (3.3 - 2.3);
	
	// BMR (1,885.2) x METs (3.3) ÷ 24 x duration of activity in hours (1) = 259.2 calories
	double calories = bmr * met * hours / 24.0;
	return calories;
}

double Profile::GetCaloriesWalkingDist(double weight_kg, double distance_km, double hours) {
	double speed = distance_km * 1000.0 / (hours * 60.0 * 60.0);
	ASSERT(speed >= 0.0);
	return GetCaloriesWalking(weight_kg, speed, hours);
}

double Profile::GetCaloriesWalkingDistSpeed(double weight_kg, double distance_km, double speed) {
	double speed_kmh = speed * 60*60 / 1000.0;
	double hours = distance_km / speed_kmh;
	ASSERT(hours >= 0.0);
	return GetCaloriesWalking(weight_kg, speed, hours);
}

double Profile::GetCaloriesJogging(double weight_kg, double speed, double hours) {
	double bmr = GetBMR(weight_kg);
	
	// 2.50m/s = 8.8 MET
	// 3.03m/s = 11.2 MET
	double met = 8.8 + ((speed - 2.50) / (3.03 - 2.50)) * (11.2 - 8.8);
	
	// BMR (1,885.2) x METs (3.3) ÷ 24 x duration of activity in hours (1) = 259.2 calories
	double calories = bmr * met * hours / 24.0;
	return calories;
}

double Profile::GetCaloriesJoggingDistSpeed(double weight_kg, double distance_km, double speed) {
	double speed_kmh = speed * 60*60 / 1000.0;
	double hours = distance_km / speed_kmh;
	ASSERT(hours >= 0.0);
	return GetCaloriesJogging(weight_kg, speed, hours);
}

bool Profile::UpdatePlan(int min_days) {
	double weight = this->begin_weight;
	double walking_dist = this->walking_dist + this->tgt_walking_dist;
	double jogging_dist = this->tgt_jogging_dist;
	double walking_speed = 0.916; // m/s
	double jogging_speed = 2.500; // m/s
	double cals_in_kg_fat = 3500/0.453592;
	double tgt_fat_perc = 13;
	
	int calory_limit_reached = 0;
	double calory_limit_sum = 0.0;
	double max_calory_deficit = 0.0;
	
	planned_daily.SetCount(0);
	planned_daily.Reserve(2*365);
	while (weight > tgt_weight) {
		DailyPlan& d = planned_daily.Add();
		
		d.weight = weight;
		d.prog = 1.0 - (weight - tgt_weight) / (this->begin_weight - tgt_weight);
		d.fat_perc = d.prog * tgt_fat_perc + (1.0 - d.prog) * this->bodyfat * 0.01;
		d.fat_kgs = weight * d.fat_perc;
		d.lean_body_kgs = weight * (1.0 - d.fat_perc);
		
		d.maintain_calories = GetCaloriesMaintainWeight(weight);
		d.allowed_calories = d.maintain_calories * (1.0 - calory_deficit);
		
		double min_protein = d.lean_body_kgs * 0.8;
		double min_fat = 44.0 / 2000.0 * d.maintain_calories;
		double min_protein_cals = min_protein / ((18+17+15)/3.0) * ((87+75+66)/3.0);
		double min_fat_cals = min_fat / 100.0 * 900.0;
		
		double carb_cals = d.allowed_calories * 0.05;
		double prot_cals = d.allowed_calories * 0.20;
		double fat_cals =  d.allowed_calories - prot_cals - carb_cals;
		
		if (prot_cals < min_protein_cals || fat_cals < min_fat_cals) {
			if (prot_cals < min_protein_cals) prot_cals = min_protein_cals;
			if (fat_cals  < min_fat_cals)     fat_cals  = min_fat_cals;
			carb_cals = (prot_cals + fat_cals) / 0.95 * 0.05;
			double new_allowed_calories = carb_cals + prot_cals + fat_cals;
			if (new_allowed_calories > d.allowed_calories) {
				calory_limit_sum += new_allowed_calories - d.allowed_calories;
				calory_limit_reached++;
				d.allowed_calories = new_allowed_calories;
				double calory_deficit = 1.0 - (new_allowed_calories / d.maintain_calories);
				if (calory_deficit > max_calory_deficit)
					max_calory_deficit = calory_deficit;
			}
		}
		else if (calory_deficit > max_calory_deficit)
			max_calory_deficit = calory_deficit;
		
		d.maintain_burned_calories = d.maintain_calories * calory_deficit;
		if (walking_dist)
			d.walking_burned_calories = GetCaloriesWalkingDistSpeed(weight, walking_dist, walking_speed);
		else
			d.walking_burned_calories = 0;
		if (jogging_dist)
			d.jogging_burned_calories = GetCaloriesJoggingDistSpeed(weight, jogging_dist, jogging_speed);
		else
			d.jogging_burned_calories = 0;
		d.burned_calories = d.maintain_burned_calories + d.walking_burned_calories + d.jogging_burned_calories;
		d.burned_kgs = d.burned_calories / cals_in_kg_fat;
		
		d.food.grams = weight / 100.0 * 1000.0;
		d.food.kcals = d.allowed_calories;
		d.food.protein = min_protein;
		d.food.fat = max(min_fat, fat_cals / 9.0);
		d.food.carbs = carb_cals / 4.0;
		d.food.salt = 1.5;
		
		weight -= d.burned_kgs;
	}
	/*double calory_limit_av = calory_limit_reached ? calory_limit_sum / calory_limit_reached : 0;
	double limited = (double)calory_limit_reached / planned_daily.GetCount();
	if (calory_limit_av > 1)
		return false;*/
		
	if (planned_daily.GetCount() < min_days)
		return false;
	
	if (calory_deficit > max_calory_deficit)
		calory_deficit = max_calory_deficit;
	return true;
}





void Profile::LoadThis() {
	String file = ConfigFile("latest.bin");
	LoadFromFile(*this, file);
}

void Profile::StoreThis() {
	String tstr = Format("%", GetSysTime());
	tstr.Replace("/", "_");
	tstr.Replace(":", "_");
	tstr.Replace(" ", "_");
	String file = ConfigFile(tstr + ".bin");
	StoreToFile(*this, file);
	
	file = ConfigFile("latest.bin");
	StoreToFile(*this, file);
}

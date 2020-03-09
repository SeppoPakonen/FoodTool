#include "Food.h"


FoodStorage::FoodStorage() {
	
}

MealPreset& FoodStorage::AddMealPreset(String code) {
	return meal_types.Add(code);
}

void FoodStorage::Init(Date begin) {
	ASSERT(days.IsEmpty());
	FoodDay& first_day = days.Add();
	first_day.date = begin;
	first_day.is_shopping = true;
	first_day.total_consumed.Reset();
}

void FoodStorage::Update(bool replan, const Vector<DailyPlan>& planned_daily) {
	ASSERT_(days.GetCount(), "Run Init first");
	Profile& prof = GetProfile();
	Date today = GetSysTime();
	Date begin = today;
	
	int target_count = 7;
	int today_i = 0;
	for(int i = 0; i < days.GetCount(); i++) {
		const FoodDay& day = days[i];
		if (day.date == begin) {
			today_i = i;
			target_count = i + 7;
			break;
		}
	}
	
	int begin_i = today_i;
	if (!replan)
		begin_i = days.GetCount();
	
	Date d = GetProfile().begin_date;
	d += begin_i;
	days.SetCount(target_count);
	for(int i = begin_i; i < days.GetCount(); i++) {
		days[i].date = d++;
		if (i > 0)
			PlanDay(i, planned_daily);
	}
	for(int i = begin_i; i < days.GetCount(); i++) {
		if (days[i].is_shopping)
			PlanShopping(i, planned_daily);
	}
	
	prof.foodlog.queue.Clear();
	prof.shoplog.queue.Clear();
	
	for(int i = today_i; i < days.GetCount(); i++) {
		AddFoodQueue(i, planned_daily);
		if (days[i].is_shopping)
			AddShopQueue(i);
	}
	
}

void FoodStorage::AddFoodQuantity(const FoodQuantityInt& src, FoodQuantity& dst) {
	const Database& db = DB();
	for(int i = 0; i < src.GetCount(); i++) {
		int db_i = src.GetKey(i);
		int grams = src[i];
		if (grams > 0)
			dst.GetAdd(db_i, 0) += grams;
	}
}

Date FoodStorage::GetLastShopping() {
	Date today = GetSysTime();
	
	for(int i = days.GetCount()-1; i >= 0; i--) {
		if (days[i].date == today) {
			for(int j = i; j >= 0; j--) {
				if (days[j].is_shopping) {
					return days[j].date;
				}
			}
		}
	}
	
	return Time();
}

Date FoodStorage::GetNextShopping() {
	Date today = GetSysTime();
	
	for(int i = days.GetCount()-1; i >= 0; i--) {
		if (days[i].date == today) {
			for(int j = i+1; j < days.GetCount(); j++) {
				if (days[j].is_shopping) {
					return days[j].date;
				}
			}
		}
	}
	
	return Time();
}

String FoodStorage::GetTodaysMenu() {
	Date today = GetSysTime();
	
	for(int i = days.GetCount()-1; i >= 0; i--) {
		FoodDay& day = days[i];
		if (day.date == today)
			return day.menu;
	}
	
	return "<Error fetching today's menu>";
}

bool FoodStorage::NeedShopping() {
	
	
	return false;
}

void FoodStorage::DoShop() {
	
}

void FoodStorage::ConsumeDay() {
	
}

void FoodStorage::PlanDay(int i, const Vector<DailyPlan>& planned_daily) {
	const Database& db = DB();
	Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	int shop_interval = conf.shop_interval;
	
	FoodDay& prev = days[i-1];
	Ingredient target_diff = prev.target_sum - prev.total_sum;
	
	const DailyPlan& plan = planned_daily[i];
	FoodDay& day = days[i];
	ASSERT(plan.date == day.date);
	const Date& d = plan.date;
	day.wake_time = Time(d.year, d.month, d.day, conf.waking_hour, conf.waking_minute, 0);
	day.sleep_time = Time(d.year, d.month, d.day, conf.sleeping_hour, conf.sleeping_minute, 0);
	if (day.sleep_time < day.wake_time) day.sleep_time += 24*60*60;
	
	day.is_shopping = (i % shop_interval) == 0;
	day.used_food_amount <<= prev.used_food_amount;
	day.used_meal_amount <<= prev.used_meal_amount;
	day.mode = plan.mode;
	day.food_grams <<= prev.food_grams;
	for(int j = 0; j < prev.food_usage.GetCount(); j++) {
		double& grams = day.food_grams.GetAdd(prev.food_usage.GetKey(j), 0);
		grams -= prev.food_usage[j];
		if (grams < 0) grams = 0;
	}
	FindSetFoodStorageSnapshot(EndOfYesterday(day.date), day.food_grams);
	// Increase those nutritional values, which has not reached recommendation previously.
	// Don't let anything to be less than recommended.
	// Keep planned values in mass, energy, protein, fat, carbs and sodium.
	day.target_sum = plan.food;
	day.target_sum += target_diff; // add to today's target those, which totaled less than target previously
	day.target_sum.Limit();
	for(int j = 0; j < db.nutr_recom.GetCount(); j++) {
		const NutritionRecommendation& r = db.nutr_recom[j];
		double target = r.GetValue(plan.weight);
		auto& value = day.target_sum.nutr[r.nutr_no];
		if (r.nutr_no == SODIUM) // Sodium (salt) must be exact always
			value = target;
		else if (value < target) // Decrease in target is not allowed
			value = target;
		else if (value > 2*target)
			value = target;
	}
	day.target_sum.grams = plan.food.grams;
	day.target_sum.nutr[KCAL] = plan.food.nutr[KCAL];
	day.target_sum.nutr[PROT] = plan.food.nutr[PROT];
	day.target_sum.nutr[FAT]  = plan.food.nutr[FAT];
	day.target_sum.nutr[CARB] = plan.food.nutr[CARB];
	
	/*for(int j = 0; j < db.nutr_types.GetCount(); j++) {
		const NutritionType& t = db.nutr_types[j];
		LOG(t.nutr_desc << ": " << orig_target_sum.nutr[j]);
	}*/
	
	Ingredient remaining(day.target_sum);
	day.ResetPlan();
	
	double target_fat = remaining.nutr[FAT];
	
	// Do supplements first with low calories
	if (remaining.nutr[KCAL] <= 1000) {
		/*double supplement_kcal = min<double>(100.0, remaining.nutr[KCAL]);
		MakeSupplements(plan, day, supplement_kcal, remaining);
		
		if (remaining.nutr[KCAL] >= 50)
			MakeMenu(plan, day, remaining.nutr[KCAL], remaining);*/
		MakeMenu(plan, day, remaining.nutr[KCAL] - 50, remaining);
		MakeSupplements(plan, day, remaining.nutr[KCAL], remaining);
	}
	else {
		MakeMenu(plan, day, remaining.nutr[KCAL] - 400, remaining);
		MakeSupplements(plan, day, remaining.nutr[KCAL], remaining);
	}
	
	day.total_consumed = prev.total_consumed;
	day.total_consumed += day.total_sum;
	
	if (0) {
		double day_target_kcals_sum = day.target_sum.nutr[KCAL];
		double day_kcals_sum = day.total_sum.nutr[KCAL];
		double day_kcals_target_sum = plan.food.nutr[KCAL];
		
		VectorMap<int, double> max_diff;
		for(int j = 0; j < prof.planned_nutrients.GetCount(); j++) {
			int k = prof.planned_nutrients[j];
			const NutritionType& t = db.nutr_types[k];
			double tgt = day.target_sum.nutr[k];
			if (tgt > 0) {
				/*double diff = fabs(day.total_sum.nutr[k] / tgt - 1);
				max_diff.Add(k, diff);*/
				double diff = day.total_sum.nutr[k] / tgt - 1;
				if (diff < 0)
					max_diff.Add(k, fabs(diff));
			}
		}
		SortByValue(max_diff, StdGreater<double>());
		for(int j = 0; j < max_diff.GetCount(); j++) {
			int k = max_diff.GetKey(j);
			double value = max_diff[j];
			const NutritionType& t = db.nutr_types[k];
			LOG(j << ": " << t.nutr_desc << ": " << value << ": " << day.target_sum.nutr[k] << " = " << target_diff.nutr[k]);
		}
	}
	
	day.preparation << t_("Kcals/day: ") << day.total_sum.nutr[KCAL] << "\n";
	day.preparation << t_("Fat g/day: ") << day.total_sum.nutr[FAT] << "\n";
	day.preparation << t_("Protein g/day: ") << day.total_sum.nutr[PROT] << "\n";
	day.preparation << t_("Carbs g/day: ") << day.total_sum.nutr[CARB] << "\n";
	
	static int nutrilet_i;
	if (!nutrilet_i)
		nutrilet_i = db.FindFood("Nutrilet");
	if (nutrilet_i >= 0) {
		Ingredient nutri_ing;
		nutri_ing.Reset();
		const FoodDescription& fd = db.food_descriptions[nutrilet_i];
		OnlineAverage1 av;
		for(const NutritionInfo& ni : fd.nutr) {
			double a = day.total_sum.nutr[ni.nutr_no];
			double b = ni.nutr_value;
			double ratio = min(2.0, a / b);
			//if (ni.nutr_no == KCAL || ni.nutr_no == FAT || ni.nutr_no == PROT) continue;
			av.Add(ratio);
		}
		day.preparation << Format(t_("Nutrilet factor: %2n"), av.GetMean()) << "\n";
	}
}

void FoodStorage::PlanShopping(int day_i, const Vector<DailyPlan>& planned_daily) {
	const Database& db = DB();
	const DailyPlan& plan = planned_daily[day_i];
	FoodDay& day = days[day_i];
	
	day.buy_amount.Clear();
	
	if (!day.is_shopping)
		return;
	
	FoodDay* prev = &day;
	FoodQuantity total_usage;
	for(int i = day_i+1; i < days.GetCount(); i++) {
		FoodDay& day = days[i];
		day.used_food_amount <<= prev->used_food_amount;
		for(int j = 0; j < day.food_usage.GetCount(); j++) {
			int db_i = day.food_usage.GetKey(j);
			double usage = day.food_usage[j];
			total_usage.GetAdd(db_i, 0) += usage;
			day.used_food_amount.GetAdd(db_i, 0) += usage;
		}
		prev = &day;
		if (day.is_shopping)
			break;
	}
	
	
	// Get how much old food one has after the shopping day
	FoodQuantity existing_food_grams;
	existing_food_grams <<= day.food_grams;
	for(int j = 0; j < day.food_usage.GetCount(); j++)
		existing_food_grams.GetAdd(day.food_usage.GetKey(j), 0) -= day.food_usage[j];
	
	// Check what food products must be bought
	for(int i = 0; i < total_usage.GetCount(); i++) {
		int db_i = total_usage.GetKey(i);
		double usage = total_usage[i];
		double already = existing_food_grams.Get(db_i, 0);
		double must_buy = usage - already;
		int buy_count = 0;
		double bought_sum = 0;
		
		const FoodDescription& d = db.food_descriptions[db_i];
		
		if (must_buy >= 1.0) {
			day.buy_amount.Add(db_i, bought_sum + 0.5);
		}
	}
	
	//day.buy_amount.GetAdd(db.FindFood("Soy protein isolate"), 0) += 100;
	
	prev = &day;
	for(int i = day_i+1; i < days.GetCount(); i++) {
		FoodDay& day = days[i];
		
		day.food_grams <<= prev->food_grams;
		for(int j = 0; j < prev->food_usage.GetCount(); j++)
			day.food_grams.GetAdd(prev->food_usage.GetKey(j), 0) -= prev->food_usage[j];
		
		if (i == day_i+1)
			AddFoodQuantity(prev->buy_amount, day.food_grams);
		
		FindSetFoodStorageSnapshot(EndOfYesterday(day.date), day.food_grams);
		
		/*for(int j = 0; j < day.food_usage.GetCount(); j++) {
			int db_i = day.food_usage.GetKey(j);
			ASSERT(day.food_grams.Find(db_i) >= 0);
		}*/
		
		prev = &day;
		if (day.is_shopping)
			break;
	}
}

void FoodStorage::MakeSupplements(const DailyPlan& plan, FoodDay& day, double target_kcal, Ingredient& remaining) {
	const Database& db = DB();
	const Configuration& conf = GetProfile().confs.Top();
	Profile& prof = GetProfile();
	
	bool dbg = 0;
	int dbg_no = db.FindFoodLeft("Rainbow 72g monivitamiini");
	
	Ingredient tmp;
	int limit_count = 0;
	Vector<bool> limit_nutrition;
	Index<int> filled_nutrients;
	VectorMap<int, OnlineAverage1> supplement_foods;
	for(int i = 0; i < prof.supplements.GetCount(); i++) {
		const auto& s = prof.supplements[i];
		if ((plan.variant_type == VARIANT_WEIGHTLOSS && s.is_weightloss) ||
			(plan.variant_type != VARIANT_WEIGHTLOSS && s.is_maintenance)) {
			const NutritionRecommendation& recom = db.nutr_recom[i];
			int nutr_no = recom.nutr_no;
			double target = remaining.nutr[nutr_no];
			if (target > 0) {
				for(const int db_no : s.used_food) {
					const FoodDescription& d = db.food_descriptions[db_no];
					tmp.Set(1, d);
					double value = tmp.nutr[nutr_no];
					double rel = value / target;
					double grams = 1.0 / rel;
					supplement_foods.GetAdd(db_no).Add(grams);
					
					if (dbg && db_no == dbg_no) {
						LOG("DBG: " << db.nutr_types[nutr_no].nutr_desc << " = " << grams);
					}
				}
				filled_nutrients.Add(nutr_no);
				
				bool limit = true;
				//if (recom.group == AMINOACID)
				//	limit = false;
				limit_nutrition.Add(limit);
				if (limit)
					limit_count++;
			}
		}
	}
	
	if (filled_nutrients.IsEmpty())
		return;
	
	if (dbg) {
		for(int i = 0; i < supplement_foods.GetCount(); i++) {
			const FoodDescription& d = db.food_descriptions[supplement_foods.GetKey(i)];
			LOG("Food " << i << ": " << d.long_desc << " " << supplement_foods[i].GetMean());
		}
	}
	
	Optimizer opt;
	opt.Min().SetCount(supplement_foods.GetCount());
	opt.Max().SetCount(supplement_foods.GetCount());
	for(int i = 0; i < supplement_foods.GetCount(); i++) {
		double fill_mean = supplement_foods[i].GetMean();
		opt.Min()[i] = -fill_mean * 2;
		opt.Max()[i] = +fill_mean * 2;
	}
	opt.SetMaxGenerations(100);
	opt.Init(supplement_foods.GetCount(), 100);
	
	double best_score = -DBL_MAX;
	double best_mul = 0;
	Vector<double> best;
	Ingredient opt_nutr_sum;
	while (!opt.IsEnd()) {
		opt.Start();
		
		opt_nutr_sum.Reset();
		const Vector<double>& trial = opt.GetTrialSolution();
		for(int i = 0; i < supplement_foods.GetCount(); i++) {
			const FoodDescription& d = db.food_descriptions[supplement_foods.GetKey(i)];
			double grams = max(0.0, trial[i]);
			if (grams > 0.0) {
				tmp.Set(grams, d);
				opt_nutr_sum += tmp;
			}
		}
		
		double kcal = opt_nutr_sum.nutr[KCAL];
		double mul = 1;
		if (kcal > target_kcal) {
			mul = target_kcal / kcal;
			opt_nutr_sum *= mul;
		}
		
		// Partial score: how well protein is filled
		double protein_score = 0;
		{
			double target = remaining.nutr[PROT];
			if (target > 0) {
				double value = opt_nutr_sum.nutr[PROT];
				double rel = max(1.0, value / target) - 1;
				protein_score = -rel;
			}
		}
		
		// Partial score: how well PUFA is filled. It's required for insulin reduction
		double pufa_score = 0;
		{
			double target = remaining.nutr[PUFA];
			if (target > 0) {
				double value = opt_nutr_sum.nutr[PUFA];
				double rel = min(1.0, value / target);
				pufa_score = rel;
			}
		}
		
		// Partial score: how well all nutrition targets are filled
		double target_fill_score = 0;
		for(int nutr_no : filled_nutrients) {
			double target = remaining.nutr[nutr_no];
			if (target > 0) {
				double value = opt_nutr_sum.nutr[nutr_no];
				double rel = min(1.0, value / target);
				target_fill_score += rel;
			}
		}
		target_fill_score /= filled_nutrients.GetCount();
		
		// Partial score: how nutrition targets are exceeded
		double target_exceed_score = 0;
		if (limit_count) {
			for(int i = 0; i < filled_nutrients.GetCount(); i++) {
				if (limit_nutrition[i]) {
					int nutr_no = filled_nutrients[i];
					double target = remaining.nutr[nutr_no];
					if (target > 0) {
						double value = opt_nutr_sum.nutr[nutr_no];
						double rel = max(0.0, value / target - 1.0);
						target_exceed_score -= rel;
					}
				}
			}
			target_exceed_score /= limit_count;
		}
		
		// Partial score: limit fat intake
		double fat_exceed_score = 0;
		{
			double target = remaining.nutr[FAT];
			if (target > 0) {
				double value = opt_nutr_sum.nutr[FAT];
				double rel = max(0.0, value / target - 1.0);
				fat_exceed_score = -rel;
			}
		}
		
		
		// Get full score
		double score =
			 100 * protein_score +
			 100 * pufa_score +
			 100 * fat_exceed_score +
			  10 * target_fill_score +
			   3 * target_exceed_score
			;
		
		//LOG(opt.GetRound() << ": " << score);
		
		if (score > best_score) {
			best_score = score;
			best_mul = mul;
			best <<= trial;
		}
		opt.Stop(score);
	}
	
	for(int i = 0; i < supplement_foods.GetCount(); i++) {
		int db_no = supplement_foods.GetKey(i);
		const FoodDescription& d = db.food_descriptions[db_no];
		double grams = max(0.0, best[i] * best_mul);
		if (grams > 0.0) {
			day.supplement_usage.GetAdd(db_no) = grams;
			tmp.Set(grams, d);
			day.supplement_sum += tmp;
			
			day.menu << d.long_desc << ": " << Format(t_("%4n grams"), grams) << "\n";
		}
	}
	
	remaining -= day.supplement_sum;
	remaining.Limit();
	day.total_sum += day.supplement_sum;
}

void FoodStorage::MakeMenu(const DailyPlan& plan, FoodDay& day, double target_kcal, Ingredient& remaining) {
	ASSERT(remaining.nutr[FAT]  >= 0 && remaining.nutr[FAT]  < 2000);
	ASSERT(remaining.nutr[CARB] >= 0 && remaining.nutr[CARB] < 2000);
	ASSERT(remaining.nutr[PROT] >= 0 && remaining.nutr[PROT] < 2000);
	const Database& db = DB();
	const Configuration& conf = GetProfile().confs.Top();
	Profile& prof = GetProfile();
	Time now = GetSysTime();
	Time t = day.wake_time;
	FoodQuantity food_left;
	food_left <<= day.food_grams;
	
	int different_mealtypes = 1;
	Time prev_different_start = t;
	Vector<int> meal_presets;
	while (t <= day.sleep_time) {
		Meal& m = day.meals.Add();
		m.time = t;
		if (t >= prev_different_start + conf.hours_between_making_meals * 60 * 60) {
			different_mealtypes++;
			prev_different_start = t;
		}
		meal_presets.Add(different_mealtypes-1);
		
		t += conf.hours_between_meals * 60 * 60;
	}
	
	Ingredient meal_type_target = remaining;
	meal_type_target.nutr[KCAL] = target_kcal;
	meal_type_target *= 1.0 / different_mealtypes;
	int prev_preset_i = -1;
	double kcal_per_mealtype = target_kcal / different_mealtypes;
	for(int i = 0; i < different_mealtypes; i++) {
		int preset_i = FindBestMeal(plan.weight, kcal_per_mealtype, plan.variant_type, food_left, day.used_meal_amount, meal_type_target);
		//if (prof.presets.GetCount() > 1) do {preset_i = Random(prof.presets.GetCount());} while (preset_i == prev_preset_i);
		prev_preset_i = preset_i;
		const MealPreset& mp = prof.presets[preset_i];
		const MealPresetVariant& var = mp.variants[plan.variant_type];
		Ingredient ing;
		var.GetNutritions(ing);
		double mealtype_grams = ing.grams * kcal_per_mealtype / ing.nutr[KCAL];
		day.food_sum.Add(mealtype_grams, ing);
		
		Index<int> meals;
		for(int j = 0; j < meal_presets.GetCount(); j++)
			if (meal_presets[j] == i)
				meals.Add(j);
		
		double kcal_per_meal = kcal_per_mealtype / meals.GetCount();
		double portion_grams = ing.grams * kcal_per_meal / ing.nutr[KCAL];
		
		for(int j = 0; j < meals.GetCount(); j++) {
			Meal& m = day.meals[meals[j]];
			m.grams = portion_grams;
			m.key = mp.key;
		}
		day.menu << Format(t_("Food #%d: %s %d * %d grams\n\n"), i+1, mp.name, meals.GetCount(), (int)portion_grams);
		
		double mul = mealtype_grams / ing.grams;
		for(int j = 0; j < var.ingredients.GetCount(); j++) {
			const MealIngredient& mi = var.ingredients[j];
			double grams = mul * mi.max_grams;
			day.food_usage.GetAdd(mi.db_food_no, 0) += grams;
			
			double& left = food_left.GetAdd(mi.db_food_no, 0);
			left = max(0.0, left - grams);
		}
		
		day.used_meal_amount.GetAdd(mp.key, 0)++;
	}
	
	remaining -= day.food_sum;
	remaining.Limit();
	day.total_sum += day.food_sum;
}

int FindBestMeal(double weight, double kcal, int variant_type, const FoodQuantity& food_left, const VectorMap<String, int>& used_meal_amount, const Ingredient& target_sum) {
	const Database& db = DB();
	const Profile& prof = GetProfile();
	VectorMap<int, double> preset_scores;
	Vector<int> usages;
	int max_usage_before = 1;
	
	for(int i = 0; i < prof.presets.GetCount(); i++) {
		const MealPreset& mp = prof.presets[i];
		int usage_before = used_meal_amount.Get(mp.key, 0);
		usages.Add(usage_before);
		max_usage_before = max(max_usage_before, usage_before);
	}
	
	for(int i = 0; i < prof.presets.GetCount(); i++) {
		const MealPreset& mp = prof.presets[i];
		const MealPresetVariant& var = mp.variants[variant_type];
		
		Ingredient ing;
		var.GetNutritions(ing);
		double mul = kcal / ing.nutr[KCAL];
		
		int local_usage_count = 0;
		double local_usage_mass = 0, local_usage_required = 0;
		for(int j = 0; j < var.ingredients.GetCount(); j++) {
			const MealIngredient& mi = var.ingredients[j];
			const FoodDescription& d = db.food_descriptions[mi.db_food_no];
			double grams = mi.max_grams * mul;
			if (!grams)
				continue;
			double left = food_left.Get(mi.db_food_no, 0);
			if (grams <= left)
				local_usage_count++;
			local_usage_mass += min(left, grams);
			local_usage_required += grams;
		}
		//double local_usage_factor = (double)local_usage_count / var.ingredients.GetCount();
		double local_usage_factor = local_usage_mass / local_usage_required;
		double super_match = 0;
		//if (local_usage_factor >= 0.9) super_match = 1000000;
		
		int usage_before = usages[i];
		double existing_usage_factor = 1.0 - (double)usage_before / max_usage_before;
		
		double mass_score = 0;
		if (variant_type == VARIANT_WEIGHTLOSS)
			mass_score = min(0.0, var.mass_factor - 3.0);
		
		double prot_score = 0;
		double target_score = 0;
		for(const NutritionRecommendation& r : db.nutr_recom) {
			double value = ing.nutr[r.nutr_no] * mul;
			double target = target_sum.nutr[r.nutr_no];// r.GetValue(weight);
			if (target > 0.0) {
				target_score += min(1.0, value / target);
				if (r.nutr_no == PROT)
					prot_score = -(max(1.0, value / target) - 1.0);
			}
			else if (r.nutr_no == PROT)
				prot_score = -(max(1.0, value / 1) - 1.0);
		}
		target_score /= db.nutr_recom.GetCount();
		
		if (mp.wished_factor) {
			int i = mp.wished_factor;
		}
		double score =
			 10 * local_usage_factor +
			      var.score +
			 10 * existing_usage_factor +
			 10 * mass_score +
			 10 * target_score +
			 10 * prot_score +
			 10 * mp.wished_factor +
			super_match
			 ;
		preset_scores.Add(i, score);
	}
	
	SortByValue(preset_scores, StdGreater<double>());
	return preset_scores.GetKey(0);
}

void FoodStorage::AddFoodQueue(int day_i, const Vector<DailyPlan>& planned_daily) {
	const FoodDay& day = days[day_i];
	const DailyPlan& plan = planned_daily[day_i];
	const Profile& prof = GetProfile();
	Vector<FoodPrice>& out_vec = GetProfile().foodlog.queue;
	
	VectorMap<String, double> meal_types;
	VectorMap<String, Time> meal_times;
	for(int i = 0; i < day.meals.GetCount(); i++) {
		const Meal& m = day.meals[i];
		meal_types.GetAdd(m.key, 0) += m.grams;
		meal_times.GetAdd(m.key, m.time);
	}
	
	if (day.supplement_usage.GetCount()) {
		FoodPrice& out = out_vec.Add();
		out.time = day.wake_time;
		for(int i = 0; i < day.supplement_usage.GetCount(); i++) {
			int db_no = day.supplement_usage.GetKey(i);
			double grams = day.supplement_usage[i];
			if (grams > 0.0) {
				FoodPriceQuote& quote = out.values.Add(db_no);
				int k = prof.price.history.Find(db_no);
				if (k >= 0 && prof.price.history[k].GetCount()) {
					const FoodPriceQuote& prev_quote = prof.price.history[k].Top();
					quote.Set(out.time, grams, prev_quote);
				}
				else {
					quote.SetPriceless(out.time, grams);
				}
			}
		}
	}
	
	for(int i = 0; i < meal_types.GetCount(); i++) {
		int preset_i = prof.FindMealPreset(meal_types.GetKey(i));
		if (preset_i < 0) continue;
		double grams = meal_types[i];
		Time time = meal_times[i];
		const MealPreset& mp = prof.presets[preset_i];
		const MealPresetVariant& var = mp.variants[plan.variant_type];
		Ingredient ing;
		var.GetNutritions(ing);
		double mul = grams / ing.grams;
		
		FoodPrice& out = out_vec.Add();
		out.time = time;
		for(int j = 0; j < var.ingredients.GetCount(); j++) {
			const MealIngredient& mi = var.ingredients[j];
			double grams = mi.max_grams * mul;
			FoodPriceQuote& quote = out.values.Add(mi.db_food_no);
			int k = prof.price.history.Find(mi.db_food_no);
			if (k >= 0 && prof.price.history[k].GetCount()) {
				const FoodPriceQuote& prev_quote = prof.price.history[k].Top();
				quote.Set(time, grams, prev_quote);
			}
			else {
				quote.SetPriceless(time, grams);
			}
		}
	}
}

void FoodStorage::AddShopQueue(int day_i) {
	const FoodDay& day = days[day_i];
	const Profile& prof = GetProfile();
	Vector<FoodPrice>& out_vec = GetProfile().shoplog.queue;
	Time time(day.date.year, day.date.month, day.date.day, 12, 0, 0);
	
	if (day.buy_amount.GetCount()) {
		FoodPrice& out = out_vec.Add();
		out.time = time;
		for(int i = 0; i < day.buy_amount.GetCount(); i++) {
			int db_no = day.buy_amount.GetKey(i);
			double grams = day.buy_amount[i];
			FoodPriceQuote& quote = out.values.Add(db_no);
			int k = prof.price.history.Find(db_no);
			if (k >= 0 && prof.price.history[k].GetCount()) {
				const FoodPriceQuote& prev_quote = prof.price.history[k].Top();
				quote.Set(time, grams, prev_quote);
			}
			else {
				quote.SetPriceless(time, grams);
			}
		}
	}
}

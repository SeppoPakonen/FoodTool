#include "FoodTool.h"


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

void FoodStorage::Update(const Vector<DailyPlan>& planned_daily) {
	ASSERT_(days.GetCount(), "Run Init first");
	while (!HasEnoughPreplanned())
		PlanWeek(planned_daily);
}

void FoodStorage::AddFoodQuantity(const FoodQuantityInt& src, FoodQuantity& dst) {
	const Database& db = DB();
	for(int i = 0; i < src.GetCount(); i++) {
		int db_i = src.GetKey(i);
		int count = src[i];
		int local_i = db.local_products.Find(db_i);
		double food_grams = DEFAULT_STEP_GRAMS;
		if (local_i >= 0) {
			const LocalProduct& p = db.local_products[local_i];
			food_grams = p.grams;
		}
		ASSERT(food_grams > 0);
		dst.GetAdd(db_i, 0) += count * food_grams;
	}
}

bool FoodStorage::HasEnoughPreplanned() {
	Date today = GetSysTime();
	
	int today_i = -1;
	for(int i = days.GetCount()-1; i >= 0; i--) {
		FoodDay& day = days[i];
		if (day.date == today) {
			today_i = i;
			break;
		}
	}
	
	if (today_i < 0)
		return false;
	
	for(int i = today_i; i < days.GetCount(); i++) {
		FoodDay& day = days[i];
		if (day.is_shopping && i < days.GetCount()-1)
			return true;
	}
	
	return false;
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

String FoodStorage::GetNextShoppingList() {
	Date today = GetSysTime();
	for(int i = days.GetCount()-2; i >= 0; i--) {
		FoodDay& day = days[i];
		if (day.date <= today)
			break;
		if (day.is_shopping)
			return day.shopping_list;
	}
	
	return "<Error fetching the next shopping list>";
}

bool FoodStorage::NeedShopping() {
	
	
	return false;
}

void FoodStorage::DoShop() {
	
}

void FoodStorage::ConsumeDay() {
	
}

void FoodStorage::PlanWeek(const Vector<DailyPlan>& planned_daily) {
	const Database& db = DB();
	Profile& prof = GetProfile();
	const Configuration& conf = prof.confs.Top();
	int shop_interval = conf.shop_interval;
	
	FoodDay& shopping_day = days.Top();
	ASSERT(shopping_day.is_shopping);
	
	Date d = shopping_day.date;
	d++;
	FoodDay* prev = &shopping_day;
	FoodQuantity total_usage;
	int zero_day_i = 0;
	
	Ingredient target_diff = prev->total_sum - prev->target_sum;
	
	for(int i = 0; i < shop_interval; i++) {
		int day_i = days.GetCount();
		
		if (!i) zero_day_i = days.GetCount();
		const DailyPlan& p = planned_daily[day_i];
		FoodDay& day = days.Add();
		day.date = d;
		day.wake_time = Time(d.year, d.month, d.day, conf.waking_hour, conf.waking_minute, 0);
		day.sleep_time = Time(d.year, d.month, d.day, conf.sleeping_hour, conf.sleeping_minute, 0);
		if (day.sleep_time < day.wake_time) day.sleep_time += 24*60*60;
		d++;
		
		day.is_shopping = i == (shop_interval-1);
		day.used_food_amount <<= prev->used_food_amount;
		day.used_meal_amount <<= prev->used_meal_amount;
		day.generated_meals <<= prev->generated_meals;
		day.total_consumed = prev->total_consumed;
		day.mode = p.mode;
		
		day.target_sum = p.food;
		day.target_sum -= target_diff;
		day.target_sum.grams = p.food.grams;
		day.target_sum.Limit();
		for(int j = 0; j < db.nutr_recom.GetCount(); j++) {
			const NutritionRecommendation& r = db.nutr_recom[j];
			double target;
			if (r.per_kg)
				target = r.value * p.weight;
			else
				target = r.value;
			
			auto& value = day.target_sum.nutr[r.nutr_no];
			if (r.nutr_no == SODIUM)
				value = target;
			if (value < target)
				value = target;
		}
		day.target_sum.nutr[KCAL] = max((float)MINIMUM_DAILY_KCAL, day.target_sum.nutr[KCAL]);
		Ingredient orig_target_sum = day.target_sum;
		
		/*for(int j = 0; j < db.nutr_types.GetCount(); j++) {
			const NutritionType& t = db.nutr_types[j];
			LOG(t.nutr_desc << ": " << orig_target_sum.nutr[j]);
		}*/
		
		MakeMenu(p, day);
		
		double day_target_kcals_sum = day.target_sum.nutr[KCAL];
		double day_kcals_sum = day.total_sum.nutr[KCAL];
		double day_kcals_target_sum = p.food.nutr[KCAL];
		
		target_diff = day.total_sum - orig_target_sum;
		day.total_consumed += day.total_sum;
		
		if (1) {
			VectorMap<int, double> max_diff;
			for(int j = 0; j < prof.planned_nutrients.GetCount(); j++) {
				int k = prof.planned_nutrients[j];
				const NutritionType& t = db.nutr_types[k];
				double tgt = orig_target_sum.nutr[k];
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
				LOG(j << ": " << t.nutr_desc << ": " << value << ": " << day.total_sum.nutr[k] << " - " << orig_target_sum.nutr[k] << " = " << target_diff.nutr[k]);
			}
		}
		
		for(int j = 0; j < day.food_usage.GetCount(); j++) {
			int db_i = day.food_usage.GetKey(j);
			float usage = day.food_usage[j];
			total_usage.GetAdd(db_i, 0) += usage;
			day.used_food_amount.GetAdd(db_i, 0) += usage;
		}
		
		for(int j = 0; j < day.meals.GetCount(); j++)
			day.used_meal_amount.GetAdd(day.meals[j].key, 0)++;
		
		prev = &day;
	}
	
	FoodQuantity existing_food_grams;
	existing_food_grams <<= shopping_day.food_grams;
	for(int j = 0; j < shopping_day.food_usage.GetCount(); j++)
		existing_food_grams.Get(shopping_day.food_usage.GetKey(j)) -= shopping_day.food_usage[j];
	
	FoodQuantityInt& buy_amount = shopping_day.buy_amount;
	buy_amount.Clear();
	for(int i = 0; i < total_usage.GetCount(); i++) {
		int db_i = total_usage.GetKey(i);
		float usage = total_usage[i];
		float already = existing_food_grams.Get(db_i, 0);
		ASSERT(already >= 0.0 && usage >= 0.0);
		float must_buy = usage - already;
		int buy_count = 0;
		float bought_sum = 0;
		
		const FoodDescription& d = db.food_descriptions[db_i];
		
		if (must_buy > 0) {
			int step_grams = DEFAULT_STEP_GRAMS;
			
			int local_i = db.local_products.Find(db_i);
			if (local_i >= 0)
				step_grams = db.local_products[local_i].grams;
			
			do {
				buy_count++;
				bought_sum += step_grams;
			}
			while (bought_sum < must_buy);
			
			buy_amount.Add(db_i, buy_count);
		}
	}
	
	if (buy_amount.GetCount()) {
		shopping_day.shopping_list.Clear();
		for(int i = 0; i < buy_amount.GetCount(); i++) {
			int db_i = buy_amount.GetKey(i);
			int count = buy_amount[i];
			const FoodDescription& d = db.food_descriptions[db_i];
			shopping_day.shopping_list << d.long_desc << " *" << count << "\n";
		}
	}
	else
		shopping_day.shopping_list = "Nothing to buy at this time";
	
	prev = &shopping_day;
	for(int i = 0; i < shop_interval; i++) {
		FoodDay& day = days[zero_day_i + i];
		
		day.food_grams <<= prev->food_grams;
		for(int j = 0; j < prev->food_usage.GetCount(); j++)
			day.food_grams.Get(prev->food_usage.GetKey(j)) -= prev->food_usage[j];
		
		if (i == 0)
			AddFoodQuantity(shopping_day.buy_amount, day.food_grams);
		
		for(int j = 0; j < day.food_usage.GetCount(); j++) {
			int db_i = day.food_usage.GetKey(j);
			ASSERT(day.food_grams.Find(db_i) >= 0);
		}
		
		
		prev = &day;
	}
	ASSERT(days.Top().is_shopping && days.Top().food_grams.GetCount());
}

void FoodStorage::MakeMenu(const DailyPlan& p, FoodDay& d) {
	ASSERT(d.target_sum.nutr[FAT]  >= 0 && d.target_sum.nutr[FAT]  < 2000);
	ASSERT(d.target_sum.nutr[CARB] >= 0 && d.target_sum.nutr[CARB] < 2000);
	ASSERT(d.target_sum.nutr[PROT] >= 0 && d.target_sum.nutr[PROT] < 2000);
	const Database& db = DB();
	const Configuration& conf = GetProfile().confs.Top();
	Profile& prof = GetProfile();
	Time now = GetSysTime();
	
	Index<int> high_fat_foods, high_prot_foods, high_vitamin_foods, other_foods;
	Index<int> fat_nutr, prot_nutr;
	for(int i = 0; i < db.used_foods.GetCount(); i++) {
		int db_i = db.used_foods[i];
		const FoodDescription& f = db.food_descriptions[db_i];
		Ingredient ing;
		ing.Reset();
		for (const auto& n : f.nutr)
			ing.nutr[n.nutr_no] = n.nutr_value;
		
		if (ing.nutr[PROT] >= 80)
			high_prot_foods.Add(i);
		else if (ing.nutr[FAT] >= 80)
			high_fat_foods.Add(i);
		else {
			OnlineAverage1 vit_av;
			for(const NutritionRecommendation& r: db.nutr_recom) {
				double value = ing.nutr[r.nutr_no];
				double target;
				if (r.per_kg)
					target = r.value * p.weight;
				else
					target = r.value;
				ASSERT(target > 0);
				double frac = value / target;
				vit_av.Add(frac);
			}
			if (vit_av.GetMean() > 1.0)
				high_vitamin_foods.Add(i);
			else
				other_foods.Add(i);
		}
	}
	fat_nutr.Add(FAT);
	prot_nutr.Add(PROT);
	
	if (0) {
		for(int i = 0; i < high_fat_foods.GetCount(); i++) {
			const FoodDescription& d = db.food_descriptions[db.used_foods[high_fat_foods[i]]];
			LOG("High fat food " << i << ": " << d.long_desc);
		}
		for(int i = 0; i < high_prot_foods.GetCount(); i++) {
			const FoodDescription& d = db.food_descriptions[db.used_foods[high_prot_foods[i]]];
			LOG("High protein food " << i << ": " << d.long_desc);
		}
		for(int i = 0; i < high_vitamin_foods.GetCount(); i++) {
			const FoodDescription& d = db.food_descriptions[db.used_foods[high_vitamin_foods[i]]];
			LOG("High vitamin food " << i << ": " << d.long_desc);
		}
		for(int i = 0; i < other_foods.GetCount(); i++) {
			const FoodDescription& d = db.food_descriptions[db.used_foods[other_foods[i]]];
			LOG("Other food " << i << ": " << d.long_desc);
		}
	}
	
	
	if (d.mode == MODE_WEIGHTLOSS) {
		Time t = d.wake_time;
		int gen_for_today = 0;
		
		d.total_sum.Reset();
		d.menu.Clear();
		d.preparation.Clear();
		
		int skip_meal = 0;
		int old_meals = 0;
		Time last_old_meal;
		while (t <= d.sleep_time) {
			Meal& m = d.meals.Add();
			m.time = t;
			
			if (d.generated_meals.GetCount()) {
				m.gen_i = d.generated_meals.GetKey(0);
				const MealPreset& mp = prof.presets[m.gen_i];
				ASSERT(mp.serving_grams > 0);
				m.grams = mp.serving_grams;
				int& count = d.generated_meals[0];
				if (!--count)
					d.generated_meals.Remove(0);
				old_meals++;
				last_old_meal = t;
				
				static thread_local Ingredient ing;
				mp.GetNutritions(ing);
				d.total_sum.Add(mp.serving_grams, ing);
			}
			else {
				if (t < now)
					skip_meal++;
				gen_for_today++;
			}
			t += conf.hours_between_meals * 60 * 60;
		}
		
		if (old_meals) {
			if (old_meals == d.meals.GetCount())
				d.menu << t_("Eat previous food all day\n");
			else
				d.menu << Format(t_("Eat previous food until %d:%02d"), (int)last_old_meal.hour, (int)last_old_meal.minute) << "\n";
		}
		
		if (gen_for_today > 0) {
			Panic("TODO");
			
			// Generate until later part of the next day
			int gen_count = gen_for_today + d.meals.GetCount() * (2.0 / 3.0);
			int gen_i = prof.presets.GetCount();
			MealPreset& new_food = prof.presets.Add();
			new_food.name = "Food #" + IntStr(prof.presets.GetCount());
			new_food.type = BREAKFAST;
			new_food.ingredients.SetCount(db.used_foods.GetCount());
			for(int i = 0; i < db.used_foods.GetCount(); i++) {
				MealIngredient& ing = new_food.ingredients[i];
				ing.db_food_no = db.used_foods[i];
				ing.min_grams = 0;
				ing.max_grams = 0;
				const FoodDescription& d = db.food_descriptions[ing.db_food_no];
				if (d.require_soaking)
					ing.pre_day_instructions.GetAdd(1)
						<< "Put into water: " << d.long_desc << "\n";
			}
			new_food.instructions += "Put all ingredients to blender and blend. Serve hot or cold.";
			
			#define SINGLE_OPT 1
			#define GEN_OPT 1
			#define GEN_NORMALIZE 1
			
			#if SINGLE_OPT
			
			#if GEN_OPT
			Optimizer opt;
			opt.MinMax(0, 200);
			opt.SetMaxGenerations(100);
			opt.Init(db.used_foods.GetCount(), 1000);
			#else
			StepOptimizer opt;
			opt.Init(db.used_foods.GetCount(), 3000, 1);
			#endif
			
			while (!opt.IsEnd()) {
				opt.Start();
			
				const Vector<double>& trial = opt.GetTrialSolution();
				for(int i = 0; i < db.used_foods.GetCount(); i++) {
					MealIngredient& ing = new_food.ingredients[i];
					double new_grams = max(0.0, trial[i]);
					ing.max_grams = new_grams;
				}
				
				MealDebugger dbg;
				double energy = new_food.GetOptimizerEnergy(d.target_sum, prof.planned_nutrients, dbg);
				
				//LOG(opt.GetRound() << ": " << energy);
				
				opt.Stop(energy);
			}
			
			const Vector<double>& best = opt.GetBestSolution();
			for(int i = 0; i < db.used_foods.GetCount(); i++) {
				MealIngredient& ing = new_food.ingredients[i];
				double grams = max(0.0, best[i]);
				ing.max_grams = grams;
			}
			
			static thread_local Ingredient ing;
			
			#if GEN_OPT && GEN_NORMALIZE
			new_food.GetNutritions(ing);
			double kcal_mul = d.target_sum.nutr[KCAL] / ing.nutr[KCAL];
			ASSERT(IsFin(kcal_mul));
			if (kcal_mul < 1.0) {
				for (auto& ing : new_food.ingredients) {
					ing.max_grams *= kcal_mul;
				}
			}
			#endif
			
			/*{
				static thread_local Ingredient ing;
				new_food.GetNutritions(ing);
				for(int i = 0; i < new_food.ingredients.GetCount(); i++) {
					int j = i;
					MealIngredient& mi = new_food.ingredients[j];
					const FoodDescription& fd = db.food_descriptions[mi.db_food_no];
					if (fd.long_desc.Find("Rainbow 72g monivitamiini") >= 0) {
						int nutr_i = db.FindNutrition("RIBF");
						double new_food_ribf = ing.nutr[nutr_i];
						
						Ingredient di;
						di.Reset();
						for(const NutritionInfo& ni : fd.nutr)
							di.nutr[ni.nutr_no] = ni.nutr_value / 100.0 * mi.grams;
						double fd_ribf = di.nutr[nutr_i];
						
						double target_ribf = d.target_sum.nutr[nutr_i];
						
						DUMP(new_food_ribf);
						DUMP(fd_ribf);
						DUMP(target_ribf);
					}
				}
			}*/

			#else
			
			int iter_count = 2;
			for (int iter = 0; iter < iter_count; iter++) {
				double mul = (double)(iter + 1) / iter_count;
				Ingredient target_sum = d.target_sum;
				target_sum *= mul;
				
				static thread_local Vector<double> existing_grams;
				existing_grams.SetCount(new_food.ingredients.GetCount());
				
				for (int part = 0; part < 4; part++) {
					Index<int>* idx;
					Index<int>* nutr_idx;
					int step = 1;
					
					for(int i = 0; i < new_food.ingredients.GetCount(); i++)
						existing_grams[i] = new_food.ingredients[i].grams;
					
					// Fat
					if (part == 2) {
						idx = &high_fat_foods;
						nutr_idx = &fat_nutr;
					}
					// Protein
					else if (part == 3) {
						idx = &high_prot_foods;
						nutr_idx = &prot_nutr;
					}
					// Normal food
					else if (part == 0) {
						idx = &other_foods;
						nutr_idx = &prof.planned_nutrients;
						step = 10;
					}
					// Dietary supplements
					else if (part == 1) {
						idx = &high_vitamin_foods;
						nutr_idx = &prof.planned_nutrients;
					}
					
					if (idx->IsEmpty())
						continue;
					
					MealDebugger dbg;
					bool show_av = false;
					if (show_av) {
						for(int i = 0; i < db.nutr_types.GetCount(); i++)
							dbg.nutr_fabs_av.Add(i);
						dbg.do_debug = true;
					}
					
					#if GEN_OPT
					Optimizer opt;
					opt.MinMax(0, 200);
					opt.SetMaxGenerations(100);
					opt.Init(idx->GetCount(), 200);
					#else
					StepOptimizer opt;
					opt.Init(idx->GetCount(), 1000, step);
					#endif
					
					while (!opt.IsEnd()) {
						opt.Start();
						
						const Vector<double>& trial = opt.GetTrialSolution();
						for(int i = 0; i < idx->GetCount(); i++) {
							int j = (*idx)[i];
							MealIngredient& ing = new_food.ingredients[j];
							double new_grams = max(0.0, existing_grams[j] + trial[i]);
							ing.grams = new_grams;
						}
						
						double energy = new_food.GetOptimizerEnergy(target_sum, *nutr_idx, dbg);
						
						//LOG(opt.GetRound() << ": " << energy);
						
						opt.Stop(energy);
					}
					
					if (show_av) {
						LOG("ITER " << iter << " PART " << part);
						SortByValue(dbg.nutr_fabs_av, OnlineAverage1());
						for(int i = 0; i < dbg.nutr_fabs_av.GetCount(); i++) {
							int j = dbg.nutr_fabs_av.GetKey(i);
							const NutritionType& t = db.nutr_types[j];
							double av = dbg.nutr_fabs_av[i].GetMean();
							if (av > 0) {
								LOG(i << ": " << t.nutr_desc << ": " << av);
							}
						}
					}
					
					const Vector<double>& best = opt.GetBestSolution();
					for(int i = 0; i < idx->GetCount(); i++) {
						int j = (*idx)[i];
						MealIngredient& ing = new_food.ingredients[j];
						double grams = max(0.0, existing_grams[j] + best[i]);
						ing.grams = grams;
					}
					
					
					#if GEN_OPT && GEN_NORMALIZE
					static thread_local Ingredient ing;
					new_food.GetNutritions(ing);
					double kcal_mul = d.target_sum.nutr[KCAL] / ing.nutr[KCAL];
					ASSERT(IsFin(kcal_mul));
					if (kcal_mul < 1.0) {
						for (auto& ing : new_food.ingredients) {
							ing.grams *= kcal_mul;
						}
					}
					#endif
				}
			}
			static thread_local Ingredient ing;
			#endif
			new_food.GetNutritions(ing);
			
			
			if (0) {
				double limit = 0.33;
				while (true) {
					double fat_factor = ing.nutr[FAT] / d.target_sum.nutr[FAT];
					double prot_factor = ing.nutr[PROT] / d.target_sum.nutr[PROT];
					
					if (fat_factor >= limit && prot_factor >= limit)
						break;
					
					if (fat_factor < limit)
						new_food.ingredients[high_fat_foods[0]].max_grams += 1.0;
					if (prot_factor < limit)
						new_food.ingredients[high_prot_foods[0]].max_grams += 1.0;
					
					new_food.GetNutritions(ing);
				}
			}
			
			if (1) {
				VectorMap<int, double> ing_grams;
				for(int i = 0; i < new_food.ingredients.GetCount(); i++) {
					const MealIngredient& ing = new_food.ingredients[i];
					if (ing.max_grams > 0)
						ing_grams.Add(i, ing.max_grams);
				}
				SortByValue(ing_grams, StdGreater<double>());
				for(int i = 12; i < ing_grams.GetCount(); i++) {
					int j = ing_grams.GetKey(i);
					new_food.ingredients[j].max_grams = 0;
				}
				
				new_food.GetNutritions(ing);
				double kcal_mul = d.target_sum.nutr[KCAL] / ing.nutr[KCAL];
				ASSERT(IsFin(kcal_mul));
				for (auto& ing : new_food.ingredients) {
					ing.max_grams *= kcal_mul;
				}
				new_food.GetNutritions(ing);
			}
			
			double grams_sum = 0;
			for (auto& ing : new_food.ingredients)
				grams_sum += ing.max_grams;
			
			new_food.serving_grams = grams_sum / d.meals.GetCount();
			
			
			int leftovers = gen_count;
			int gen_meals_today = 0;
			Time first_gen_meal ;
			for(int i = 0; i < d.meals.GetCount(); i++) {
				Meal& m = d.meals[i];
				if (m.gen_i >= 0) continue;
				if (!gen_meals_today) {
					first_gen_meal = m.time;
				}
				m.gen_i = gen_i;
				m.grams = new_food.serving_grams;
				leftovers--;
				gen_meals_today++;
				
				d.total_sum.Add(new_food.serving_grams, ing);
			}
			ASSERT(leftovers >= 0);
			
			double new_food_grams = ing.grams;
			double total_sum_grams = d.total_sum.grams;
			double food_day_kcals = ing.nutr[KCAL];
			double day_kcals_sum = d.total_sum.nutr[KCAL];
			
			if (leftovers > 0)
				d.generated_meals.Add(gen_i, leftovers);
			
			d.menu << Format("Eat new food after %d:%02d", (int)first_gen_meal.hour, (int)first_gen_meal.minute) << "\n";
			
			double mul = (double)(gen_count - skip_meal) / d.meals.GetCount();
			d.preparation << "Eat:\n";
			grams_sum = 0;
			for(int i = 0; i < new_food.ingredients.GetCount(); i++) {
				const MealIngredient& ing = new_food.ingredients[i];
				const FoodDescription& f = db.food_descriptions[ing.db_food_no];
				if (ing.max_grams >= 1.0) {
					float grams = mul * ing.max_grams;
					grams_sum += grams;
					d.preparation << Format(" - (%s) %1n grams (%1n grams total)\n", f.long_desc, grams, grams_sum);
					d.food_usage.GetAdd(ing.db_food_no, 0) += grams;
				}
			}
			
			d.preparation << "Kcals/day: " << ing.nutr[KCAL] << "\n";
			d.preparation << "Fat g/day: " << ing.nutr[FAT] << "\n";
			d.preparation << "Protein g/day: " << ing.nutr[PROT] << "\n";
			d.preparation << "Carbs g/day: " << ing.nutr[CARB] << "\n";
			
			static int nutrilet_i;
			if (!nutrilet_i)
				nutrilet_i = db.FindFood("Nutrilet");
			if (nutrilet_i >= 0) {
				Ingredient nutri_ing;
				nutri_ing.Reset();
				const FoodDescription& fd = db.food_descriptions[nutrilet_i];
				OnlineAverage1 av;
				for(const NutritionInfo& ni : fd.nutr) {
					double a = ing.nutr[ni.nutr_no];
					double b = ni.nutr_value;
					double ratio = min(2.0, a / b);
					if (ni.nutr_no == KCAL || ni.nutr_no == FAT || ni.nutr_no == PROT)
						continue;
					av.Add(ratio);
				}
				d.preparation << Format("Nutrilet factor: %2n", av.GetMean()) << "\n";
			}
		}
	}
	else if (d.mode == MODE_MAINTAIN) {
		Panic("TODO");
		
		#if 0
		d.meals.SetCount(4);
		
		float calorie_sum = 0.0;
		for(int i = 0; i < 4; i++) {
			int bit = 1 << i;
			Meal& m = d.meals[i];
			
			// These are just relative numbers in this point
			switch (i) {
				case 0: m.target_sum.kcals = 200; break;
				case 1: m.target_sum.kcals = 200; break;
				case 2: m.target_sum.kcals = 500; break;
				case 3: m.target_sum.kcals = 200; break;
			}
			calorie_sum += m.target_sum.kcals;
		}
		
		int food_item_count = 0;
		for(int i = 0; i < 4; i++) {
			int bit = 1 << i;
			Meal& m = d.meals[i];
			m.target_sum = d.target_sum;
			
			//float perc = m.target_sum.kcals / calorie_sum;
			//m.target_sum *= perc;
			
			
			// Select randomly
			int mtype_i = -1;
			if (Randomf() < 0.2) {
				int counter = Random(meal_types.GetCount());
				int j = 0;
				while (1) {
					MealPreset& t = meal_types[j];
					if (t.time_of_day & bit) {
						if (!counter) {
							mtype_i = j;
							break;
						}
						counter--;
					}
					j = (j + 1) % meal_types.GetCount();
				}
			}
			else {
				static thread_local Vector<int> allowed_meals;
				allowed_meals.SetCount(0);
				
				for(int j = 0; j < meal_types.GetCount(); j++) {
					MealPreset& t = meal_types[j];
					if (t.time_of_day & bit)
						allowed_meals.Add(j);
				}
				
				if (allowed_meals.GetCount() > 1) {
					MealQuantitySorter sorter(this, d);
					Sort(allowed_meals, sorter);
				}
				mtype_i = allowed_meals[0];
			}
			ASSERT(mtype_i >= 0);
			m.key = meal_types.GetKey(mtype_i);
			
			
			m.food.Clear();
			
			const MealPreset& t = meal_types.Get(m.key);
			for(int j = 0; j < t.foods.GetCount(); j++) {
				String key = t.foods[j];
				const FoodType& ft = food_types.Get(key);
				Ingredient& details = m.food.Add(key);
				details = ft.details;
				details.grams = 100;
			}
			
			if (t.opt_foods.GetCount()) {
				static thread_local VectorMap<String, double> usage;
				usage.Clear();
				for(int j = 0; j < t.opt_foods.GetCount(); j++) {
					String key = t.opt_foods[j];
					double food_usage = d.used_food_amount.Get(key, 0);
					usage.Add(key, food_usage);
				}
				SortByValue(usage, StdLess<double>());
			
				double max_usage = usage[0] * 1.25;
				for(int j = 0; j < t.opt_foods.GetCount(); j++) {
					String key = t.opt_foods[j];
					double food_usage = usage.Get(key);
					if (food_usage > max_usage)
						continue;
					const FoodType& ft = food_types.Get(key);
					Ingredient& details = m.food.Add(key);
					details = ft.details;
					details.grams = 100;
				}
			}
			
			food_item_count += m.food.GetCount();
		}

		Optimizer opt;
		opt.MinMax(0, 200);
		opt.SetMaxGenerations(100);
		opt.Init(food_item_count, 200);
		while (!opt.IsEnd()) {
			opt.Start();
			
			const Vector<double>& trial = opt.GetTrialSolution();
			d.SetMealGrams(trial, food_types);
			
			double invalid_sum = 0.0;
			for(int i = 0; i < d.meals.GetCount(); i++) {
				const Meal& m = d.meals[i];
				double kcal_diff = fabs(m.target_sum.kcals - m.food_sum.kcals);
				if (kcal_diff >= 100) {
					invalid_sum += kcal_diff;
				}
			}
			
			double energy;
			if (invalid_sum > 0.0)
				energy = -100000 * invalid_sum;
			else
				energy = d.GetOptimizerEnergy();
			
			//LOG(opt.GetRound() << ": " << energy);
			
			opt.Stop(energy);
		}
		
		const Vector<double>& best = opt.GetBestSolution();
		d.SetMealGrams(best, food_types, true);
		double energy = d.GetOptimizerEnergy();
		LOG("Final optimization result: " << energy);
		
		d.menu.Clear();
		d.food_usage.Clear();
		d.preparation = "<not implemented yet>";
		for(int i = 0; i < d.meals.GetCount(); i++) {
			const Meal& m = d.meals[i];
			
			switch (i) {
				case 0: d.menu << "Breakfast\n---------\n"; break;
				case 1: d.menu << "Early snack\n-----------\n"; break;
				case 2: d.menu << "Lunch\n-----\n"; break;
				case 3: d.menu << "Late snack\n----------\n"; break;
			}
			
			for(int j = 0; j < m.food.GetCount(); j++) {
				int db_i = m.food.GetKey(j);
				float grams = m.food[j];
				ASSERT(grams >= 0.0);
				if (grams > 0.01) {
					d.food_usage.GetAdd(db_i, 0) += grams;
					
					int local_i = db.local_products.Find(db_i);
					if (local_i >= 0) {
						const LocalProduct& p = db.local_products[local_i];
						if (p.serving_grams > 0) {
							int servings = (int)((grams / (float)p.serving_grams) + 0.5);
							d.menu << p.title << ": " << servings << " pieces\n";
						}
						else {
							d.menu << p.title << ": " << Format("%1n", grams) << "g\n";
						}
					}
					else {
						const FoodDescription& f = db.food_descriptions[db_i];
						d.menu << f.long_desc << ": " << Format("%1n", grams) << "g\n";
					}
				}
			}
			
			d.menu << "\n";
		}
		#endif
	}
	else if (d.mode == MODE_MUSCLEGAIN) {
		Panic("TODO");
	}
	
	
	
}

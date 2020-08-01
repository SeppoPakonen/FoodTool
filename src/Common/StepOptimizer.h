#ifndef _FoodTool_StepOptimizer_h_
#define _FoodTool_StepOptimizer_h_


struct StepOptimizer {
	Vector<double> best_solution, trial_solution;
	Vector<double> values;
	double step = 1.0;
	int value_count = 0;
	int max_iters = 0;
	int iter = 0;
	
	void Init(int value_count, int max_iters, double step=1.0) {
		best_solution.SetCount(value_count);
		trial_solution.SetCount(value_count);
		values.SetCount(2 * value_count);
		for (double& d : best_solution) d = 0;
		for (double& d : trial_solution) d = 0;
		this->value_count = value_count;
		this->max_iters = max_iters * value_count;
		iter = 0;
		this->step = step;
	}
	
	const Vector<double>& GetBestSolution() const {return best_solution;}
	const Vector<double>& GetTrialSolution() const {return trial_solution;}
	
	bool IsEnd() const {return iter >= max_iters;}
	
	void Start() {
		CopyHeapless(best_solution, trial_solution);
		int i = iter % values.GetCount();
		bool inc = i / value_count;
		int value_i = i % value_count;
		if (inc)
			trial_solution[value_i] += step;
		else
			trial_solution[value_i] -= step;
	}
	
	void Stop(double energy) {
		int i = iter++ % values.GetCount();
		values[i] = energy;
		if (iter % values.GetCount() == 0) {
			int max_i = -1;
			double max_value = -DBL_MAX;
			for(int i = 0; i < values.GetCount(); i++) {
				double d = values[i];
				if (d > max_value) {
					max_i = i;
					max_value = d;
				}
			}
			bool inc = max_i / value_count;
			int value_i = max_i % value_count;
			if (inc)
				best_solution[value_i] += step;
			else
				best_solution[value_i] -= step;
		}
	}
};

#endif

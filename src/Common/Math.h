#ifndef _Common_Math_h_
#define _Common_Math_h_

double nround (double n, unsigned int c);

struct OnlineAverage : Moveable<OnlineAverage> {
	double mean;
	int64 count;
	SpinLock lock;
	OnlineAverage() : mean(0), count(0) {}
	
	void Clear() {
		mean = 0.0;
		count = 0;
	}
	
	void Resize(int64 i) {count = i;}
	
	void Add(double a) {
		lock.Enter();
		if (count == 0) {
			mean = a;
		}
		else {
			double delta = a - mean;
			mean += delta / count;
		}
		count++;
		lock.Leave();
	}
	
	double GetMean() const {
		return mean;
	}
	
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % mean % count;}
	}
	
	bool operator()(const OnlineAverage& a, const OnlineAverage& b) const {return a.mean < b.mean;}
};

double NormalPDF(double x, double mean, double dev);

#endif

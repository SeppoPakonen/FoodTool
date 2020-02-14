#ifndef _FoodTool_Util_h_
#define _FoodTool_Util_h_


struct RunningFlag {
	int sleep_time = 100;
	bool running = false;
	Atomic workers_running;
	
	RunningFlag() {workers_running = 0;}
	void Start(int count) {Stop(); running = true; workers_running = count;}
	void Stop() {running = false; while (workers_running > 0) Sleep(sleep_time);}
	void SetNotRunning() {running = false;}
	void IncreaseStopped() {workers_running--;}
	bool IsRunning() const {return running;}
};


struct Version {
	byte write_v = 0, read_v = 0;
	Stream& s;
	
	Version(byte write_v, Stream& s) : write_v(write_v), s(s) {
		if (s.IsStoring())
			s.Put(&write_v, sizeof(write_v));
		else
			s.Put(&read_v, sizeof(read_v));
	}
	
	bool Is(byte cmp_v) {
		if (s.IsStoring())
			return cmp_v <= write_v;
		else
			return cmp_v <= read_v;
	}
};

#define VER(x) Version v(x, s)
#define FOR_VER(x) if (v.Is(x))
	

#ifdef flagWIN32
inline void PlayCameraShutter() {PlaySoundA(ConfigFile("camera-shutter.wav"), NULL, SND_ASYNC|SND_FILENAME);}
inline void PlayAlert() {PlaySoundA(ConfigFile("alert.wav"), NULL, SND_ASYNC|SND_FILENAME);}
#else
inline void PlayCameraShutter() {}
inline void PlayAlert() {}
#endif

Color Rainbow(float progress);



enum {
	DEF_CALCIUM,
	DEF_MAGNESIUM,
	DEF_SODIUM,
	DEF_POTASSIUM,
	DEF_COPPER,
	DEF_ZINC,
	DEF_MANGANESE,
	DEF_CHROMIUM,
	DEF_VANADIUM,
	DEF_MOLYBDENUM,
	DEF_BORON,
	DEF_IODINE,
	DEF_LITHIUM,
	DEF_PHOSPHORUS,
	DEF_SELENIUM,
	DEF_STRONTIUM,
	DEF_SULFUR,
	DEF_COBALT,
	DEF_IRON,
	DEF_GERMANIUM,
	DEF_RUBIDIUM,
	DEF_ZIRCONIUM,
	
	DEF_COUNT
};

String GetNutrientDeficitElement(int i);

struct NutrientDeficitProfile : Moveable<NutrientDeficitProfile> {
	double elements[DEF_COUNT];
	Time added;
	
	void Reset() {
		for(int i = 0; i < DEF_COUNT; i++)
			elements[i] = 50;
	}
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			for(int i = 0; i < DEF_COUNT; i++)
				s % elements[i];
			s % added;
		}
	}
};

struct NutrientDeficitEdit : public Ctrl {
	NutrientDeficitProfile prof;
	Vector<double> x_to_percentile;
	double x25, x160, x450, x500, x550, x840, x975;
	
	Color green, yellow, red;
	
	NutrientDeficitEdit();
	virtual void Paint(Draw& d);
	virtual void LeftDown(Point p, dword keyflags);
};

double KiloJoulesToKiloCalories(double kj);

template <class T>
inline void CopyHeapless(const T& from, T& to) {
	to.SetCount(from.GetCount());
	for(int i = 0; i < to.GetCount(); i++)
		to[i] = from[i];
}


struct OnlineAverage1 : Moveable<OnlineAverage1> {
	double mean;
	int64 count;
	SpinLock lock;
	OnlineAverage1() : mean(0), count(0) {}
	
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
	
	bool operator()(const OnlineAverage1& a, const OnlineAverage1& b) const {return a.mean < b.mean;}
};


template <int I>
struct TopValueSorter {
	static const int size = I;
	
	double value[size];
	int key[size];
	
	
	TopValueSorter() {Reset();}
	void Reset() {
		for(int i = 0; i < size; i++) {
			value[i] = -DBL_MAX;
			key[i] = -1;
		}
	}
	void Add(int key, double value) {
		for(int i = 0; i < size; i++) {
			if (value > this->value[i]) {
				for(int j = size-1; j > i; j--) {
					this->value[j] = this->value[j-1];
					this->key[j]   = this->key[j-1];
				}
				this->value[i] = value;
				this->key[i] = key;
				break;
			}
		}
	}
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {
			for(int i = 0; i < size; i++)
				s % value[i] % key[i];
		}
	}
};




#endif

#ifndef _FoodTool_Util_h_
#define _FoodTool_Util_h_

#define FLOAT COMBINE(flo, at)

struct Gram {
	double value = 0;
	
	Gram() {}
	Gram(double d) : value(d) {}
	Gram(const Gram& s) : value(s.value) {}
	void operator=(const Gram& s) {value = s.value;}
	operator double () const {return value;}
	String ToString() const {return DblStr(value) + " g";}
	static String GetUnitString() {return "g";}
};

struct Kilogram {
	double value = 0;
	
	Kilogram() {}
	Kilogram(double d) : value(d) {}
	Kilogram(const Kilogram& s) : value(s.value) {}
	void operator=(const Kilogram& s) {value = s.value;}
	operator double () const {return value;}
	String ToString() const {return DblStr(value) + " kg";}
	static String GetUnitString() {return "kg";}
};




#define FLOATTRANS COMBINE(Float, Trans)
struct FLOATTRANS : Moveable<FLOATTRANS> {
	double value;
	
	FLOATTRANS() {value = 0;}
	FLOATTRANS(double d) {value = d;}
	void Serialize(Stream& s) {
		if (s.IsLoading()) {
			FLOAT f;
			s % f;
			value = f;
		}
		else {
			s % value;
		}
	}
	
	double operator=(double d) {value = d; return d;}
	double operator+=(double d) {value += d; return d;}
	double operator-=(double d) {value -= d; return d;}
	double operator*=(double d) {value *= d; return d;}
	double operator/=(double d) {value /= d; return d;}
	bool operator==(double d) {return value == d;}
	bool operator!=(double d) {return value != d;}
	operator double() const {return value;}
	operator Value() const {return value;}
	String ToString() const {return DblStr(value);}
};

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
			s.Get(&read_v, sizeof(read_v));
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

Color Rainbow(double progress);
Color CoolBlue(double progress);



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



Time EndOfYesterday(Date d);

double nround (double n, unsigned int c);

inline String GetTimeString(Time t) {
	return Format("%d.%d.%d %d:%02d", (int)t.day, (int)t.month, (int)t.year, (int)t.hour, (int)t.minute);
}


class TableDrawer {
	int cols = 0, rows = 0;
	Vector<String> items;
	Vector<int> max_col_w;
	
public:
	
	void SetSize(int rows, int cols) {this->cols = cols; this->rows = rows; items.SetCount(cols * rows);}
	void Reset() {for(auto& s : items) s.Clear();}
	void Set(int r, int c, String s) {ASSERT(c>=0&&c<cols&&r>=0&&r<rows); items[r * cols + c] = s;}
	String Get(int r, int c) const {ASSERT(c>=0&&c<cols&&r>=0&&r<rows); return items[r * cols + c];}
	void Paint(Rect rect, Draw& d) {
		int pad = 5;
		Size sz(rect.GetSize());
		if (sz.cx < 3*pad || sz.cy < 3*pad)
			return;
		int fnt_h = min(20, sz.cy / rows - 2*pad);
		Font fnt = SansSerif(fnt_h);
		max_col_w.SetCount(cols);
		
		Color color = Black();
		
		for(int t = 0; t < 2; t++) {
			for(auto& d : max_col_w) d = 0;
			
			for(int c = 0; c < cols; c++) {
				for(int r = 0; r < rows; r++) {
					String s = Get(r, c);
					Size sz = GetTextSize(s, fnt);
					sz.cx += 2*pad;
					sz.cy += 2*pad;
					max_col_w[c] = max(sz.cx, max_col_w[c]);
				}
			}
			
			int w = 0;
			for(int c = 0; c < cols; c++)
				w += max_col_w[c];
			if (!w) break;
			
			if (t == 0) {
				double mul = (double)sz.cx / w;
				if (mul < 1.0) {
					fnt_h *= mul;
					fnt = SansSerif(fnt_h);
					continue;
				}
			}
			
			int x = rect.left;
			for(int c = 0; c < cols; c++) {
				int y = rect.top;
				d.DrawLine(x, rect.top, x, rect.bottom, 1, color);
				for(int r = 0; r < rows; r++) {
					String s = Get(r, c);
					d.DrawText(x+pad, y+pad, s, fnt, color);
					y += fnt_h + 2*pad;
				}
				x += max_col_w[c];
			}
			d.DrawLine(x, rect.top, x, rect.bottom, 1, color);
			
			int y = rect.top;
			for(int r = 0; r <= rows; r++) {
				d.DrawLine(rect.left, y, rect.right, y, 1, color);
				y += fnt_h + 2*pad;
			}
			
			break;
		}
	}
};

#endif

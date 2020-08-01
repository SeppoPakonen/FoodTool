#ifndef _Common_Util_h_
#define _Common_Util_h_



// Class for transforming already stored floats to doubles
#define FLOAT COMBINE(flo, at)
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
	void DecreaseRunning() {workers_running--;}
	bool IsRunning() const {return running;}
};


// Version control for persistent binary files
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





template <class T>
inline void CopyHeapless(const T& from, T& to) {
	to.SetCount(from.GetCount());
	for(int i = 0; i < to.GetCount(); i++)
		to[i] = from[i];
}



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





void FillVector(Vector<double>& v);

String UpperBegin(String s);

String GetTimeDurationString(Time begin, Time end);

#endif

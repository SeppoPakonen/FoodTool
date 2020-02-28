#include "Common.h"

#ifdef flagMAIN

struct Rel : Moveable<Rel> {
	double mul, add;
	
	void Set(double m, double a) {mul = m; add = a;}
};

struct Unit : Moveable<Unit> {
	String name, symbol, quantity;
	bool no_prefix;
	String allowed_prefix;
	
	void Set(String n, String s, String q, bool no_prefix=false, String allowed_prefix="") {name = n; symbol = s; quantity = q; this->no_prefix = no_prefix; this->allowed_prefix = allowed_prefix;}
};

struct UnitConversion : Moveable<UnitConversion> {
	String from, to;
	Rel rel;
	
	void Set(String f, String t, double m, double a) {from = f; to = t; rel.Set(m, a);}
};

struct Prefix : Moveable<Prefix> {
	String name, sym;
	
	void Set(String n, String s) {name = n; sym = s;}
};

Vector<String> used_prefix;
VectorMap<String, Rel> rel;
VectorMap<double, Prefix> prefix;
Vector<Unit> unit;
Vector<UnitConversion> conv;


String UpperBegin(String s) {
	String o;
	Vector<String> parts = Split(s, " ");
	for(int i = 0; i < parts.GetCount(); i++) {
		String p = parts[i];
		o += ToUpper(p.Left(1)) + ToLower(p.Mid(1));
	}
	return o;
}

void Generic(int prefix_i, Stream& s, const Unit& u) {
	String pn = used_prefix[prefix_i];
	Prefix* pp = NULL;
	for(int i = 0; i < prefix.GetCount(); i++) {
		if (prefix[i].name == pn) {
			pp = &prefix[i];
			break;
		}
	}
	const Prefix& p = *pp;
	String cls = UpperBegin(p.name) + UpperBegin(u.name);
	String type = "double";
	
	s << "\t" << type << " value = 0;\n";
	s << "\t\n";
	s << "\t" << cls << "() {}\n";
	s << "\t" << cls << "(" << type << " v) : value(v) {}\n";
	s << "\t" << cls << "(const " << cls << "& s) : value(s.value) {}\n";
	s << "\tvoid Serialize(Stream& s) {s \% value;}\n";
	s << "\tvoid operator=(const " << cls << "& f) {value = f.value;}\n";
	s << "\tvoid operator+=(const " << cls << "& f) {value += f.value;}\n";
	s << "\tvoid operator-=(const " << cls << "& f) {value -= f.value;}\n";
	s << "\tvoid operator*=(const " << cls << "& f) {value *= f.value;}\n";
	s << "\tvoid operator/=(const " << cls << "& f) {value /= f.value;}\n";
	s << "\tbool operator==(const " << cls << "& f) {return value == f.value;}\n";
	s << "\tbool operator!=(const " << cls << "& f) {return value != f.value;}\n";
	s << "\tbool operator<=(const " << cls << "& f) {return value <= f.value;}\n";
	s << "\tbool operator>=(const " << cls << "& f) {return value >= f.value;}\n";
	s << "\tbool operator<(const " << cls << "& f) {return value < f.value;}\n";
	s << "\tbool operator>(const " << cls << "& f) {return value > f.value;}\n";
	s << "\tbool operator!() {return value == 0;}\n";
	s << "\toperator " << type << " () const {return value;}\n";
	s << "\tString ToString() const {return AsString(value) + \"" << p.sym << "\";}\n";
	s << "\tstatic String GetUnitString() {return \"" << p.sym << "\";}\n";
}

CONSOLE_APP_MAIN {
	
	
	//prefix.Add(1000000).Set("mega", "M");
	prefix.Add(1000).Set("kilo", "k");
	//prefix.Add(100).Set("hecto");
	//prefix.Add(10).Set("deca");
	prefix.Add(1).Set("", "");
	prefix.Add(0.1).Set("deci", "d");
	prefix.Add(0.01).Set("centi", "c");
	prefix.Add(0.001).Set("milli", "m");
	prefix.Add(0.000001).Set("micro", "μ");
	prefix.Add(0.000000001).Set("nano", "n");
	
	// !!! Don't insert values, only add them !!!
	// The position must be same or persistency breaks...
	unit.Add().Set("second", "s", "time", true);
	unit.Add().Set("minute", "min", "time", true);
	unit.Add().Set("hour", "h", "time", true);
	unit.Add().Set("day", "day", "time", true);
	unit.Add().Set("week", "week", "time", true);
	unit.Add().Set("metre", "m", "length");
	//unit.Add().Set("inch", "in", "length");
	//unit.Add().Set("foot", "ft", "length");
	//unit.Add().Set("yard", "yd", "length");
	//unit.Add().Set("mile", "mi", "length");
	//unit.Add().Set("nautical mile", "nmi", "length");
	unit.Add().Set("litre", "l", "volume", true);
	//unit.Add().Set("pint", "pt", "volume");
	//unit.Add().Set("gallon", "gal", "volume");
	unit.Add().Set("gram", "M", "mass");
	//unit.Add().Set("ounce", "oz", "mass");
	//unit.Add().Set("pound", "lb", "mass");
	//unit.Add().Set("stone", "st", "mass");
	//unit.Add().Set("kelvin", "K", "temperature");
	unit.Add().Set("degree celsius", "°C", "temperature", true);
	//unit.Add().Set("joule", "J", "energy");
	unit.Add().Set("calorie", "cal", "energy", true, "kilo");
	// Add here
	
	//unit.Add().Set("ampere", "A", "electric current");
	//unit.Add().Set("mole", "", "amount of substance");
	//unit.Add().Set("candela", "cd", "luminous intensity");
	
	conv.Add().Set("inch", "metre", 0.0000254, 0);
	conv.Add().Set("foot", "metre", 0.3048, 0);
	conv.Add().Set("yard", "metre", 0.9144, 0);
	conv.Add().Set("mile", "metre", 1609.344, 0);
	conv.Add().Set("nautical mile", "metre", 1852, 0);
	conv.Add().Set("pint", "litre", 0.56826125, 0);
	conv.Add().Set("gallon", "litre", 4.54609, 0);
	conv.Add().Set("ounce", "gram", 28.349523125, 0);
	conv.Add().Set("pound", "gram", 453.59237, 0);
	conv.Add().Set("stone", "gram", 6350.29318, 0);
	//conv.Add().Set("kelvin", "degree celsius", 1, -273.15);
	conv.Add().Set("joule", "calorie", 0.2390 , 0);
	
	FileOut cout(GetDataFile("UnitsGenerated.cpp"));
	FileOut hout(GetDataFile("UnitsGenerated.h"));
	hout	<< "#ifndef _Common_UnitsGenerated_h_\n"
			<< "#define _Common_Units_h_\n\n\n";
	cout	<< "#include \"Common.h\"\n\n\n";
	
	
	// Forward declaration
	for(int i = 0; i < unit.GetCount(); i++) {
		const Unit& u = unit[i];
		used_prefix.Clear();
		if (u.no_prefix) {
			used_prefix = Split(u.allowed_prefix, ",");
			used_prefix.Insert(0, "");
		}
		else {
			for(int j = 0; j < prefix.GetCount(); j++)
				used_prefix.Add(prefix[j].name);
		}
		for(int j = 0; j < used_prefix.GetCount(); j++) {
			String cls = UpperBegin(used_prefix[j]) + UpperBegin(u.name);
			hout << "struct " << cls << ";\n";
		}
	}
	hout << "\n\n";
	
	
	// Headers
	for(int i = 0; i < unit.GetCount(); i++) {
		const Unit& u = unit[i];
		
		rel.Clear();
		rel.Add(u.name).Set(1, 0);
		while (1) {
			bool added = false;
			for(int j = 0; j < rel.GetCount(); j++) {
				String to = rel.GetKey(j);
				Rel r = rel[j];
				for(const UnitConversion& c : conv) {
					if (c.to == to) {
						if (rel.Find(c.from) == -1) {
							ASSERT(r.add == 0 && c.rel.add == 0);
							rel.Add(c.from).Set(c.rel.mul * r.mul, 0);
							added = true;
						}
					}
					else if (c.from == to) {
						if (rel.Find(c.to) == -1) {
							ASSERT(r.add == 0 && c.rel.add == 0);
							rel.Add(c.to).Set(1.0 / c.rel.mul * r.mul, 0);
							added = true;
						}
					}
				}
			}
			if (!added) break;
		}
		rel.RemoveKey(u.name);
		
		used_prefix.Clear();
		if (u.no_prefix) {
			used_prefix = Split(u.allowed_prefix, ",");
			used_prefix.Insert(0, "");
		}
		else {
			for(int j = 0; j < prefix.GetCount(); j++)
				used_prefix.Add(prefix[j].name);
		}
		
		for(int j = 0; j < used_prefix.GetCount(); j++) {
			String cls = UpperBegin(used_prefix[j]) + UpperBegin(u.name);
			hout << "struct " << cls << " : Moveable<" << cls << "> {\n";
			Generic(j, hout, u);
			for(int k = 0; k < rel.GetCount(); k++)
				Relation(j, hout, u, rel.GetKey(k), rel[k]);
			hout << "};\n\n";
		}
	}
	
	/*template <class N, class D> struct UnitDiv {
		double value = 0;
		
		UnitDiv() {}
		UnitDiv(double d) : value(d) {}
		UnitDiv(const UnitDiv& s) : value(s.value) {}
		void operator=(const UnitDiv& s) {value = s.value;}
		operator double () const {return value;}
		String ToString() const {return DblStr(value) + " (" + N::GetUnitString() + "/" + D::GetUnitString() + ")";}
		static String GetUnitString() {return "(" + N::GetUnitString() + "/" + D::GetUnitString() + ")";}
	};
	
	typedef UnitDiv<UnitDiv<MilliLitre, Second>, Gram> BloodFlow;
	typedef UnitDiv<Gram, MilliLitre> Gram_MilliLitre;
	
	
	
	struct UnitValue : Moveable<UnitValue> {
		
		
	};*/

	
	hout	<< "#endif\n";
	hout.Close();
	cout.Close();
}


#endif


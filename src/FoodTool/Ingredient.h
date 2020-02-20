#ifndef _FoodTool_Ingredient_h_
#define _FoodTool_Ingredient_h_



template <class T>
struct IngredientT : Moveable<IngredientT<T>> {
	T grams = 0;
	Vector<T> nutr;
	
	IngredientT() {Reset();}
	IngredientT(const IngredientT& src) {*this = src;}
	IngredientT(T grams, const FoodDescription& d) {Set(grams, d);}
	void operator=(const IngredientT& src) {
		grams = src.grams;
		CopyHeapless(src.nutr, nutr);
	}
	void Set(T grams, const FoodDescription& d) {
		Reset();
		for(const auto& n : d.nutr)
			nutr[n.nutr_no] = n.nutr_value * 0.01 * grams;
	}
	void Add(T grams, const FoodDescription& d) {
		this->grams += grams;
		for(const auto& n : d.nutr)
			nutr[n.nutr_no] += n.nutr_value * 0.01 * grams;
	}
	void Add(T grams, const IngredientT<T>& d) {
		double mul = grams / d.grams;
		ASSERT(IsFin(mul));
		this->grams += grams;
		ASSERT(nutr.GetCount() && d.nutr.GetCount() == nutr.GetCount());
		for(int i = 0; i < nutr.GetCount(); i++)
			nutr[i] += d.nutr[i] * mul;
	}
	void Reset() {
		grams = 0;
		nutr.SetCount(DB().nutr_types.GetCount());
		for(auto& a : nutr) a = 0;
		ASSERT(nutr.GetCount());
	}
	void Serialize(Stream& s) {
		VER(0);
		FOR_VER(0) {s % grams % nutr;}
	}
	void Limit() {
		grams = max(grams, (T)0);
		for(auto& f : nutr)
			f = max(f, (T)0.0);
	}
	template <class K>
	void operator +=(const K& d) {
		grams += d.grams;
		ASSERT(d.nutr.GetCount() == nutr.GetCount());
		for(int i = 0; i < nutr.GetCount(); i++)
			nutr[i] += d.nutr[i];
	}
	template <class K>
	void operator -=(const K& d) {
		grams -= d.grams;
		ASSERT(d.nutr.GetCount() == nutr.GetCount());
		for(int i = 0; i < nutr.GetCount(); i++)
			nutr[i] -= d.nutr[i];
	}
	void operator*=(double mul) {
		grams *= mul;
		ASSERT(nutr.GetCount());
		for(int i = 0; i < nutr.GetCount(); i++)
			nutr[i] *= mul;
	}
	void ChangeGrams(double new_grams) {
		ASSERT(grams > 0.0 && IsFin(new_grams));
		double mul = new_grams / grams;
		ASSERT(nutr.GetCount());
		for(auto& f : nutr) {
			f *= mul;
			ASSERT(IsFin(f));
		}
		grams = new_grams;
	}
	
	
};

template <class T>
inline IngredientT<T> operator-(const IngredientT<T>& a, const IngredientT<T>& b) {
	IngredientT<T> out;
	out.grams = a.grams - b.grams;
	ASSERT(out.nutr.GetCount());
	ASSERT(out.nutr.GetCount() == a.nutr.GetCount());
	ASSERT(out.nutr.GetCount() == b.nutr.GetCount());
	for(int i = 0; i < out.nutr.GetCount(); i++)
		out.nutr[i] = a.nutr[i] - b.nutr[i];
	return out;
}

typedef IngredientT<float> Ingredient;
typedef IngredientT<double> IngredientDouble;


#endif

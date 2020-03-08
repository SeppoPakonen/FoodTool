#ifndef _FoodTool_MultipurposeGraph_h_
#define _FoodTool_MultipurposeGraph_h_


struct MultipurposeGraph : public Ctrl {
	enum {
		FOOD_BEGIN = 4,
		MEASURES_BEGIN = 14,
		GRAPH_COUNT = 34,
	};
	
	struct Line : Moveable<Line> {
		String title;
		Color color;
		int width;
	};
	struct Src : Moveable<Src> {
		Vector<Line> lines;
		int vert_line = 0;
		
		Src& Add(String t, int w, Color c) {Line& l = lines.Add(); l.title = t; l.width = w; l.color = c; return *this;}
		Src& Vert(int y) {vert_line = y; return *this;}
	};
	
private:
	Vector<Vector<double>> tmp;
	Vector<Point> polyline;
	Vector<Src> src;
	
public:
	int new_src = 0;
	
	Src& Add() {return src.Add();}
	int GetLineCount(int src);
	Color GetLineColor(int src, int l);
	int GetLineWidth(int src, int l);
	String GetLineTitle(int src, int l);
	int GetCount(int src);
	int GetVertLine(int src);
	int GetHorzLine(int src);
	const Vector<double>& GetValue(int src, int l);
	void Paint(Draw& d);
};

#endif

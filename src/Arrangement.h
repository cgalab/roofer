#ifndef ARRANGEMENT_H_
#define ARRANGEMENT_H_

#include "CGALTypes.h"
#include "SweepLine.h"

using namespace std;

enum class ChainPointType : int {CLOSED = 0, OPEN};

struct ChainPoint : public Point {
//	ChainPoint(const ChainPoint& p):
//		Point(Point(p)),type(p.type),base(p.base),bisectorA(p.bisectorA),bisectorB(p.bisectorB) {}

	ChainPoint(Point p, ChainPointType t, EdgeIterator e, Ray a, Ray b):
		Point(p),type(t),base(e),bisectorA(a),bisectorB(b) {}

	ChainPointType type;
	EdgeIterator   base;

	Ray bisectorA, bisectorB;

	inline void close() {type=ChainPointType::CLOSED;}

	friend bool operator>  (const ChainPoint& a, const Point& b);
	friend bool operator>  (const ChainPoint& a, const ChainPoint& b);
	friend bool operator<  (const ChainPoint& a, const Point& b);
	friend bool operator<  (const ChainPoint& a, const ChainPoint& b);
	friend bool operator== (const ChainPoint& a, const ChainPoint& b);
};


using Chain = list<ChainPoint>; //,less<ChainPoint> >;

class Arrangement {
public:
	Arrangement();

	void addCell(SweepItem& item);

	void addBaseCell(ArrangementLine& line);
	void checkLowerChainEnds();

private:
	Chain 	upperChain;
	Chain	lowerChain;
};

#endif /* ARRANGEMENT_H_ */

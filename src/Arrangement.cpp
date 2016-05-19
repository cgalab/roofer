#include "Arrangement.h"

bool operator==(const ChainPoint& a, const ChainPoint& b) {
		return  Point(a) == Point(b) &&
				a.base == b.base;
}

bool operator> (const ChainPoint& a, const Point& b) {
	auto aOnBase = a.base->supporting_line().projection(a);
	auto bOnBase = a.base->supporting_line().projection(b);
    return a.base->direction() == Vector(aOnBase - bOnBase).direction();
}
bool operator> (const ChainPoint& a, const ChainPoint& b) {
	assert(a.base == b.base);
	auto aOnBase = a.base->supporting_line().projection(a);
	auto bOnBase = b.base->supporting_line().projection(b);
    return a.base->direction() == Vector(aOnBase - bOnBase).direction();
}

bool operator< (const ChainPoint& a, const Point& b) {
	auto aOnBase = a.base->supporting_line().projection(a);
	auto bOnBase = a.base->supporting_line().projection(b);
    return a.base->direction() == Vector(bOnBase - aOnBase).direction();
}
bool operator< (const ChainPoint& a, const ChainPoint& b) {
	assert(a.base == b.base);
	auto aOnBase = a.base->supporting_line().projection(a);
	auto bOnBase = b.base->supporting_line().projection(b);
    return a.base->direction() == Vector(bOnBase - aOnBase).direction();
}

Arrangement::Arrangement() {}

void Arrangement::addCell(SweepItem& item) {

}

void Arrangement::addBaseCell(ArrangementLine& line) {
	ChainPoint p(line.start,ChainPointType::OPEN,line.base,line.bisector,line.bisector);

	Point edgeStart(line.base->vertex(0));
	Point edgeEnd(line.base->vertex(1));

	if(!(edgeStart > p || edgeEnd < p)) {
		lowerChain.push_back(p);
	}
}

void Arrangement::checkLowerChainEnds() {
	if(!lowerChain.empty()) {
		auto& p = lowerChain.front();
		Point q(p + p.bisectorA.to_vector());

		if(p < q) {p.close();}

		auto& r = lowerChain.back();
		Point s(r + r.bisectorA.to_vector());

		if(r > s) {r.close();}
	}

	for(auto i : lowerChain) {
		switch(i.type) {
		case ChainPointType::OPEN : cout << "O "; break;
		case ChainPointType::CLOSED: cout << "1 "; break;
		}
	}
	cout << endl;
}

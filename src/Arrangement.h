#ifndef ARRANGEMENT_H_
#define ARRANGEMENT_H_

#include "CGALTypes.h"
#include "SweepLine.h"

class Arrangement {
public:
	Arrangement();

	void addCell(SweepItem& item);

private:
	// per basline map or per plane arrangement... of cells? TODO!
};

#endif /* ARRANGEMENT_H_ */

/*
 * Skeleton.h
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include "Data.h"


class Skeleton {
public:
	Skeleton(std::list<std::string>& args);
	virtual ~Skeleton();

	bool guiEnabled() {return data.config.gui;}

	void handleNextEvent(SweepEvent& event);

	Data data;

private:
	void createSkeleton();

	void createLineArrangements();
	void addAllBaseCells();
	void startPlaneSweep();

};

#endif /* SKELETON_H_ */

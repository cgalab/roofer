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

	void handleNextEvent(Event& e);

	Data data;

private:
	void createSkeleton();

	void createInitialWavefront();
	void createBisectorRays();

	void computeEdgeEvents();
	void computeSplitEvents();
	void computeCreateEvents();

	void startWavefrontPropagation();

	Event  computeEdgeEvent(WavefrontIterator a, WavefrontIterator b);
	Vertex computeVelocity(Point& v_a, Point& v_b, Point& v_c);

	void handleEdgeEvent(Event& e);
	void handleSplitEvent(Event& e);
	void handleDivideEvent(Event& e);
	void handleCreateEvent(Event& e);
};

#endif /* SKELETON_H_ */

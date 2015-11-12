/*
 * Skeleton.h
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include "Data.h"

#ifdef CGAL_LCC_USE_VIEWER
#include "GUI.h"
#endif



class Skeleton {
public:
	Skeleton(std::list<std::string>& args);
	virtual ~Skeleton();

private:
	Data data;
#ifdef CGAL_LCC_USE_VIEWER
	GUI  gui;
#endif

	void createSkeleton();

	void createInitialWavefront();
	void createBisectorRays();

	void computeEdgeEvents();
	void computeSplitEvents();
	void computeCreateEvents();

	void startWavefrontPropagation();
};

#endif /* SKELETON_H_ */

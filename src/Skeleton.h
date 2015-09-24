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

namespace Skel {


class Skeleton {
public:

	Skeleton(std::list<std::string>& args);

	virtual ~Skeleton();

private:
	Skel::Data data;
#ifdef CGAL_LCC_USE_VIEWER
	GUI        gui;
#endif

	void createSkeleton();

};

} /* end namespace */
#endif /* SKELETON_H_ */

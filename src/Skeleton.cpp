/*
 * Skeleton.cpp
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#include <iostream>

#include "Data.h"
#include "Skeleton.h"

namespace Skel {

Skeleton::Skeleton(std::list<std::string>& args) {

	/* evaluate arguments and load data */
	data.evaluateArguments(args);

	createSkeleton();

	/* display if needed */
#ifdef CGAL_LCC_USE_VIEWER
	gui.show(data.iss);
#endif // CGAL_LCC_USE_VIEWER


}

Skeleton::~Skeleton() {}

void Skeleton::createSkeleton() {
//	data.iss = CGAL::create_interior_straight_skeleton_2(data.polygon.vertices_begin(),
//			data.polygon.vertices_end());



}

} /* namespace SS3D */

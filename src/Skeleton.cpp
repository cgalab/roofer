/*
 * Skeleton.cpp
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#include <iostream>

#include "Data.h"
#include "Skeleton.h"
#include "GUI.h"


Skeleton::Skeleton(std::list<std::string>& args) {
	/* evaluate arguments and load data */
	if(!data.evaluateArguments(args)) return;


	createSkeleton();


	/* display if needed */
#ifdef CGAL_LCC_USE_VIEWER
	if(data.config.gui) {
		gui.make_facet(data.polygon);
		gui.show(data.iss);
	}
#endif // CGAL_LCC_USE_VIEWER

}

Skeleton::~Skeleton() {}

void Skeleton::createSkeleton() {

	//	data.iss = CGAL::create_interior_straight_skeleton_2(data.polygon.vertices_begin(),
	//			data.polygon.vertices_end());

	/***
	 * create initial wavefront
	 ***/
	std::cout << "gen. wavefront";
	for(auto it=data.polygon.vertices_begin(); it != data.polygon.vertices_end(); ++it) {
		data.wavefront.push_back(Vertex(*it));
	}
	std::cout << "...done" << std::endl;



	std::cout << "calc direction";
	auto e_it = data.wavefront.edges_end();
	/* loop over vertices of WF and go along with the edges as well (hopefully) */
	for(auto v_it=data.wavefront.vertices_begin(); v_it != data.wavefront.vertices_end(); ++v_it) {
		K::Line_2 ei(*e_it);
		K::Line_2 ej(*(e_it+1));

		v_it->bisector = CGAL::bisector(ei,ej);

		++e_it;
	}
	std::cout << "...done" << std::endl;


	std::cout << "calc velocity";
	for(auto it=data.wavefront.vertices_begin(); it != data.wavefront.vertices_end(); ++it) {
//		auto e1 = it
	}
	std::cout << "...done" << std::endl;
	/***
	 * calculate all edge-events an queue them
	 ***/
	std::cout << "calc edge events";
	for(auto it=data.wavefront.edges_begin(); it != data.wavefront.edges_end(); ++it) {
//		it->point()
	}
	std::cout << "...done" << std::endl;

	/***
	 * calculate all split-events and queue them
	 ***/

	/***
	 * calculate all initial create-events and queue them
	 ***/

	/***
	 * start the propagation process
	 ***/




	/***
	 * end the propagation process
	 ***/

}


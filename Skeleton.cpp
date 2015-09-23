/*
 * Skeleton.cpp
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#include <iostream>

#include "Skeleton.h"

namespace SS3D {

Skeleton::Skeleton(std::list<std::string>& args) {

	/* evaluate arguments and load data */
	data.evaluateArguments(args);

	/* vertex direction and speed calculation */
	DirectionAndSpeedCalculation();

	/* 3D Triangulation */
	Triangulate3D();

	/* fill priority queue */
	/* event handling */

	/* display if needed */
	data.convertDataForGUI();
	gui.show(data.lcc);

}

Skeleton::~Skeleton() {}

void Skeleton::DirectionAndSpeedCalculation() {
	CGALPolyhedron poly = data.getPolyhedron();
	CGAL::Vector_3<K> vector(0,0,0);

//	for(auto vertex_it = poly.vertices_begin(); vertex_it != poly.vertices_end(); ++vertex_it) {
//
//		CGALPolyhedron::Halfedge_around_vertex_circulator facet_it = vertex_it->vertex_begin();
//
//		for(int i = 0; i < vertex_it->vertex_degree(); ++i) {
//			facet_it->f
//
//			facet_it->next();
//		}
//
//		facet_it->
//
//		auto facet_it = vertex_it->Halfedge_around_vertex_circulator;
//		facet_it
//	}
}

void Skeleton::Triangulate3D() {
//	/* CGAL Triangulation is not considering our constraings,
//	 * we switched to 'Tetgen' which can do a 3D CDT, where
//	 * T stands for tetrahedrisation 						*/
//	std::list<Point> L;
//	data.fillPointList();
//	L = data.getPointList();
//	std::cout << "Vertex count: " << L.size() << std::endl;
//	Triangulation T(L.begin(), L.end());
//	Triangulation::size_type n = T.number_of_vertices();

	tetrahedralize(&data.tet_beh, &data.tet_in, &data.tet_out, NULL, NULL);
}

} /* namespace SS3D */

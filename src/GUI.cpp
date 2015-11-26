/*
 * GUI.cpp
 *
 *  Created on: May 28, 2015
 *      Author: gue
 */

#include "GUI.h"


#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>


GUI::GUI() {}

GUI::~GUI() {}

void GUI::show(const SsPtr& ss) {
	display_lcc(lcc);
}

Dart_handle GUI::make_facet(const Polygon& poly) {
	Dart_handle d =
			CGAL::make_combinatorial_polygon<LCC_3>(lcc,(unsigned int)poly.size());

	for(auto v = poly.vertices_begin(); v != poly.vertices_end(); ++v) {
		//for (unsigned int i=0; i<poly.size(); ++i) {

		Point3D p(v->x().doubleValue(),v->y().doubleValue(),0);

		lcc.set_vertex_attribute_of_dart(d, lcc.create_vertex_attribute(p));
		d = d->beta(1);
	}
	return d;
}

void GUI::addSegment(Point& a, Point& b) {
	Point3D p(a.x().doubleValue(),a.y().doubleValue(),0);
	Point3D q(b.x().doubleValue(),b.y().doubleValue(),0);

	lcc.make_segment(p,q);
}

void GUI::addSS(const SsPtr& ss) {
//	for ( Halfedge_const_iterator i = ss->halfedges_begin(); i != ss->halfedges_end(); ++i )
//	{
//		Point3D p(i->opposite()->vertex()->point().x(),i->opposite()->vertex()->point().y(),0);
//		Point3D q(i->vertex()->point().x(),i->vertex()->point().y(),0);
//
//		lcc.make_segment(p,q);
//	}
}


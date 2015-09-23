/*
 * Skeleton.h
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include "Data.h"

#include "tetgen/tetgen.h"

#ifdef CGAL_LCC_USE_VIEWER
#include "GUI.h"
#endif

namespace SS3D {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

template < class GT, class Vb = CGAL::Triangulation_vertex_base_3<GT> >
class ss_vertex_base
		: public Vb
		  {
		  public:
	typedef typename Vb::Vertex_handle  Vertex_handle;
	typedef typename Vb::Cell_handle    Cell_handle;
	typedef typename Vb::Point          Point;
	template < class TDS2 >
	struct Rebind_TDS {
		typedef typename Vb::template Rebind_TDS<TDS2>::Other  Vb2;
		typedef ss_vertex_base<GT, Vb2>                        Other;
	};
	ss_vertex_base():speed(1.0) {}
	ss_vertex_base(const Point& p)
	: Vb(p),speed(1.0) {}
	ss_vertex_base(const Point& p, Cell_handle c)
	: Vb(p, c),speed(1.0) {}

		  CGAL::Vector_3<K> direction;
		  double speed;
		  };

/*  TRIANLGE 3D */
typedef CGAL::Triangulation_data_structure_3<ss_vertex_base<K> >    Tds;
typedef CGAL::Triangulation_3<K,Tds>      Triangulation;

typedef Triangulation::Cell_handle    Cell_handle;
typedef Triangulation::Vertex_handle  Vertex_handle;
typedef Triangulation::Locate_type    Locate_type;
typedef Triangulation::Point          Point;
/* END TRIANLGE 3D */

class Skeleton {
public:

	Skeleton(std::list<std::string>& args);

	virtual ~Skeleton();

private:
	SS3D::Data data;
#ifdef CGAL_LCC_USE_VIEWER
	GUI        gui;
#endif

	void DirectionAndSpeedCalculation();
	void Triangulate3D();
};

} /* namespace SS3D */
#endif /* SKELETON_H_ */

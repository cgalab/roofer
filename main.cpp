#include<boost/shared_ptr.hpp>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/create_straight_skeleton_2.h>

#include <CGAL/Combinatorial_map_constructors.h>

#include <iostream>
#include <algorithm>
#include <vector>

#include <exception>

#ifdef CGAL_LCC_USE_QT
#include "viewer_qt.h"
#else
#ifdef CGAL_LCC_USE_VTK
#include "viewer_vtk.h"
#endif
#endif

//#include "Skeleton.h"
typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::Point_2                   Point ;
typedef CGAL::Polygon_2<K>           Polygon_2 ;
typedef CGAL::Straight_skeleton_2<K> Ss ;
  
typedef Ss::Vertex_const_handle     Vertex_const_handle ;
typedef Ss::Halfedge_const_handle   Halfedge_const_handle ;
typedef Ss::Halfedge_const_iterator Halfedge_const_iterator ;

typedef boost::shared_ptr<Ss> SsPtr ;


typedef CGAL::Linear_cell_complex<3> LCC_3;
typedef LCC_3::Dart_handle           Dart_handle;
typedef LCC_3::FT                    FT;
typedef LCC_3::Traits 				    Traits;
typedef LCC_3::Point                 Point3D;

Dart_handle make_facet(LCC_3& lcc, const Polygon_2& poly) { 
	Dart_handle d =
			CGAL::make_combinatorial_polygon<LCC_3>(lcc,(unsigned int)poly.size());
	for (unsigned int i=0; i<poly.size(); ++i)
	{
      Point3D p(poly.vertex(i).x(),poly.vertex(i).y(),0); 

		lcc.set_vertex_attribute_of_dart(d, lcc.create_vertex_attribute(p));
		d=d->beta(1);
	}
	return d;
}

void addSS(LCC_3& lcc, const SsPtr& ss) {
for ( Halfedge_const_iterator i = ss->halfedges_begin(); i != ss->halfedges_end(); ++i )
  {
      Point3D p(i->opposite()->vertex()->point().x(),i->opposite()->vertex()->point().y(),0); 
      Point3D q(i->vertex()->point().x(),i->vertex()->point().y(),0); 
    
      lcc.make_segment(p,q);
  }
}


int main(int argc, char** argv) {
//	std::list<std::string> args;
//	for(auto i = 1; i < argc; ++i) { args.push_back(std::string(argv[i])); }
//
//	try {
//		SS3D::Skeleton *skeleton = new SS3D::Skeleton(args);
//		delete skeleton;
//	} catch(std::exception& e) {
//		std::cout << "Exception: " << e.what() << std::endl;
//	}

//	SS3D::Data *data = new SS3D::Data(std::string(argv[1]));
//
//	std::cout << "Testing... ";
//	data->testing();
//
//	std::cout << "done" << std::endl << "Converting... ";

#ifdef CGAL_LCC_USE_VIEWER
   LCC_3 lcc;
#endif

   Polygon_2 poly ;
  
  poly.push_back( Point(0,0) ) ;
  poly.push_back( Point(10,0) ) ;
  poly.push_back( Point(12,10) ) ;
  poly.push_back( Point(9,9) ) ;
  poly.push_back( Point(8,12) ) ;
  poly.push_back( Point(7,8) ) ;
  poly.push_back( Point(5,11) ) ;
  poly.push_back( Point(0,9) ) ;
     
  // You can pass the polygon via an iterator pair
  SsPtr iss = CGAL::create_interior_straight_skeleton_2(poly.vertices_begin(), poly.vertices_end());

#ifdef CGAL_LCC_USE_VIEWER
	Dart_handle dh = make_facet(lcc,poly);
   addSS(lcc,iss);
   display_lcc(lcc);
#endif // CGAL_LCC_USE_VIEWER

	return EXIT_SUCCESS;
}


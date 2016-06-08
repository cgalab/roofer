#ifndef CGALTYPES_H_
#define CGALTYPES_H_

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Combinatorial_map_constructors.h>

#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Linear_cell_complex.h>
#include <CGAL/bounding_box.h>
#include <CGAL/intersections.h>

using K 		     = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;

using Polygon        = CGAL::Polygon_2<K>;
using VertexIterator = Polygon::Vertex_iterator;
using EdgeIterator   = Polygon::Edge_const_iterator;

using Vector         = K::Vector_2;
using Point          = K::Point_2;
using Line           = K::Line_2;
using Ray            = K::Ray_2;
using Direction      = K::Direction_2;
using Edge           = K::Segment_2;
using BBox			 = K::Iso_rectangle_2;

using Transformation = CGAL::Aff_transformation_2<K>;

using Exact          = K::FT;

using Input          = std::vector<Point>;


#ifdef QTGUI

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Cartesian_converter.h>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>

using GUIKernel   = CGAL::Cartesian<double>;

using LCC_3       = CGAL::Linear_cell_complex<3>;
using Dart_handle = LCC_3::Dart_handle;
using Point3D     = LCC_3::Point;
using SsPtr       = std::shared_ptr<Polygon>;

#endif /* QTGUI */

#endif /* CGALTYPES_H_ */

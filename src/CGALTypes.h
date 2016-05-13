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

//using Trait          = CGAL::Arr_segment_traits_2<K>;
//using Polygon        = CGAL::Polygon_2<Trait>;
//using Vector         = Trait::Vector_2;
//using Point          = Trait::Point_2;
//using Line           = Trait::Line_2;
//using Ray            = Trait::Ray_2;
//using Direction      = Trait::Direction_2;
////using Edge           = Trait::Segment_2;
//using Edge           = Trait::X_monotone_curve_2;
//using Arrangement    = CGAL::Arrangement_2<Trait>;

#endif /* CGALTYPES_H_ */

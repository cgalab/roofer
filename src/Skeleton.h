/*
 * Skeleton.h
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include "Data.h"

class Skeleton {
public:
	Skeleton(std::list<std::string>& args);
	virtual ~Skeleton();

	bool guiEnabled() {return data.config.gui;}

	void handleNextEvent(Event& e);

	Data data;

private:
	void createSkeleton();

	void createInitialWavefront();
	void createBisectorRays();

	void computeEdgeEvents();
	void computeSplitEvents();
	void computeCreateEvents();

	void startWavefrontPropagation();

	Event  computeEdgeEvent(WavefrontIterator a, WavefrontIterator b);
	Vertex computeVelocity(Point& v_a, Point& v_b, Point& v_c);

	void handleEdgeEvent(Event& e);
	void handleSplitEvent(Event& e);
	void handleDivideEvent(Event& e);
	void handleCreateEvent(Event& e);
};
// https://www.ics.uci.edu/~dock/manuals/cgal_manual/Sweep_line_2/Chapter_main.html
/*
 *** SWEPP LINE
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_cached_traits_2.h>
#include <CGAL/Sweep_line_2.h>
#include <vector>

typedef CGAL::Quotient<CGAL::MP_Float>                  NT;
typedef CGAL::Cartesian<NT>                             Kernel;
typedef CGAL::Arr_segment_cached_traits_2<Kernel>       Traits;
typedef Traits::Point_2                                 Point_2;
typedef Traits::Curve_2                                 Curve_2;
typedef std::list<Curve_2>                              CurveList;
typedef CurveList::iterator                             CurveListIter;
typedef CGAL::Sweep_line_2<CurveListIter, Traits>       Sweep_line;

int main()
{
  CurveList  segments;
  Curve_2 c1(Point_2(1,5), Point_2(8,5));
  Curve_2 c2(Point_2(1,1), Point_2(8,8));
  Curve_2 c3(Point_2(3,1), Point_2(3,8));
  Curve_2 c4(Point_2(8,5), Point_2(8,8));

  segments.push_back(c1);
  segments.push_back(c2);
  segments.push_back(c3);
  segments.push_back(c4);

  std::list<Curve_2> subcurves;
  Sweep_line sl;
  sl.get_subcurves(segments.begin(), segments.end(),
		   std::back_inserter(subcurves), true);

  for (std::list<Curve_2>::iterator scv_iter = subcurves.begin();
       scv_iter != subcurves.end(); scv_iter++)
    std::cout << *scv_iter << std::endl;
  return 0;
}
 * */


#endif /* SKELETON_H_ */

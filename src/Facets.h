/*
 * roofer is written in C++ and uses CGAL.  It computes straight skeleton roofs
 * as well as minimum- and maximum-volume roofs over a simple polygon.
 * Copyright (C) 2016 - Günther Eder - roofer@geder.at
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef FACETS_H_
#define FACETS_H_


#include "CGALTypes.h"

#include "Definitions.h"
#include "SweepLine.h"

using namespace std;

class PointExt : public Point {
public:
//	using Point::Point;
	PointExt(Point p):Point(p),nextList(NOLIST) {}
	PointExt(int next):Point(),nextList(next) {}
	int nextList;
	friend ostream& operator<<(ostream& os, const PointExt& p);
};

// the lists are directly referenced by the arrangements lines
using AllLists		  = vector<list<PointExt>>;

using PointToZ        = map<Point,double>;

//// a facet groups together all lists that are part of one facet
//using Facet 		  = vector<int>;

/* for every edge we have and idx to a list correspondig to a facet */
using AllFacets 	  = map<EdgeIterator,list<int>>;

/* ListIdx points to a list entry for a specific allfacets, list */
using ListToFacet	  = map<int,list<int>::iterator>;

class RoofFacets {
public:
	RoofFacets();

	void handleEvent(SweepEvent *event);
	void addBaseCell(ALIterator& line);

	bool aGreaterB(Point a, Point b, EdgeIterator base);

	void setMinimizing() { if(!maximize) minimize = true; }
	void setMaximizing() { if(!minimize) maximize = true; }

	inline void setPolygon(const Polygon* poly)   { polygon   = poly; }
	inline void setConfig(const Config* conf)     { config    = conf;}

	inline void setSweepLine(SweepLine* sl)       { sweepLine = sl;}

	AllLists    allLists;
	AllFacets   allFacets;
	ListToFacet listToFacet;

	PointToZ    zMap;

	void printAllLists();
private:
	void handleEdgeEvent(SweepEvent* event);
	void handleSplitEvent(SweepEvent* event);

	bool handleCreateEventA(SweepEvent* event);
	bool handleCreateEventB(SweepEvent* event);
	bool handleVertexEvent(SweepEvent* event);
	bool handlePossibleGhostVertexEnd(SweepEvent* event);
	void handleEnterLeaveEvent(SweepEvent* event);

	void handleLeaveEvent(SweepItem* cell);
	void handleEnterEvent(SweepItem* cell);

	void handleGhostInsert(SweepEvent* event);

	void handleMergeEvent(SweepEvent* event);

	vector<SweepItem*> checkColinearCells(vector<SweepItem*>& cells);

	void addPointToCurrentList(SweepItem* cell);

	/* a->left to b->left or b->right to a->right */
	void turnLefRightOnIntersection(SweepItem* cell);

	inline void insertToZmap(Point p, double v) {zMap[p] = v; LOG_IF(config->verbose,INFO) << " zINFO:(" <<v << ") ";}

	bool			  minimize, maximize;

	const Polygon* 	  polygon;
	const Config*     config;
	SweepLine*  	  sweepLine;

	EdgeIterator next(EdgeIterator i);
	EdgeIterator prev(EdgeIterator i);
};

#endif /* FACETS_H_ */

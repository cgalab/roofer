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

/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_


#include <memory>
#include <assert.h>
#include <ctime>

#include <list>

#include <sys/stat.h>

#include "CGALTypes.h"
#include "Definitions.h"
#include "SweepLine.h"
#include "Facets.h"

using namespace std;

using AllArrangements = map<EdgeIterator,RoofFacets>;

class Data {
public:
    Input        input;
	Polygon    	 polygon;
	BBox		 bbox;

	/* handles the sweepLine algorithm (events in a global queue and line arrangements
	 * in a per base-line vector */
	SweepLine	 sweepLine;

	/* handles the cell arrangements and the resulting skeleton */
	RoofFacets	 facets;

	Config       config;

	Data();
	virtual ~Data();

	bool evaluateArguments(list<string> args);
	void writeOutput();

	inline Line getBBoxLine(int i) {
		if(i == 0) {
			return Line(Point(bbox.xmin(),bbox.ymin()),Point(bbox.xmax(),bbox.ymin()));
		} else if(i == 1) {
			return Line(Point(bbox.xmax(),bbox.ymin()),Point(bbox.xmax(),bbox.ymax()));
		} else if(i == 2) {
			return Line(Point(bbox.xmax(),bbox.ymax()),Point(bbox.xmin(),bbox.ymax()));
		} else if(i == 3) {
			return Line(Point(bbox.xmin(),bbox.ymax()),Point(bbox.xmin(),bbox.ymin()));
		}
		return Line();
	}

	friend VertexIterator next(const Polygon* poly, VertexIterator i);
	friend VertexIterator prev(const Polygon* poly, VertexIterator i);

private:
	bool fileExists(const string& fileName);
	bool loadFile(const string& fileName);
	void printHelp();
	void printLongHelp();

	void writeOBJ();
	void writePOLY();

	bool parseOBJ(const vector<string>& lines);
	bool parsePOLY(const vector<string>& lines);

	string currentTimeStamp();
	void getNormalizer(double& xt, double& xm, double& yt, double& ym, double& zt, double& zm);
};

#endif /* DATA_H_ */

/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_


#include <vector>
#include <list>
#include <memory>

#include <assert.h>

#include <sys/stat.h>

#include "CGALTypes.h"
#include "Definitions.h"
#include "SweepLine.h"
#include "Facets.h"

using namespace std;

struct Config {
	Config():gui(false),minimize(false),maximize(false),fileName("") {
		printOptions = "[-min|-max] [-gui] <filename>";
	}

	bool 		gui;
	bool 		minimize;
	bool 		maximize;
	string      fileName;

	string 		printOptions;
};

using AllArrangements = map<EdgeIterator,RoofFacets>;

class Data {
public:
    Input                input;
	Polygon    	  		 polygon;
	BBox				 bbox;

	/* handles the sweepLine algorithm (events in a global queue and line arrangements
	 * in a per base-line vector */
	SweepLine			 sweepLine;

	/* handles the cell arrangements and the resulting skeleton */
	RoofFacets			 facets;

	Config        		 config;

	Data();
	virtual ~Data();

	bool evaluateArguments(list<string> args);

	friend VertexIterator next(const Polygon* poly, VertexIterator i);
	friend VertexIterator prev(const Polygon* poly, VertexIterator i);

private:
	bool fileExists(string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */

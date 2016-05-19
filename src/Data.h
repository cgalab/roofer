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
#include "SweepLine.h"
#include "Arrangement.h"

using namespace std;

enum class EventType {EDGE,SPLIT,CREATE,MERGE,EMPTY};


struct Config {
	Config():gui(false),maximize(true),fileName("") {
		printOptions = "[-min|-max] [-gui] <filename>";
	}

	bool 		gui;
	bool 		maximize;
	string      fileName;

	string 		printOptions;
};

using AllArrangements = map<EdgeIterator,Arrangement>;

class Data {
public:
    Input                input;
	Polygon    	  		 polygon;
	BBox				 bbox;

	/* handles the sweepLine algorithm (events in a global queue and line arrangements
	 * in a per base-line vector */
	SweepLine			 sweepLine;

	/* handles the cell arrangements and the resulting skeleton */
	AllArrangements		 allArrangements;

	Config        		 config;



    //Transformation 		 rotateNintyLeft;

	Data();
	virtual ~Data();

	inline void addBaseCells(ArrangementLine& line) {
		allArrangements[line.base].addBaseCell(line);
	}

	inline void checkLowerChainEnds(EdgeIterator& e) {
		allArrangements[e].checkLowerChainEnds();
	}

	bool evaluateArguments(list<string> args);

	VertexIterator next(VertexIterator i);
	VertexIterator prev(VertexIterator i);

private:
	bool fileExists(string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */

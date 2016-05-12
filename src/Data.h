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

#include <sys/stat.h>

#include "CGALTypes.h"
#include "SweepLine.h"

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


using Input               = vector<Point>;

//using Cell       		  = vector<Point>;
//using Facet      		  = vector<Cell>;

class Data {
public:
    Input                input;
	Polygon    	  		 polygon;

	SweepLine			 sweepLine;

	Config        		 config;

    //Transformation 		 rotateNintyLeft;

	Data();
	virtual ~Data();

	bool evaluateArguments(list<string> args);

	VertexIterator next(VertexIterator i);
	VertexIterator prev(VertexIterator i);

private:
	bool fileExists(string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */

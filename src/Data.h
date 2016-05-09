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

// TODO: use arrangement class for this!
//struct LineArrangement {
//    LineArrangement(Edge e, p)
//    {}
//
//    vector<Ray>  lines;
//    vector<int>  sweepLineStatus;
//
//
//};


/* Intersection of lines in a line arrangemnt */
//struct EventPoint {
//    /* e ... edge of current line arrangement,
//       e_i, e_j ... bisectors that intersect e | e_i and e | e_j */
//    Exact squaredHeight;
//    EdgeIterator e, e_i, e_j;
//
//	EventPoint(Exact psquaredHeight, EdgeIterator pe, EdgeIterator pe_i, EdgeIterator pe_j)
//	: squaredHeight(psquaredHeight),e(pe),e_i(pe_i),e_j(pe_j) {}
//};

///* event location is the point itself.  */
//struct Event {
//	EventType 	   type;
//    Point          point;
//	Exact 		   height;
//
//    int edgeIndex;
//    int edgeIndexA;
//    int edgeIndexB;
//
//	Event(Point v, EventType t, K::FT _height, int _self, int _a, int _b)
//	: type(t),point(v),height(_height),edgeIndex(_self),edgeIndexA(_a),edgeIndexB(_b) {}
//};

//struct Compare {
//    bool operator() (Event &a, Event &b) {
//    	return a.height > b.height;
//    }
//};

using Input               = vector<Point>;

//using EventPoints 		  = vector<vector<Point> >;

//using EventQueue 		  = priority_queue<Event,vector<Event>, Compare >;
//using AllLineArrangements = vector<Arrangement>;
//using Cell       		  = vector<Point>;
//using Facet      		  = vector<Cell>;

class Data {
public:
    Input                input;
	Polygon    	  		 polygon;

//	BBox 				 bbox;
//	Exact				 bboxDiagonal;
//
//	EventPoints			 allEventPoints;

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

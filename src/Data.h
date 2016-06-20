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


enum class OutputType : int {OBJ=0,POLY,NONE,OBJ3D};

struct Config {
	Config():gui(false),minimize(false),maximize(false),fileName(""),outputType(OutputType::NONE),outputFileName("") {
#ifdef QTGUI
		printOptions = "[-min|-max] [-gui] [-poly <filename>] [-obj[3d] <filename>] <filename>";
#else
		printOptions = "[-min|-max] [-poly <filename>] [-obj[3d] <filename>] <filename>";
#endif
	}

	bool 		gui;
	bool 		minimize;
	bool 		maximize;
	string      fileName;

	OutputType  outputType;
	string		outputFileName;

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
	void writeOutput();

	friend VertexIterator next(const Polygon* poly, VertexIterator i);
	friend VertexIterator prev(const Polygon* poly, VertexIterator i);

private:
	bool fileExists(const string& fileName);
	bool loadFile(const string& fileName);
	void printHelp();

	void writeOBJ(const string& fileName);
	void writePOLY(const string& fileName);

	bool parseOBJ(const vector<string>& lines);
	bool parsePOLY(const vector<string>& lines);
};

#endif /* DATA_H_ */

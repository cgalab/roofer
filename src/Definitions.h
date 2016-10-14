#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <iostream>

#include "easyloggingpp/src/easylogging++.h"

using namespace std;

#define NIL    -1
#define NOLIST -2
#define ZSCALE 0.5

#define INFPOINT Point(numeric_limits<double>::max(),numeric_limits<double>::max())

//enum class EventType : int {EMPTY=0,EDGE,SPLIT,CREATE1,CREATE2,MERGE,CREATE1ORENTER,CREATE2ORMERGE,ENTER};

enum class OutputType : int {OBJ=0,POLY,NONE,OBJ3D};

struct Config {
	Config():
		gui(false),minimize(false),maximize(false),
		fileName(""),verbose(false),silent(false),
		outputType(OutputType::NONE),outputFileName("") {

		printOptions = "[-h] [-v] [-s] ";
#ifdef QTGUI
		printOptions += "[-gui] ";
#endif
		printOptions += "[-min|-max] [-poly|-obj|-obj3d <filename>] <filename>";
	}

	bool 		gui;
	bool 		minimize;
	bool 		maximize;
	string      fileName;

	bool 		verbose;
	bool		silent;

	OutputType  outputType;
	string		outputFileName;

	string 		printOptions;
};


#endif

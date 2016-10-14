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

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <iostream>

#include "easyloggingpp/src/easylogging++.h"

using namespace std;

#define NIL    -1
#define NOLIST -2
#define ZSCALE 0.5

#define INFPOINT Point(numeric_limits<double>::max(),numeric_limits<double>::max())

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

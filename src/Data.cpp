/*
 * Data.cpp
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#include "Data.h"

//#include "ImportOBJ.h"

// TEST

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>

#include <CGAL/bounding_box.h>


Data::Data() {
	facets.setPolygon(&polygon);
	facets.setConfig(&config);

	sweepLine.setConfig(&config);
}

//Data():rotateNintyLeft(CGAL::ROTATION, 1, 0) {}

Data::~Data() {}

bool Data::loadFile(const string& fileName) {
	if(fileExists(fileName)) {
		ifstream in(fileName);
		vector<string> lines;
		for(string line; getline(in, line);) {
			lines.push_back(line);
		}

		string extension = (fileName.find(".")  != string::npos) ? fileName.substr(fileName.find_last_of(".")+1) : "";
		transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if(extension == "obj") {
			/* wavefront obj format - https://en.wikipedia.org/wiki/Wavefront_.obj_file */
			if(!parseOBJ(lines)) {return false;}
		} else if(extension == "poly") {
			/* triangle's poly file format - https://www.cs.cmu.edu/~quake/triangle.poly.html */
			if(!parsePOLY(lines)) {return false;}
		}

		/* create bounding box from input vertices */
		bbox = CGAL::bounding_box(input.begin(),input.end());

		return true;
	}

	return false;
}

VertexIterator next(const Polygon* polygon, VertexIterator i) {
	return (i+1 == polygon->vertices_end()) ? polygon->vertices_begin() : i+1;
}
VertexIterator prev(const Polygon* polygon, VertexIterator i) {
	return (i == polygon->vertices_begin()) ? polygon->vertices_end()-1 : i-1;
}

bool Data::evaluateArguments(std::list<std::string> args) {
	string argument;
	bool fileLoaded = false;

	if(args.empty()) {
		printHelp();
		return false;
	} else {
		while(!args.empty()) {
			argument = args.front();
			args.pop_front();
			if (argument == "-max") {
				config.maximize  	= true;
				facets.setMaximizing();
			} else if (argument == "-min") {
				config.minimize 	= false;
				facets.setMinimizing();
			} else if (argument == "-gui") {
				config.gui 			= true;
			} else if (argument == "-help" || argument == "-h") {
				printHelp();
				printLongHelp();
				return false;
			} else if (argument == "-v") {
				config.verbose 		= true;
			} else if (argument == "-s") {
				config.silent 		= true;
			} else if (argument == "-obj" || argument == "-obj3d") {
				if(args.empty()) {return false;}

				config.outputType = OutputType::OBJ;
				if(argument == "-obj3d") {config.outputType = OutputType::OBJ3D;}

				argument = args.front();
				args.pop_front();
				config.outputFileName = argument;
			} else if (argument == "-poly") {
				cout << ".poly is not supported yet!" << endl;
				return false;
			} else if(args.empty()){
				config.fileName = argument;
				fileLoaded = loadFile(argument);
			} else {
				cout << argument << " is no valid option or filename!" << std::endl;
			}
		}
	}

	return fileLoaded;
}

bool Data::fileExists(const string& fileName) {
  struct stat buffer;
  return (stat (fileName.c_str(), &buffer) == 0);
}


void Data::printHelp() {
	string name = " Roofer ";
	string usage = "usage: roofer ";

	int frameLength = usage.length() + config.printOptions.length() + 4;
	int frameNameLength = (frameLength/2) - ((name.length())/2);

	for(auto i=0; i < frameNameLength; ++i) {cout << "-";}
	cout << name;
	for(auto i=0; i < frameNameLength+1; ++i) {cout << "-";}
	cout << endl;

	cout << "| " << usage << config.printOptions << " |" << endl;

	for(auto i=0; i < frameLength; ++i) {cout << "-";}
	cout << endl;
}

bool Data::parseOBJ(const vector<string>& lines) {
	vector<vector<int>> faces;

	for(auto l : lines) {
		istringstream buf(l);
		istream_iterator<string> beg(buf), end;
		vector<string> tokens(beg, end);

		auto isVertex = false;
		auto isFacet = false;

		double p_x=-INFINITY, p_y=-INFINITY, p_z=0;
		vector<int> facetList;

		for(auto& s : tokens) {
			/* comments not supported or needed obj parameters */
			// TODO: there are more OBJ parameters that this:
			if(s == "#" || s == "o" || s == "vt" || s == "vn" ||
			   s == "g" || s == "usemtl" || s == "s" || s == "off" ||
			   s == "mtllib" || s == "Ka" || s == "Kd" || s == "Ks" ||
			   s == "d" || s == "newmtl" || s == "illum" || *s.begin() == 'm' ||
			   *s.begin() == '-' || *s.begin() == 'b') {
				break;
			}

			if(isVertex) {
				if(p_x == -INFINITY) {
					p_x = atof(s.c_str());
				} else if(p_y == -INFINITY){
					p_y = atof(s.c_str());
				} else if(p_z == 0) {
					p_z = atof(s.c_str());
				}
			} else if(isFacet) {
				facetList.push_back(atoi(s.c_str()));
			}

			if(s == "v") {isVertex = true;}
			if(s == "f") {isFacet  = true;}
		}

		if(p_x != -INFINITY && p_y != -INFINITY) {
			input.push_back(Point(p_x,p_y));
		}
		if(p_z != 0) {
			facets.zMap[Point(p_x,p_y)] = p_z;
		}

		if(!facetList.empty()) {
			faces.push_back(facetList);
		}
	}

	/* create polygon from input vertices */
	for(auto face : faces) {
		for(auto idx : face) {
			if(idx==0) {
				cout << "WARNING: idx in obj should not be 0!" << endl;
			} else {
				polygon.push_back(input[idx-1]);
			}
		}
	}

	if(!input.empty() && !faces.empty()) {return true;}
	return false;
}

bool Data::parsePOLY(const vector<string>& lines) {
	cout << "no yet supported!";
	return false;
}

void Data::writePOLY(const string& fileName) {
	cout << "no yet supported!";
}

void Data::writeOutput() {
	switch(config.outputType) {
	case OutputType::OBJ3D:
	case OutputType::OBJ:  writeOBJ(config.outputFileName);  break;
	case OutputType::POLY: writePOLY(config.outputFileName); break;
	case OutputType::NONE: break;
	}
}

void Data::writeOBJ(const string& fileName) {
	set<Point> allPoints;

	map<Point,int> vertexMap;
	vector<Point> rightOrder;
	int cnt = 0;

	double xt, yt, zt, xm, ym, zm;
	getNormalizer(xt,xm,yt,ym,zt,zm);

	for(auto l : facets.allLists) {
		for(auto p : l) {
			if(p.nextList == NOLIST) {
				Point pin(p);
				allPoints.insert(pin);
			}
		}
	}

	for(auto p : allPoints) {
		vertexMap[p] = ++cnt;
		rightOrder.push_back(p);
	}

	ofstream outfile (config.outputFileName,std::ofstream::binary);
	outfile << "# OBJ-File autogenerated by roofer from file ("
			<< config.fileName << ") - "
			<< currentTimeStamp() <<  endl;
	for(auto v : rightOrder) {
		double x = (v.x().doubleValue() * xm) - xt;
		double y = (v.y().doubleValue() * ym) - yt;
		if(config.outputType == OutputType::OBJ3D) {
			double z = (facets.zMap[v] * zm)  - zt;
			outfile << "v " << x << " " << y << " " << z << endl;
		} else {
			outfile << "v " << x << " " << y << endl;
		}
	}

	for(auto& facet : facets.allFacets) {
		for(auto f : facet.second) {
			auto list = &facets.allLists[f];

			if(list->empty()) continue;

			auto listIt = list->begin();
			outfile << "f";
			do {
				if(listIt->nextList != NOLIST) {
					list = &facets.allLists[listIt->nextList];
					listIt = list->begin();
				} else {
					Point p(*listIt);
					outfile << " " << vertexMap[p];
					++listIt;
				}
			} while(listIt != list->end() && listIt->nextList != f);
			outfile <<  endl;
		}
	}
	outfile << endl;
	outfile.close();
}


void Data::getNormalizer(double& xt, double& xm, double& yt, double& ym, double& zt, double& zm) {
	xt = bbox.min().x().doubleValue() + 1;
	yt = bbox.min().y().doubleValue() +	1;
	zt = 0.0;

	double x_span  = bbox.max().x().doubleValue() - bbox.min().x().doubleValue();
	double y_span  = bbox.max().y().doubleValue() - bbox.min().y().doubleValue();

	double z_max = 0;
	for(auto z : facets.zMap) {
		if(z.second > z_max) { z_max = z.second; }
	}

	xm = (x_span != 0) ? 2.0/x_span : 1;
	ym = (x_span != 0) ? 2.0/y_span : 1;
	zm = (z_max  != 0) ? 1.0/z_max  : 1;
	zm *= ZSCALE;
}

string Data::currentTimeStamp() {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
	std::string str(buffer);
	return str;
}



void Data::printLongHelp() {
	cout << "  -h \t\t\tprint this help" << endl;
	cout << "  -v \t\t\tverbose mode, shows more information about the computation" << endl;
	cout << "  -s \t\t\tsilent mode, shows no information" << endl;
#ifdef QTGUI
	cout << "  -gui \t\t\tenable GUI" << endl;
#endif
	cout << "  -min | -max\t\tminimize or maximize the resulting roof" << endl;
//	cout << "  -poly \t\twrite output in triangle's poly format" << endl;
	cout << "  -obj \t\t\twrite output in wavefront obj format (2D coordinates)" << endl;
	cout << "  -obj3d \t\twrite output in wavefront obj format (3D coordinates)" << endl;
	cout << "  <filename> \t\tinput type is wavefront obj format" << endl;
}






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

		if(fileName.find(".")  != string::npos && fileName.substr(fileName.find_last_of(".")+1) == "obj") {
			/* wavefront obj format - https://en.wikipedia.org/wiki/Wavefront_.obj_file */
			if(!parseOBJ(lines)) {return false;}
		} else if(fileName.find(".")  != string::npos && fileName.substr(fileName.find_last_of(".")+1) == "poly") {
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
				config.maximize = true;
				facets.setMaximizing();
			} else if (argument == "-min") {
				config.minimize = false;
				facets.setMinimizing();
			} else if (argument == "-gui") {
				config.gui = true;
			} else if (argument == "-obj" || argument == "-obj3d") {
				if(args.empty()) {return false;}

				config.outputType = OutputType::OBJ;
				if(argument == "-obj3d") {config.outputType = OutputType::OBJ3D; cout << "3d type" << endl;}

				argument = args.front();
				args.pop_front();
				config.outputFileName = argument;
			} else if (argument == "-poly") {
				cout << ".poly is not supported yet!" << endl;
				return false;
			} else {
				config.fileName = argument;
				fileLoaded = loadFile(argument);
				if(!fileLoaded) {
					cout << "no valid option or filename provided!" << std::endl;
				}
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
	std::cout << "------------------------ Roofer --------------------------" << std::endl;
	std::cout << "| usage: roofer " << config.printOptions << "            |" << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;
}

bool Data::parseOBJ(const vector<string>& lines) {
	vector<vector<int>> faces;

	for(auto l : lines) {
		istringstream buf(l);
		istream_iterator<string> beg(buf), end;
		vector<string> tokens(beg, end);

		auto isVertex = false;
		auto isFacet = false;

		double p_x=-INFINITY, p_y=-INFINITY;
		vector<int> facetList;

		for(auto& s : tokens) {
			if(isVertex) {
				if(p_x == -INFINITY) {
					p_x = atof(s.c_str());
				} else {
					p_y = atof(s.c_str());
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
	case OutputType::OBJ:  writeOBJ(config.outputFileName); break;
	case OutputType::POLY: writePOLY(config.outputFileName); break;
	case OutputType::NONE: break;
	}
}

void Data::writeOBJ(const string& fileName) {
	set<Point> allPoints;

	map<Point,int> vertexMap;
	vector<Point> rightOrder;
	int cnt = 0;

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
	outfile << "# OBJ-File autogenerated by roofer from " << config.fileName <<  endl;
	for(auto v : rightOrder) {
		if(config.outputType == OutputType::OBJ3D) {
			outfile << "v " << v.x().doubleValue() << " " << v.y().doubleValue() << " " << ZSCALE * facets.zMap[v] << endl;
		} else {
			outfile << "v " << v.x().doubleValue() << " " << v.y().doubleValue() << endl;
		}
	}

	for(auto& facet : facets.allFacets) {
		for(auto f : facet.second) {
			auto list = &facets.allLists[f];

			if(list->empty()) continue;

			auto listIt = list->begin();
			outfile << "f";
			do {
				//cout << *listIt << "(" << listIt->nextList << ") - ";
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





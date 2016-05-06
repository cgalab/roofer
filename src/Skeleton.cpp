/*
 * Skeleton.cpp
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#include <iostream>
#include <exception>


#include "Data.h"
#include "Skeleton.h"
#include "GUI.h"

using namespace std;

Skeleton::Skeleton(list<string>& args) {
	/* evaluate arguments and load data */
	if(!data.evaluateArguments(args)) return;

	/* compute the sought bisector graph */
	createSkeleton();

//	/* display if needed */
//		#ifdef CGAL_LCC_USE_VIEWER
//			if(data.config.gui) {
//				gui.make_facet(data.polygon);
//				gui.show(data.iss);
//			}
//		#endif // CGAL_LCC_USE_VIEWER
}

Skeleton::~Skeleton() {}

void Skeleton::createSkeleton() {
	/***
	 * create initial wavefront
	 ***/
	cout << "generate wavefront...";
	createLineArrangements();
	cout << "done" << endl;

//	/* compute initial bisector rays */
//	cout << "generate bisector-rays...";
//	createBisectorRays();
//	cout << "done" << endl;
//
//	/* compute initial edge-events */
//	cout << "compute edge-events...";
//	computeEdgeEvents();
//	cout << "done" << endl;
//
//	/* compute initial split-events */
//	cout << "compute split-events...";
//	computeSplitEvents();
//	cout << "done" << endl;
//
//	/* compute initial create-events */
//	cout << "compute create-events...";
//	computeCreateEvents();
//	cout << "done" << endl;

	/* start wavefront propagation */
#ifdef QTGUI
	if(!data.config.gui) {
#endif
		cout << "start wavefront propagation...";
		startPlaneSweep();
		cout << "done" << endl;
#ifdef QTGUI
	}
#endif
}

void Skeleton::createLineArrangements() {
	for(auto edgeIt=data.polygon.edges_begin(); edgeIt != data.polygon.edges_end(); ++edgeIt) {
		Line baseLine(edgeIt->vertex(0),edgeIt->vertex(1));
		auto bisectors = new list<Ray>();

		for(auto it=data.polygon.edges_begin(); it != data.polygon.edges_end(); ++it) {

			// Change direction of linedirection in order to become the correct bisector
			Line iteratorLine(it->vertex(1), it->vertex(0));
			Line bisector(CGAL::bisector(baseLine,iteratorLine));

			auto intersection = CGAL::intersection(baseLine,iteratorLine);

			if(!intersection.empty()) {
				if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {

					bisectors->push_back(Ray(*ipoint,bisector));

				} else {
					cout << "No Intersection? - Parallel Input Lines (not yet supported)" << endl;
					assert(false);
				}
			} else {
				cout << "Object empty?" << endl;
				assert(false);
			}

		}

		Arrangement arr;
		CGAL::insert(arr, bisectors->begin(), bisectors->end());
		data.lineArrangements.push_back(arr);
	}
}

//void Skeleton::createBisectorRays() {
//	for(auto v_j = data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
//		auto v_i = data.prev(v_j);
//		auto v_k = data.next(v_j);
//
//		if(CGAL::right_turn(*v_i,*v_j,*v_k)) {
//			v_j->reflex    = true;
//			cout << "ref ";
//		} else {
//			v_j->reflex	   = false;
//		}
//
//		v_j->velocity = computeVelocity(*v_i,*v_j,*v_k);
//
//	}
//}

//Vertex Skeleton::computeVelocity(Point& v_a, Point& v_b, Point& v_c) {
//	Line a(v_a,v_b);
//	Line b(v_c,v_b);
//	Line bis = CGAL::bisector(a,b);
//cout << "a " << v_a << ", " << v_b << ", " << v_c << endl; fflush(stdout);
//	Point pointOnBis = bis.projection(v_a);
//	Ray direction;
//cout << "b";fflush(stdout);
//
//	if(CGAL::left_turn(v_a,v_b,v_c)) {
//		direction = Ray(v_b,pointOnBis);
//	} else if(CGAL::right_turn(v_a,v_b,v_c)) {
//		Ray r(pointOnBis,v_b);
//		direction = Ray(v_b,r.direction());
//	} else {
//		cout << "Polygon contains co-linear segments (angle equal to pi). This is not supported yet!" << endl;
//		assert(false);
//	}
//cout << "c";fflush(stdout);
//
//	/* create a unit vector normal to one of the line-segments incident at v_j */
//	Line a_p(a.perpendicular(v_b+direction.to_vector()));
//cout << "d";fflush(stdout);
//
//
//	auto divSquared = a_p.direction().to_vector().squared_length();
//	if(divSquared == CGAL::ZERO) {
//		cout << "perpend. direction vector has zero length!" << endl;
//		assert(false);
//	} else {
//		cout << "dividend: " << divSquared << endl;
//		fflush(stdout);
//	}
//
//	auto pDirUnitVect = a_p.direction().to_vector() / CGAL::sqrt(divSquared);
//
//	cout << "len: " << pDirUnitVect.squared_length() << " ";
//	auto a_offset = Line(v_a+pDirUnitVect,v_b+pDirUnitVect);
//
////	auto perpendVector = a_p.to_vector();
////	auto transVector   = perpendVector / CGAL::sqrt(perpendVector.squared_length());
//
//	/* transform the line by 1 in direction of the unit normal (parallel) */
////	Transformation translate(CGAL::TRANSLATION, transVector);
////	auto a_translated = a.transform(translate);
//
//	/* intersect a with the bisector ray and use the distance from v_j to the intersection

//	 * as speed of the vertex */
////	auto intersection = CGAL::intersection(direction,a_translated);
//	auto intersection = CGAL::intersection(direction,a_offset);
//	if(!intersection.empty()) {
//		if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
//
//			auto dist = CGAL::sqrt(CGAL::squared_distance(*ipoint,v_b));
//
//			auto divDirSquared = direction.to_vector().squared_length();
//			if(divDirSquared == CGAL::ZERO){
//				cout << "perpend. direction vector has zero length!" << endl;
//				assert(false);
//			} else {
//				cout << "dividend: " << divDirSquared << endl;
//				fflush(stdout);
//			}
//
//			cout << "dist to int: [" << dist << "] " << "velocity: " << Vertex(dist * (direction.to_vector()/CGAL::sqrt(divDirSquared))) << endl;;
//			return Vertex(dist * (direction.to_vector()/CGAL::sqrt(divDirSquared)));
//
//		} else {
//			cout << "no intersection ???" << endl;
//		}
//	}
//	cout << "ERRROR!" << endl;
//	assert(false);
//	return Vertex(0,0);
//}

//Event Skeleton::computeEdgeEvent(WavefrontIterator a, WavefrontIterator b) {
//	assert(a != b);
//	cout << "bi [" << *a << "," << a->velocity << "] [" << *b << "," << b->velocity << "] "; fflush(stdout);
//
//	auto intersection = CGAL::intersection(a->getRay(),b->getRay());
//	cout << "ai ";fflush(stdout);
//	if(!intersection.empty()) {
//		if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
//			Line l(*a,*b);
//			cout << "be "; fflush(stdout);
//			Event e(*ipoint,EventType::EDGE,CGAL::sqrt(CGAL::squared_distance(l,*ipoint)),a,b,b);
//			//Event e(*ipoint,EventType::EDGE,CGAL::squared_distance(l,*ipoint),a,b,b);
//
//			return e;
//		}
//	}
//	return Event();
//}

//void Skeleton::computeEdgeEvents() {
//	for(auto v_i = data.wavefront.vertices_begin(); v_i != data.wavefront.vertices_end(); ++v_i) {
//		auto e = computeEdgeEvent(v_i,data.next(v_i));
//		if(e.type == EventType::EDGE) {
//			cout << e.time << " ";
//			data.eventQueue.push(e);
//		}
//	}
//}

//void Skeleton::computeSplitEvents() {
//	for(auto v_reflex = data.wavefront.vertices_begin(); v_reflex != data.wavefront.vertices_end(); ++v_reflex) {
//		/* we only need to consider reflex vertices for the split-events */
//		if(v_reflex->reflex) {
//			auto v_ii = data.next(v_reflex);
//			/* we take a line incident at the reflex vertex to calculate distance */
//			Line l_reference(*v_reflex,*v_ii);
//
//			/* circle through all 'lines' */
//			for(auto v_la = data.wavefront.vertices_begin(); v_la != data.wavefront.vertices_end(); ++v_la) {
//				auto v_lb = data.next(v_la);
//				if(v_la == v_reflex || v_lb == v_reflex) continue;
//
//				Line l(*v_la,*v_lb);
//
//				//auto val = v_reflex->getRay().to_vector() * l.to_vector().transform(data.rotateNintyLeft);
//				//cout << "bis dir vect * line normal vect = " << val << endl;
//				//fflush(stdout);
//				//if(val <= 0) continue;
//
//				/* bisector between the lines*/
//				auto l_bis = CGAL::bisector(l_reference,l);
//
//				/* intersection between bisector and ray from the reflex vertex */
//				auto intersection = CGAL::intersection(v_reflex->getRay(),l_bis);
//				if(!intersection.empty()) {
//					if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
//						/* to simplify the distance computation we compare the distance
//						 * not to the reflex vertex but to the supporting line of one of
//						 * its incident edges. */
//
//						//cout << CGAL::sqrt(CGAL::squared_distance(*ipoint,l_a)) << "/" <<CGAL::sqrt(CGAL::squared_distance(*ipoint,l_b)) << endl;
//						//if(CGAL::compare_distance(*ipoint,l_a,l_b) == CGAL::EQUAL) {
//
//						Event e(*ipoint,EventType::SPLIT, CGAL::sqrt(CGAL::squared_distance(*ipoint,l_reference)) ,v_reflex,v_la,v_lb);
//						//Event e(*ipoint,EventType::SPLIT, CGAL::squared_distance(*ipoint,l_reference) ,v_reflex,v_la,v_lb);
//						//e.time = CGAL::sqrt(CGAL::squared_distance(*ipoint,l_a));
//						//e.a    = v_i;		// the reflex vertex
//						//e.b    = v_j;		// vertex a of line segment
//						//e.c    = v_jj;	// vertex b of line segment
//
//						cout << e.time << " ";
//						data.eventQueue.push(e);
//						//} else {
//						//	cout << "distances do not match " <<
//						//	CGAL::squared_distance(l_a,*ipoint) << "," <<
//						//	CGAL::squared_distance(l_b,*ipoint) << endl;
//						//}
//					}
//				}
//			}
//
//		}
//	}
//}
//
//void Skeleton::computeCreateEvents() {
//	// TODO: ...
//}
//
void Skeleton::startPlaneSweep() {

	cout << "Start Plane Sweep!" << endl; fflush(stdout);
	while(!data.eventQueue.empty()) {
		auto e = data.eventQueue.top();
		data.eventQueue.pop();
		cout << data.eventQueue.size() << endl;

		handleNextEvent(e);

	}
}

//
//void Skeleton::handleNextEvent(Event& e) {
//	switch(e.type) {
//	case EventType::EDGE:   handleEdgeEvent(e);   break;
//	case EventType::SPLIT:  handleSplitEvent(e);  break;
//	case EventType::CREATE: handleCreateEvent(e); break;
//
//	default: break;
//	}
//}
//
//
//void Skeleton::handleEdgeEvent(Event& e) {
//	cout << "Edge Event ";
//
//	if(e.a->isInWavefront() && e.b->isInWavefront() && data.next(e.a) == e.b && e.a != e.b) {
//		cout << "!" << endl;
//
////		Event e_eval = computeEdgeEvent(e.a,e.b);
////		cout << e.time << " " << e_eval.time << " ";
////		if(e.time == e_eval.time) {
//
//			auto a = data.prev(e.a);
//			auto b = e.a;
//			auto c = data.next(e.b);
//
//			cout << "(" << *a << ", " << *c << ") -> ";
//			a->moveToTime(e.time);
//			c->moveToTime(e.time);
//			cout << "(" << *a << ", " << *c << ")" << endl;
//
//			/* only a triangle left in this polygon (does not have to be the last) */
//			if(a == c || a == b || b == c) {
//				cout << "vanishing event" << endl;
//				a->disable();
//				b->disable();
//				c->disable();
//				data.wavefront.erase(a);
//				data.wavefront.erase(b);
//				data.wavefront.erase(c);
//				return;
//			}
//
//			WavefrontPoint node(e);
//			node.start = e;
//			node.time = node.startTime = e.time;
//
//			b = data.wavefront.insert(c, node);
//
//			node.velocity = computeVelocity(*a,*b,*c);
//
//			e.a->disable();
//			e.b->disable();
//			data.wavefront.erase(e.a);
//			data.wavefront.erase(e.b);
//
//			cout << "LIST: " << *a << " -- " << *b << " -- " << *c << endl;
//			cout << "LIST: " << *a << " -- " << *(data.next(a)) << " -- " << *(data.next(data.next(a))) << endl;
//
//
//			auto e_a = computeEdgeEvent(a,data.next(a));
//			auto e_b = computeEdgeEvent(b,data.next(b));
//
//			if(e_a.type == EventType::EDGE) {
//				cout << e_a.time << " ";
//				data.eventQueue.push(e_a);
//			}
//
//			if(e_b.type == EventType::EDGE) {
//				cout << e_b.time << " ";
//				data.eventQueue.push(e_b);
//			}
//
//			cout << "EE(" << e.time << ") " << endl;
////		} else {
////			cout << "no intersection (A)" << endl;
////		}
//	} else {
//		cout << "no intersection (B)" << endl;
//	}
//}

void Skeleton::handleSplitEvent(Event& e) {
	cout << "Split Event" << endl;
}

void Skeleton::handleEdgeEvent(Event& e) {
	cout << "Divide Event" << endl;
}

void Skeleton::handleCreateEvent(Event& e) {
	cout << "Create Event" << endl;
}

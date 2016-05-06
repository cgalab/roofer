/*
 * GUI.h
 *
 *  Created on: May 28, 2015
 *      Author: gue
 */

#ifndef GUI_H_
#define GUI_H_

#ifdef QTGUI

#include <QApplication>
#include <QKeyEvent>

#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

#include <GL/gl.h>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Cartesian_converter.h>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include "Data.h"
#include "Skeleton.h"

using GUIKernel = CGAL::Cartesian<double>;

using LCC_3       = CGAL::Linear_cell_complex<3>;
using Dart_handle = LCC_3::Dart_handle;
using Point3D     = LCC_3::Point;
using SsPtr       = std::shared_ptr<Polygon>;

typedef typename GUIKernel::Point_3  Local_point;
typedef typename GUIKernel::Vector_3 Local_vector;

template<class LCC, int dim=LCC::ambient_dimension>
struct Geom_utils;

template<class LCC>
struct Geom_utils<LCC,3> {
	Local_point get_point(typename LCC::Vertex_attribute_const_handle vh)
	{ return converter(vh->point()); }

	Local_point get_point(typename LCC::Dart_const_handle dh)
	{ return converter(LCC::point(dh)); }

	Local_vector get_facet_normal(LCC& lcc, typename LCC::Dart_const_handle dh)	{
		Local_vector n = converter(CGAL::compute_normal_of_cell_2<LCC>(lcc,dh));
		n = n/(CGAL::sqrt(n*n));
		return n;
	}

	Local_vector get_vertex_normal(LCC& lcc, typename LCC::Dart_const_handle dh) {
		Local_vector n = converter(CGAL::compute_normal_of_cell_0<LCC>(lcc,dh));
		n = n/(CGAL::sqrt(n*n));
		return n;
	}
protected:
	CGAL::Cartesian_converter<typename LCC::Traits, GUIKernel> converter;
};

template<class LCC>
struct Geom_utils<LCC,2> {
	Local_point get_point(typename LCC::Vertex_attribute_const_handle vh) {
		Local_point p(converter(vh->point().x()),0,converter(vh->point().y()));
		return p;
	}

	Local_point get_point(typename LCC::Dart_const_handle dh)
	{ return get_point(LCC::vertex_attribute(dh)); }

	Local_vector get_facet_normal(LCC&, typename LCC::Dart_const_handle) {
		Local_vector n(0,1,0);
		return n;
	}

	Local_vector get_vertex_normal(LCC&, typename LCC::Dart_const_handle) {
		Local_vector n(0,1,0);
		return n;
	}
protected:
	CGAL::Cartesian_converter<typename LCC::Traits, GUIKernel> converter;
};

template<class LCC>
CGAL::Bbox_3 bbox(LCC& lcc) {
	CGAL::Bbox_3 bb;
	Geom_utils<LCC> geomutils;

	typename LCC::Vertex_attribute_range::const_iterator
	it=lcc.vertex_attributes().begin(), itend=lcc.vertex_attributes().end();
	if ( it!=itend ) {
		bb = geomutils.get_point(it).bbox();
		for( ++it; it!=itend; ++it) {
			bb = bb + geomutils.get_point(it).bbox();
		}
	}

	return bb;
}

class GUI : public QGLViewer {
public:
	GUI(Skeleton *s);
	virtual ~GUI();

	void start();

//	void show(const SsPtr& ss);
	Dart_handle make_facet(const Polygon& poly);

	void addSegment(Point& a, Point& b);

protected :
	virtual void draw();
	void drawEdges(Dart_handle ADart);
	void drawFacet(Dart_handle ADart, int AMark);

	virtual void init();
	void keyPressEvent(QKeyEvent *e);

	virtual QString helpString() const;

private:
	void addSS(const SsPtr& ss);
	void drawEvent(Event& e);
	void drawWavefrontPolygon(K::FT time);

	Skeleton *skeleton;
	Data     *data;

	LCC_3 	  lcc;
	SsPtr     iss;

	bool wireframe;
	bool flatShading;
	bool edges;
	bool vertices;

	Geom_utils<LCC_3> geomutils;
};

//template<class LCC>
//void display_lcc(LCC& alcc)
//{
//	int argc=1;
//	//typedef char* s;
//
//	const char* argv[2]={"lccviewer","\0"};
//	QApplication app(argc,const_cast<char**>(argv));
//
//	SimpleLCCViewerQt<LCC> mainwindow(alcc);
//	mainwindow.show();
//
//	app.exec();
//};

#endif /* QTGUI */

#endif /* GUI_H_ */

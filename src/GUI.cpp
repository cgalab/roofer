/*
 * GUI.cpp
 *
 *  Created on: May 28, 2015
 *      Author: gue
 */

#include "GUI.h"

#ifdef QTGUI

GUI::GUI(Skeleton *s) : QGLViewer(),
wireframe(true), flatShading(false),
edges(true), vertices(true) {
	skeleton = s;
	data     = &s->data;

	setWindowTitle("Min/Max Roofs");

	auto frame = new qglviewer::ManipulatedFrame();
	frame->setSpinningSensitivity(300);
	setManipulatedFrame(frame);

	resize(1024, 768);
	updateGL();

	make_facet(data->polygon);

	init();
	this->show();
}

GUI::~GUI() {}

//void GUI::show(const SsPtr& ss) {
//	display_lcc(lcc);
//}

Dart_handle GUI::make_facet(const Polygon& poly) {
	Dart_handle d =
			CGAL::make_combinatorial_polygon<LCC_3>(lcc,(unsigned int)poly.size());

	for(auto v = poly.vertices_begin(); v != poly.vertices_end(); ++v) {
		//for (unsigned int i=0; i<poly.size(); ++i) {

		Point3D p(v->x().doubleValue(),v->y().doubleValue(),0);

		lcc.set_vertex_attribute_of_dart(d, lcc.create_vertex_attribute(p));
		d = d->beta(1);
	}
	return d;
}

void GUI::addSegment(EdgeIterator& e) {
	Point3D p(e->vertex(0).x().doubleValue(),e->vertex(0).y().doubleValue(),0);
	Point3D q(e->vertex(1).x().doubleValue(),e->vertex(1).y().doubleValue(),0);

	lcc.make_segment(q,p);
}

void GUI::addSegment(Point3D& a, Point3D& b) {
	lcc.make_segment(a,b);
}
void GUI::addSegment(Point& a, Point& b) {
	Point3D p(a.x().doubleValue(),a.y().doubleValue(),0);
	Point3D q(b.x().doubleValue(),b.y().doubleValue(),0);

	lcc.make_segment(p,q);
}

void GUI::addSS(const SsPtr& ss) {
//	for ( Halfedge_const_iterator i = ss->halfedges_begin(); i != ss->halfedges_end(); ++i )
//	{
//		Point3D p(i->opposite()->vertex()->point().x(),i->opposite()->vertex()->point().y(),0);
//		Point3D q(i->vertex()->point().x(),i->vertex()->point().y(),0);
//
//		lcc.make_segment(p,q);
//	}
}

void GUI::keyPressEvent(QKeyEvent *e) {
	const Qt::KeyboardModifiers modifiers = e->modifiers();

	bool handled = false;
	if ((e->key()==Qt::Key_W) && (modifiers==Qt::NoButton))
	{
		wireframe = !wireframe;
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		handled = true;
		updateGL();
	}
	else if ((e->key()==Qt::Key_F) && (modifiers==Qt::NoButton))
	{
		flatShading = !flatShading;
		if (flatShading)
		{
			::glShadeModel(GL_FLAT);
			::glDisable(GL_BLEND);
			::glDisable(GL_LINE_SMOOTH);
			::glDisable(GL_POLYGON_SMOOTH_HINT);
			::glBlendFunc(GL_ONE, GL_ZERO);
			::glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
		}
		else
		{
			::glShadeModel(GL_SMOOTH);
			::glEnable(GL_BLEND);
			::glEnable(GL_LINE_SMOOTH);
			::glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
			::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		handled = true;
		updateGL();
	}
	else if ((e->key()==Qt::Key_E) && (modifiers==Qt::NoButton))
	{
		edges = !edges;
		handled = true;
		updateGL();
	}
	else if ((e->key()==Qt::Key_V) && (modifiers==Qt::NoButton))
	{
		vertices = !vertices;
		handled = true;
		updateGL();
	}
	else if ((e->key()==Qt::Key_Plus) && (modifiers==Qt::NoButton))
	{
		if(!data->sweepLine.queueEmpty()) {
			auto item = data->sweepLine.popEvent();

			drawEvent(item);
			skeleton->handleNextEvent(item);

			fflush(stdout);

//			skeleton->handleNextEvent(e);
//			drawWavefrontPolygon(e.time);
		}
		updateGL();
	}


	if (!handled)
		QGLViewer::keyPressEvent(e);
}

void GUI::drawFacet(Dart_handle ADart, int AMark) {
	::glBegin(GL_POLYGON);
	::glColor3f(.7,.7,.7);

	// If Flat shading: 1 normal per polygon
	if (flatShading)
	{
		Local_vector n = geomutils.get_facet_normal(lcc,ADart);
		::glNormal3d(n.x(),n.y(),n.z());
	}

	for (typename LCC_3::template Dart_of_orbit_range<1>::const_iterator
			it=lcc.template darts_of_orbit<1>(ADart).begin();
			it.cont(); ++it)
	{
		// If Gouraud shading: 1 normal per vertex
		if (!flatShading)
		{
			Local_vector n = geomutils.get_vertex_normal(lcc,it);
			::glNormal3d(n.x(),n.y(),n.z());
		}

		Local_point p = geomutils.get_point(it);
		::glVertex3d(p.x(),p.y(),p.z());

		lcc.mark(it, AMark);
		if ( lcc.dimension>=3 && !it->is_free(3) ) lcc.mark(it->beta(3), AMark);
	}
	// close the polygon
	if (!flatShading)
	{
		Local_vector n = geomutils.get_vertex_normal(lcc,ADart);
		::glNormal3d(n.x(),n.y(),n.z());
	}

	Local_point p = geomutils.get_point(ADart);
	::glVertex3d(p.x(),p.y(),p.z());

	::glEnd();
}

void GUI::drawEdges(Dart_handle ADart) {
	glDepthRange (0.0, 0.9);
	glBegin(GL_LINES);
	::glColor3f(.2,.2,.6);
	for (typename LCC_3::template Dart_of_orbit_range<1>::iterator
			it=lcc.template darts_of_orbit<1>(ADart).begin();
			it.cont(); ++it)
	{
		Local_point p =  geomutils.get_point(it);
		Dart_handle d2 = it->other_extremity();
		if ( d2!=NULL )
		{
			Local_point p2 = geomutils.get_point(d2);
			glVertex3f( p.x(),p.y(),p.z());
			glVertex3f( p2.x(),p2.y(),p2.z());
		}
	}
	glEnd();
	glDepthRange (0.1, 1.0);
}

void GUI::draw() {
	int facettreated = lcc.get_new_mark();
	int vertextreated = -1;

	if ( vertices) vertextreated=lcc.get_new_mark();

	for(typename LCC_3::Dart_range::iterator it=lcc.darts().begin(),
			itend=lcc.darts().end(); it!=itend; ++it)
	{
		if ( !lcc.is_marked(it,facettreated) )
		{
			drawFacet(it,facettreated);
			if ( edges) drawEdges(it);
		}

		if (vertices)
		{
			if ( !lcc.is_marked(it, vertextreated) )
			{
				Local_point p = geomutils.get_point(it);

				glDepthRange (0.0, 0.9);
				glBegin(GL_POINTS);
				::glColor3f(.6,.2,.8);
				glVertex3f(p.x(),p.y(),p.z());
				glEnd();
				glDepthRange (0.1, 1.0);

				CGAL::mark_cell<LCC_3, 0>(lcc, it, vertextreated);
			}
		}
	}

	CGAL_assertion(lcc.is_whole_map_marked(facettreated));

	if ( vertices)
	{
		CGAL_assertion(lcc.is_whole_map_marked(vertextreated));
		lcc.free_mark(vertextreated);
	}

	lcc.free_mark(facettreated);
}
void GUI::drawEvent(SweepEvent& event) {

	auto cont = event.getEventType();
	if(cont.first == EventType::EDGE) {
		for(auto i : event) {
			if(i.a->rightListIdx == i.b->leftListIdx && i.a->rightListIdx != NOLIST) {
				if(data->bbox.xmin() < i.intersectionPoint.x().doubleValue() &&
						data->bbox.ymin() < i.intersectionPoint.y().doubleValue() &&
						data->bbox.xmax() > i.intersectionPoint.x().doubleValue() &&
						data->bbox.ymax() > i.intersectionPoint.y().doubleValue() ) {

					auto& l = data->facets.allLists[i.a->rightListIdx];

					Point p = l.front();
					for(auto q : l) {
						if(p == q) continue;

						addSegment(p,q);

						p = q;
					}
					addSegment(i.intersectionPoint,i.intersectionPoint);

					addSegment(p,i.intersectionPoint);
					addSegment(i.intersectionPoint,l.front());

//					addSegment(i.intersectionPoint,i.a->start);
//					addSegment(i.intersectionPoint,i.b->start);

				}
			}
		}
	}
//	switch(e.type) {
//	case EventType::EDGE:
//		addSegment(e,e.a->start);
//		addSegment(e,e.b->start);
//		break;
//	case EventType::SPLIT:  cout << "TODO" << endl; break;
//	case EventType::DIVIDE: cout << "TODO" << endl; break;
//	case EventType::CREATE: cout << "TODO" << endl; break;
//
//	default: break;
//	}
}

void GUI::drawPolygon() {
	for(auto e = data->polygon.edges_begin(); e != data->polygon.edges_end(); ++e) {
		addSegment(e);
	}
}

void GUI::init() {
	// Restore previous viewer state.
	restoreStateFromFile();

	// Define 'Control+Q' as the new exit shortcut (default was 'Escape')
	//setShortcut(EXIT_VIEWER, Qt::CTRL+Qt::Key_Q);

	// Add custom key description (see keyPressEvent).
	setKeyDescription(Qt::Key_W, "Toggles wire frame display");
	setKeyDescription(Qt::Key_F, "Toggles flat shading display");
	setKeyDescription(Qt::Key_E, "Toggles edges display");
	setKeyDescription(Qt::Key_V, "Toggles vertices display");

	// Light default parameters
	::glLineWidth(2.4f);
	::glPointSize(7.f);
	//    ::glEnable(GL_POLYGON_OFFSET_FILL);
	//    ::glPolygonOffset(1.f,1.f);
	::glClearColor(1.0f,1.0f,1.0f,0.0f);
	::glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	::glEnable(GL_LIGHTING);

	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	// ::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	if (flatShading)
	{
		::glShadeModel(GL_FLAT);
		::glDisable(GL_BLEND);
		::glDisable(GL_LINE_SMOOTH);
		::glDisable(GL_POLYGON_SMOOTH_HINT);
		::glBlendFunc(GL_ONE, GL_ZERO);
		::glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	}
	else if (wireframe) {
		::glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		::glShadeModel(GL_SMOOTH);
		::glEnable(GL_BLEND);
		::glEnable(GL_LINE_SMOOTH);
		::glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	CGAL::Bbox_3 bb = bbox(lcc);

	this->camera()->setSceneBoundingBox(qglviewer::Vec(bb.xmin(),
			bb.ymin(),
			bb.zmin()),
			qglviewer::Vec(bb.xmax(),
					bb.ymax(),
					bb.zmax()));

	this->showEntireScene();
}

QString GUI::helpString() const	{
	QString text("<h2>L C C   V i e w e r</h2>");
	text += "Use the mouse to move the camera around the object. ";
	text += "You can respectively revolve around, zoom and translate with "
			"the three mouse buttons. ";
	text += "Left and middle buttons pressed together rotate around the "
			"camera view direction axis<br><br>";
	text += "Pressing <b>Alt</b> and one of the function keys "
			"(<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
	text += "Simply press the function key again to restore it. "
			"Several keyFrames define a ";
	text += "camera path. Paths are saved when you quit the application "
			"and restored at next start.<br><br>";
	text += "Press <b>F</b> to display the frame rate, <b>A</b> for the "
			"world axis, ";
	text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> "
			"to save a snapshot. ";
	text += "See the <b>Keyboard</b> tab in this window for a complete "
			"shortcut list.<br><br>";
	text += "Double clicks automates single click actions: A left button "
			"double click aligns the closer axis with the camera (if close enough). ";
	text += "A middle button double click fits the zoom of the camera and "
			"the right button re-centers the scene.<br><br>";
	text += "A left button double click while holding right button pressed "
			"defines the camera <i>Revolve Around Point</i>. ";
	text += "See the <b>Mouse</b> tab and the documentation web pages for "
			"details.<br><br>";
	text += "Press <b>Escape</b> to exit the viewer.";
	return text;
}

#endif /* QTGUI */

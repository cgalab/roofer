/*
 * GUI.h
 *
 *  Created on: May 28, 2015
 *      Author: gue
 */

#ifndef GUI_H_
#define GUI_H_

/* If you want to use a viewer, you can use one of the following file
 * depending if you use vtk or qglviewer. */
#ifdef CGAL_LCC_USE_QT
#include "viewer_qt.h"
#else
#ifdef CGAL_LCC_USE_VTK
#include "viewer_vtk.h"
#endif
#endif

#include "Data.h"


class GUI {
public:
	GUI();
	virtual ~GUI();

	void show(SS3D::LCC_3& lcc);
};

#endif /* GUI_H_ */

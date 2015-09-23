/*
 * GUI.cpp
 *
 *  Created on: May 28, 2015
 *      Author: gue
 */

#include "GUI.h"

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>

GUI::GUI() {}

GUI::~GUI() {}

void GUI::show(SS3D::LCC_3& lcc) {
	display_lcc(lcc);
}


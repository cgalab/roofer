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

#ifndef SKELETON_H_
#define SKELETON_H_

#include "Data.h"


class Skeleton {
public:
	Skeleton(std::list<std::string>& args);
	virtual ~Skeleton();

	bool guiEnabled() {return data.config.gui;}

	void handleNextEvent(SweepEvent& event);

	Data data;

private:
	void createSkeleton();

	void createLineArrangements();
	void addAllBaseCells();
	void startPlaneSweep();

};

#endif /* SKELETON_H_ */

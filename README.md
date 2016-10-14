# Min/Max Roofs

roofer computes the minimum-volume or maximum-volume roof over a given polygon.

# Algorithm

The algorithm is based on scientific work that is currently done by my group.
As soon as the paper is published, it will be referenced here. 

# Input/Output

Reads wavefront .obj format that should describe a simple polygon. Writes the
same format, optional with 3D coordinates which can be imported to programs like
blender.

# ToDo's
- many TODO's with the essential problem still in the code
- GUI is solved very poorly 

## Optional ToDo's
- libraray funciton

# Requirements 
- C++11 enabled complier (gcc,clang)
- CGAL

# License
roofer is written in C++ and uses CGAL.  It computes straight skeleton roofs
as well as minimum- and maximum-volume roofs over a simple polygon.
Copyright (C) 2016 - Günther Eder - roofer@geder.at

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


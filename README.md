# roofer

roofer computes the minimum-volume or maximum-volume roof over a given simple polygon.

# Algorithm

The algorithm is based on scientific work that is currently done by my research group.
As soon as the paper that goes along with this program is published, it will be referenced here. 

# Input/Output

Reads wavefront .obj format which should describe a simple polygon. Writes the
same format, optional with 3D coordinates which can be imported to programs like
blender.

# ToDo's
- some TODO's left in the code, the problems are stated along the TODO-comments
- GUI solution has to be rewritten

## Optional ToDo's
- library function

# Requirements 
- C++11 enabled complier (gcc,clang)
- CGAL
- Linux or Mac OS
- GUI only under Linux for now

# Installation

- git clone --recursive https://github.com/guenthereder/roofer
- mkdir -p roofer/build
- cd /roofer/build
- cmake ..
- make -j 

# Usage

roofer [-h] [-v] [-s] [-min|-max] [-poly|-obj|-obj3d <filename>] <filename>

|  -h           |         print this help |
|  -v           |         verbose mode, shows more information about the computation |
|  -s           |         silent mode, shows no information |
|  -l           |         logging verbose output to <filename>.log |
|  -min / -max |          minimize or maximize the resulting roof |
|  -obj      |            write output in wavefront obj format (2D coordinates) |
|  -obj3d   |             write output in wavefront obj format (3D coordinates) |
|  <filename> |           input type is wavefront obj format |

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


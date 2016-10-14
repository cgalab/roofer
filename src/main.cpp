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


#include "Skeleton.h"

#ifdef QTGUI
#include "GUI.h"
#endif

INITIALIZE_EASYLOGGINGPP

using namespace std;

int main(int argc, char** argv) {

	el::Configurations defaultConf;
	defaultConf.setToDefault();
	//defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime{H:%m:%s.%g} %level %func: %msg");
	defaultConf.setGlobally(el::ConfigurationType::Format, "%msg");
	el::Loggers::addFlag(el::LoggingFlag::HierarchicalLogging);
	el::Loggers::setLoggingLevel(el::Level::Global);
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
	el::Loggers::reconfigureAllLoggers(defaultConf);
    el::Loggers::reconfigureLogger("default", defaultConf);

	START_EASYLOGGINGPP(argc, argv);

	static list<string> args;
	for(auto i = 1; i < argc; ++i) { args.push_back(string(argv[i])); }

	/* create skeleton or if gui is enabled only initial event-queue */
	Skeleton skeleton(args);

	/* display if needed */
#ifdef QTGUI
	if(skeleton.guiEnabled()) {
		int argc=1;
		const char* argv[2]={"lccviewer","\0"};
		QApplication app(argc,const_cast<char**>(argv));
		GUI gui(&skeleton);
		app.exec();

//		gui.make_facet(data.polygon);
//		gui.show(data.iss);
	}
#endif // QTGUI

	return EXIT_SUCCESS;
}

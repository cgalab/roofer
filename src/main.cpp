
#include "Skeleton.h"

#ifdef QTGUI
#include "GUI.h"
#endif

INITIALIZE_EASYLOGGINGPP

using namespace std;

int main(int argc, char** argv) {
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	//defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level %func(): %msg");
	//defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime{H:%m:%s.%g} %level %func: %msg");
	defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime{H:%m:%s.%g} %level %func: %msg");
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

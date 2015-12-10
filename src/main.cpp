
#include "Skeleton.h"

#ifdef QTGUI
#include "GUI.h"
#endif

using namespace std;

int main(int argc, char** argv) {
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

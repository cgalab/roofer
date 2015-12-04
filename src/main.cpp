
#include "Skeleton.h"

using namespace std;

int main(int argc, char** argv) {
	static list<string> args;
	for(auto i = 1; i < argc; ++i) { args.push_back(string(argv[i])); }

	Skeleton skeleton(args);

	return EXIT_SUCCESS;
}

#include <fstream>
#include <string>

#include "CMakeListsParser.h"

using namespace std;

int main(int argc, char* argv[])
{
	//getchar();
	string _prog = "icebw";
	string _version = "13_3";
	string _path_gr = "/home/vic/Projects/C++/iceb/";
	cbProject prj;
	prj.prjPath = _path_gr + _prog + "-" + _version + "/";
	prj.buildPath = "devel";

	_Parser parser (prj);

	parser._build(prj.prjPath, string(""));

	//***
	ofstream out {prj.prjPath + _prog + ".cbp"};
	///***
	prj.writeToStream (out);

    return 0;
}

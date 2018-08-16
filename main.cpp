#include <fstream>
#include <string>

#include "pathUtil.h"
#include "CMakeListsParser.h"

using namespace std;

int main(int argc, char* argv[])
{
	//getchar();
	string _prog = "icebw";
	string _version = "13_8";
	string _path_gr = _path_merge("C:", "msys2", "Projects", "iceb");
	cbProject prj;
	prj.prjPath = _path_merge(_path_gr, _prog + "-" + _version);
	prj.buildPath = "devel";

	_Parser parser (prj);

	parser._build(prj.prjPath, string(""));

	//***
	ofstream out {_path_merge(prj.prjPath, _prog + ".cbp")};
	///***
	prj.writeToStream (out);

    return 0;
}

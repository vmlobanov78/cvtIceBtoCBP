#include <sstream>
#include "pathUtil.h"


vector<string> _disunion (const string& value)
{

	vector<string> res;
	stringstream ss {value};
	string _val;

	while(!ss.eof())
	{
		ss >> _val;
		res.push_back(_val);
	}

	return res;
}



string _path_remove_curpath (const string& path)
{
	if (path.size()>1
		&& path[0]=='.'
		&& _IS_sep(path[1]))
		return string(path.begin()+2, path.end());
	return path;
}

string _path_get_filename (const string& path)
{
	if (path.empty())
		return path;
	string::const_iterator _begin = path.begin();
	string::const_iterator it = path.end()-1;

	while ( it != _begin && !_IS_sep(*it) )
	{
		--it;
	}
	return string (it+(_IS_sep(*it) ? 1 : 0 ), path.end());
}

string _path_merge (const string& path1, const string& path2)
{
	string res1 = _path_remove_curpath(path1);
	string res2 = _path_remove_curpath(path2);

	if(!res1.empty() && !res2.empty())
	{
		res1 += '/';
	}

	return res1+res2;
}



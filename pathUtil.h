#ifndef PATHUTIL_H_INCLUDED
#define PATHUTIL_H_INCLUDED

#include <string>
#include <vector>

using namespace std;

inline bool _IS_sep (char ch) { return ch == '/' || ch == '\\'; }

struct path
{
public:
	struct iterator;
public:

	path()=default;
	path(const path&)=default;
	path& operator=(const path&)=default;

	path(const char* _path) : _M_path(_path) {}
	path(const string& _path) : _M_path(_path) {}

	operator string() { return _M_path; }

private:
	string _M_path;


};

struct path::iterator
{
private:
	path& _M_base;
};

vector<string> _disunion (const string& value);

string _path_remove_curpath (const string& path);

string _path_get_filename (const string& path);

string _path_merge (const string& path1, const string& path2);


#endif // PATHUTIL_H_INCLUDED

#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include <deque>
#include "Vars.h"


struct Context
{
	Vars	vars;
	string	path;
	string	path_prj; // Absolute path to CMakeLists
};

struct stackContext
{
	deque<Context>	_list;
	Vars			_globals;

	void push();
	void push(const string& _path);
	void pop();

	Context& get_top ();

	size_t level();

	bool check (const string& name);
	vector<string> get (const string& name);
	void set (const string& name, const string& value);
	void set (const string& name, const vector<string>& value);
	void append (const string& name, const string& value);
	void append (const string& name, const vector<string>& value);
	void remove (const string& name);

};



#endif // CONTEXT_H_INCLUDED

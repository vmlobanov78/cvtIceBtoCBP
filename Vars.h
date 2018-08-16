#ifndef VARS_H_INCLUDED
#define VARS_H_INCLUDED

#include <string>
#include <vector>
#include <map>

using namespace std;

struct Vars
{
	using collType = map<string, vector<string>>;
	collType m_vars;

	void clear();
	bool empty() { return m_vars.empty(); }
	void set(const string& name, const string& value);
	void set(const string& name, const vector<string>& value);
	void append (const string& name, const string& value);
	void append (const string& name, const vector<string>& value);
	void remove (const string& name);
	bool check (const string& name);
	vector<string> get (const string& name);

};


#endif // VARS_H_INCLUDED

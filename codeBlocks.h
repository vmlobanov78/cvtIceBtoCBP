#ifndef CODEBLOCKS_H_INCLUDED
#define CODEBLOCKS_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <deque>
#include <map>

using namespace std;

struct toolOption
{
	enum Type {OPTIONS, DIRS, INCLUDES, LIBS, DEFS, FLAGS};

	Type	type;
	string	name;
	string	value;

	toolOption () = default;
	toolOption (const toolOption&) = default;
	toolOption (toolOption&&) = default;

	toolOption (Type _tp, const string& _value) : type(_tp), name(""), value(_value) {}
	toolOption (Type _tp, const string& _name, const string& _value) : type(_tp), name(_name), value(_value) {}

	const char* get_attr ();
	operator string () const;
};

struct toolOptions
{
private:
	deque<toolOption>		_options;
	map<string, size_t> 	_opt_idx;

public:

	deque<toolOption>& get_opt() { return _options; }

	void add_dir (const string& _dir);
	void add_lib (const string& _lib);
	void add_options (const string& _opts);

	void _insert_opt (const toolOption& _opt);
};

struct cbFile
{
	set<string> targets;
};

struct cbTarget
{
	enum Type { _EXEC, _CONSOLE, _STATIC, _SHARED };

	string			name;
	string			outFileName;
	string			workingDir;
	Type			type;
	toolOptions		optionsCompiler;
	toolOptions		optionsLinker;
};

struct cbProject
{
	string 			title;
	toolOptions		optionsCompiler;
	toolOptions		optionsLinker;
	//**
	string			prjPath;
	string			buildPath;
	//**
	deque<cbTarget>			targets;
	map<string, size_t>		_targets_idx;
	map<string, cbFile>		files;

	bool		has_target (const string& _name);
	bool		has_file (const string& _name);

	cbTarget&	add_target (const string& _name, const string& _path);
	cbFile&		add_file (const string& _name);

	void		writeToStream (ostream& os);

};



#endif // CODEBLOCKS_H_INCLUDED

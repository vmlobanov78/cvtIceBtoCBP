#ifndef CMAKELISTSPARSER_H_INCLUDED
#define CMAKELISTSPARSER_H_INCLUDED

#include <memory>
#include "codeBlocks.h"
#include "Context.h"

struct _Parser;

using Params = deque<string>;
//using _FUNC_EXEC = int(*)(_Parser&, Params&);

struct _FUNC_EXEC
{
	_FUNC_EXEC() {}
	virtual ~_FUNC_EXEC() {}
	virtual int Exec(_Parser&, Params&) { return 0; }
};

using _FuncMap = map<string, shared_ptr<_FUNC_EXEC>>;

struct _Parser
{
	cbProject&	prj;
	stackContext _stack;

	_FuncMap	_fmap;

	_Parser (cbProject& _prj);

	void _build (const string& _path_prj, const string& _path);
};



#endif // CMAKELISTSPARSER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>
#include "CMakeListsParser.h"
#include "pathUtil.h"


const char* _CMakeLists_txt = "CMakeLists.txt";

string _exec_cmd (const string& _cmd)
{
	char _tmp_name[L_tmpnam];
	//mkstemp();
	if (tmpnam(_tmp_name) )
	{
		string _cmd2 = _cmd + " >" + _tmp_name;
		if(system(_cmd2.c_str())==0)
		{
			FILE *_tmp_f = fopen(_tmp_name, "r");
			if(_tmp_f)
			{
				fseek(_tmp_f, 0, SEEK_END);
				auto _size = ftell(_tmp_f);
				string _str;
				_str.resize(_size);

				fseek(_tmp_f, 0, SEEK_SET);
				if (fread(_str.data(), _size, 1, _tmp_f));

				fclose(_tmp_f);

				cout << "		DBG " << _cmd << " -> " << _str << endl;

				return _str;
			}
		}
	}
	return string();
}

vector<string> _list_dir (const string& _dir, const string& _mask)
{
	vector<string> _list;
	//cout << "	dbg: _list_dir" << endl;

	char* _cd = getcwd(nullptr, 0);
	//cout << "	dbg: chdir " << _dir << endl;
	if(chdir(_dir.c_str()));

	char _tmp_name[L_tmpnam];
	//mkstemp();
	if (tmpnam(_tmp_name) )
	{
		//cout << "	dbg: " << _tmp_name << endl;
		string _cmd;
		_cmd += "dir ";
		_cmd += _mask;
		_cmd += " >";
		_cmd += _tmp_name;
		//cout << "	dbg: EXEC " << _cmd << endl;
		//_cmd += " || popd";
		if(system(_cmd.c_str())==0)
		{
			ifstream _tmp_f(_tmp_name);
			if(_tmp_f.is_open())
			{
				while(!_tmp_f.eof())
				{
					string _line;
					_tmp_f >> _line;
					if(_line.empty())
						continue;
					_list.push_back(_line);
					//cout << "		dbg: push_back " << _line << endl;
				}
			} // _tmp_f.is_open()
			else
			{
				//cout << "	err: BAD!! ifstream(" << _tmp_name << ") don't opened!" << endl;
			}
		}
		else
		{
			//cout << "	err: BAD!! system(" << _cmd << ")" << endl;
		}
	}

	if(chdir(_cd));
	free(_cd);

	return _list;
}

struct Streamer
{
private:
    istream &inp;
    char32_t _ch;

    size_t _line;
    size_t _col;

public:

    Streamer (istream& _inp) : inp (_inp), _ch(0), _line(0LL), _col (0LL)
    {
        _ch = next();
    }

public:

    bool		eof()
    {
        return inp.eof();
    }
    operator	bool ()
    {
        return !inp.eof();
    }

    void get_pos (size_t& __line, size_t& __col)
    {
        __line = _line;
        __col = _col;
    }

    char32_t	peek ()
    {
        return _ch;
    }
    char32_t	next ()
    {
        if (inp.eof())
            return _ch = 0;
        _ch = inp.get();
        if(_ch != '\r')
            ++_col;

        if(_ch == '\n')
        {
            ++_line;
            _col = 0;
        }
        return _ch;
    }

};

struct CMakeLists_Lexer
{
public:
    enum typeToken {NONE, ID, STRING, COMMENT, LPAR, RPAR, DOLLAR, LCB, RCB, CMD, __END};

    struct ResultType
    {
        typeToken	tt;
        string		st;

        operator bool () const
        {
            return tt != typeToken::__END;
        }
    };

private:
    Streamer	_str;
    typeToken	_tt;
    string		_st;

private:
    bool _is_EOL (char32_t ch)
    {
        return ch == '\r' || ch == '\n';
    }
    bool _is_blank (char32_t ch)
    {
        return ch == ' ' || ch == '\t' || _is_EOL(ch);
    }
    bool _is_delim (char32_t ch)
    {
        switch(char(ch))
        {
        case '(':
        case ')':
        case '{':
        case '}':
        case ',':
        case '$':
            return true;
        }

        return false;
    }
    bool _is_AL (char32_t ch)
    {
        return (ch >= 'a' && ch <= 'z')
               || (ch >= 'A' && ch <= 'Z');
    }
    bool _is_AL_ (char32_t ch)
    {
        return _is_AL(ch) || ch == '_';
    }
    bool _is_NUM (char32_t ch)
    {
        return (ch >= '0' && ch <= '9');
    }
    bool _is_ALNUM_ (char32_t ch)
    {
        return _is_AL_(ch) || _is_NUM(ch);
    }

    bool _is_ID_cont (char32_t ch)
    {
        return ch != 0 && !_is_blank(ch) && !_is_delim(ch);
    }

public:

    CMakeLists_Lexer (istream& _inp) : _str (_inp), _tt(typeToken::NONE) {}

public:

    void skip_blank ()
    {
        char32_t ch = _str.peek();
        while(_str && _is_blank(ch))
            ch = _str.next();
    }
    void next_line ()
    {
        char32_t ch = _str.peek();
        while(_str && !_is_EOL(ch))
            ch = _str.next();
    }

    ResultType next ()
    {
        ResultType res { .tt = typeToken::NONE };

        skip_blank();

        while (_str && _str.peek() == '#')
        {
            next_line();
            skip_blank();
        }

        if( _str )
        {
            //***
            char32_t ch = _str.peek();

            if (ch == '"')
            {
                res.tt = typeToken::STRING;
                while ( (ch = _str.next()) != 0 )
                {

                    if(ch == '"')
                    {
                        _str.next();
                        break;
                    }
                    res.st += ch;
                }
            }
            else if (_is_ID_cont(ch))
            {
                res.tt = typeToken::ID;
                res.st += ch;

                while (_str.next() && _is_ID_cont(ch = _str.peek()))
                {
                    res.st += ch;
                }
            }
            else if (_is_delim(ch))
            {
                res.st += ch;
                switch(char(ch))
                {
                case '(':
                    res.tt = typeToken::LPAR;
                    break;
                case ')':
                    res.tt = typeToken::RPAR;
                    break;
                case '$':
                    res.tt = typeToken::DOLLAR;
                    break;
                case '{':
                    res.tt = typeToken::LCB;
                    break;
                case '}':
                    res.tt = typeToken::RCB;
                    break;
                default:
                    res.tt = typeToken::NONE;
                }
                _str.next();
            }
            else if(_str)
            {
                res.st += ch;
                _str.next();
            }
        }
        else
        {
            res.tt = typeToken::__END;
        }

        return res;
    }

};


ostream& operator<< (ostream& os, CMakeLists_Lexer::typeToken tt)
{

    switch (tt)
    {
    case CMakeLists_Lexer::typeToken::CMD:
        os << "CMD";
        break;
    case CMakeLists_Lexer::typeToken::COMMENT:
        os << "COMMENT";
        break;
    case CMakeLists_Lexer::typeToken::DOLLAR:
        os << "DOLLAR";
        break;
    case CMakeLists_Lexer::typeToken::ID:
        os << "ID";
        break;
    case CMakeLists_Lexer::typeToken::LCB:
        os << "LCB";
        break;
    case CMakeLists_Lexer::typeToken::LPAR:
        os << "LPAR";
        break;
    case CMakeLists_Lexer::typeToken::NONE:
        os << "NONE";
        break;
    case CMakeLists_Lexer::typeToken::RCB:
        os << "RCB";
        break;
    case CMakeLists_Lexer::typeToken::RPAR:
        os << "RPAR";
        break;
    case CMakeLists_Lexer::typeToken::STRING:
        os << "STRING";
        break;
    default:
        os << "<CMakeLists_Lexer::typeToken::!UNKNOWN!>";
    }
    return os;
}

ostream& operator<< (ostream& os, CMakeLists_Lexer::ResultType rt)
{

    os << '{'
       << rt.tt
       << ','  << ' '
       << '\''
       << rt.st
       << '\''
       << '}';
    return os;
}



int _PROJECT (_Parser& Self, Params& _params)
{
	if (_params.size()>0 && Self.prj.title.empty())
		Self.prj.title = _params[0];
	return 0;
}

int _SET (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
        string var_name = _params.front();
        _params.pop_front();
        Self._stack.set(var_name, vector<string>(_params.begin(), _params.end()));
    }
    return 0;
}

int _ADD_EXECUTABLE (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
        string _target_name = _params.front();
        _params.pop_front();
        //_stack.set(var_name, _params);

        Context &ctxt = Self._stack.get_top();

        cbTarget& _target = Self.prj.add_target(_target_name, Self.prj.buildPath);
        _target.type = cbTarget::Type::_EXEC;

        //***
        for (string& _file : _params)
        {
            cbFile& _cbFile = Self.prj.add_file(_path_merge(ctxt.path, _file));

            _cbFile.targets.insert(_target_name);
        }
    }
    return 0;
}

int _ADD_LIBRARY (_Parser& Self, Params& _params)
{
    if (_params.size()>2)
    {
        string _target_name = _params.front();
        _params.pop_front();
        string _lib_type 	= _params.front();
        _params.pop_front();

        Context &ctxt = Self._stack.get_top();

        string _buildPath = Self.prj.buildPath;
        cbTarget::Type _target_type = cbTarget::Type::_SHARED;

        if( _lib_type == "STATIC" )
		{
			_target_type = cbTarget::Type::_STATIC;
			_buildPath = "";
		}

        //_stack.set(var_name, _params);
        cbTarget& _target = Self.prj.add_target(_target_name, _buildPath);
		_target.type = _target_type;

        //***
        for (string& _file : _params)
        {
            cbFile& _cbFile = Self.prj.add_file(_path_merge(ctxt.path, _file));

            _cbFile.targets.insert(_target_name);
        }
    }
	return 0;
}

int _TARGET_LINK_LIBRARY (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
        string _target_name = _params.front();
        _params.pop_front();

        cbTarget& _target = Self.prj.add_target(_target_name, Self.prj.buildPath);

        for(auto it : _params)
        {
            _target.optionsLinker.add_lib(it);
        }

    }
    return 0;
}

int _INCLUDE_DIRECTORIES (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
        for(auto it : _params)
        {
            Self.prj.optionsCompiler.add_dir(it);
        }
    }
    return 0;
}

int _LINK_DIRECTORIES (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
        for(auto it : _params)
        {
            Self.prj.optionsLinker.add_dir(it);
        }
    }
    return 0;
}

int _ADD_DEFINITIONS (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
        for(auto it : _params)
        {
            Self.prj.optionsCompiler.add_options(it);
        }
    }
    return 0;
}

int _ADD_SUBDIRECTORY (_Parser& Self, Params& _params)
{
    if (_params.size()>0)
    {
    	cout << "dbg: PROCESS dir ='" << _params[0] << "'" << endl;
    	Context& ctxt = Self._stack.get_top();

        Self._build(_path_merge(ctxt.path_prj, _params[0]), _path_merge(ctxt.path, _params[0]));
    }
    return 0;
}

int _FILE (_Parser& Self, Params& _params)
{
	string _cmd = _params.front();
	_params.pop_front();
	Context& ctxt = Self._stack.get_top();

	if( _cmd == "GLOB" )
	{
		string _var_name = _params.front();
		_params.pop_front();

		cout << "dbg: cmd " << _cmd << " Var=" << _var_name << " params: " << _params[0] << endl;
		set<string> _files;

		for(string& _p : _params)
		{
			auto _list = _list_dir(ctxt.path_prj, _p);
			_files.insert(_list.begin(), _list.end());
		}
		ctxt.vars.set(_var_name, vector<string>(_files.begin(), _files.end()));
	}

	return 0;
}

using _FUNC_PTR = int (*)(_Parser&, Params&);

struct _SYS_FUNC : public _FUNC_EXEC
{
	_FUNC_PTR ptr;

	_SYS_FUNC (_FUNC_PTR _ptr) : ptr(_ptr) {}
	~_SYS_FUNC () {}

	int Exec(_Parser& _parser, Params& _params) override
	{
		if ( ptr )
			return ptr (_parser, _params);
		return 0;
	}
};

int _ (_Parser& Self, Params& _params)
{
	return 0;
}


_Parser::_Parser (cbProject& _prj)
	: prj (_prj)
{
	_fmap.insert(make_pair(string("project"),				make_shared<_SYS_FUNC>(&_PROJECT) ));
	_fmap.insert(make_pair(string("set"),					make_shared<_SYS_FUNC>(&_SET) ));
	_fmap.insert(make_pair(string("add_executable"),		make_shared<_SYS_FUNC>(&_ADD_EXECUTABLE) ));
	_fmap.insert(make_pair(string("add_library"),			make_shared<_SYS_FUNC>(&_ADD_LIBRARY) ));
	_fmap.insert(make_pair(string("add_definitions"),		make_shared<_SYS_FUNC>(&_ADD_DEFINITIONS) ));
	_fmap.insert(make_pair(string("add_subdirectory"),		make_shared<_SYS_FUNC>(&_ADD_SUBDIRECTORY) ));
	_fmap.insert(make_pair(string("target_link_libraries"),	make_shared<_SYS_FUNC>(&_TARGET_LINK_LIBRARY) ));
	_fmap.insert(make_pair(string("include_directories"),	make_shared<_SYS_FUNC>(&_INCLUDE_DIRECTORIES) ));
	_fmap.insert(make_pair(string("link_directories"),		make_shared<_SYS_FUNC>(&_LINK_DIRECTORIES) ));
	_fmap.insert(make_pair(string("file"),					make_shared<_SYS_FUNC>(&_FILE) ));
	//_fmap.insert(make_pair(string(""), &));
	//_fmap.insert(make_pair(string(""), &));

}


void _Parser::_build (const string& _path_prj, const string& _path)
{
	if (_stack.level()==0)
	{
		_stack._globals.set("GTK3_INCLUDE_DIRS", _exec_cmd("pkg-config --cflags-only-I gtk+-3.0"));
		_stack._globals.set("GTK3_LIBRARY_DIRS", _exec_cmd("pkg-config --libs-only-L gtk+-3.0"));
		_stack._globals.set("GTK3_LIBRARIES", 	_exec_cmd("pkg-config --libs-only-l gtk+-3.0"));
		_stack._globals.set("GTK3_CFLAGS_OTHER", _exec_cmd("pkg-config --cflags-only-other gtk+-3.0"));
	}

	string pathCMakeLists = _path_merge(_path_prj, _CMakeLists_txt);

	cout << "dbg: open CMakeLists.txt (" << pathCMakeLists << ")" << endl;

	ifstream _cmake (pathCMakeLists);

	if (!_cmake.is_open())
		return;

	CMakeLists_Lexer lexer (_cmake);

    _stack.push(_path);  /// new Context

    Context& ctxt = _stack.get_top();
    ctxt.path_prj = _path_prj;

    cout << "dbg: Path='" << _path << "' Path(prj)='" << _path_prj <<"'" << endl;

    string			_cmd;
    Params			_params;
    deque<int>		_stack_lvl;
    int lvl = 0;

    while (auto it = lexer.next())
    {
        switch (lvl)
        {
        case 0:
            if (it.tt == CMakeLists_Lexer::typeToken::ID)
            {
                //_stack.push_back(it);
                _cmd = it.st;
                lvl = 1;
            }
            else
            {
                cerr << "err: " << it << endl;
            }
            break;

        case 1:
            if (it.tt == CMakeLists_Lexer::typeToken::LPAR)
            {
                //_stack.push_back(it);
                lvl = 2;
            }
            else
            {
                cerr << "err: " << it << endl;
            }
            break;
        case 2:
            if (it.tt == CMakeLists_Lexer::typeToken::RPAR)
            {
            	//cout << "dbg: EXECUTE " << _cmd << endl;
            	//cout << "		params [";
            	//for(string& _str : _params)
				//	cout << _str << ' ';
            	//cout << "]" << endl;
            	auto itf = _fmap.find(_cmd);
            	if (itf != _fmap.end())
				{
					itf->second->Exec(*this, _params);
				}
                else
				{
					cerr << "err: Command '" << _cmd << "' do'nt implemeted" << endl;
				}

                _params.clear();
                lvl = 0;
            }
            else if (it.tt == CMakeLists_Lexer::typeToken::ID)
            {
                _params.push_back(it.st);
            }
            else if (it.tt == CMakeLists_Lexer::typeToken::STRING)
            {
                _params.push_back(it.st);
            }
            else if (it.tt == CMakeLists_Lexer::typeToken::DOLLAR)
            {
                _stack_lvl.push_back(lvl);
                lvl = 3;
            }
            else
            {
                cerr << "err: " << it << endl;
            }
            break;
        case 3:
            if (it.tt == CMakeLists_Lexer::typeToken::LCB)
            {
                lvl = 4;
            }
            else
            {
                cerr << "err: '{' got " << it << endl;
                lvl = 0;
                _stack_lvl.pop_back();
            }
            break;
        case 4:
            if (it.tt == CMakeLists_Lexer::typeToken::ID)
            {
				cout << "dbg: Get Value " << it.st << endl;
                if (!_stack.check(it.st))
                    cerr << "warn: var '"<< it.st << "' - not defined!" << endl;

                vector<string> val = _stack.get(it.st);
                cout << "	";
                //for(auto _str : val)
				//	cout << _str << ' ';
				cout << endl;
                _params.insert(_params.end(), val.begin(), val.end());
                lvl = 5;
            }
            else
            {
                cerr << "err: ID got " << it << endl;
                lvl = 0;
                _stack_lvl.pop_back();
            }
            break;
        case 5:
            if (it.tt == CMakeLists_Lexer::typeToken::RCB)
            {
                lvl = _stack_lvl.back();
                _stack_lvl.pop_back();
            }
            else
            {
                cerr << "err: '}' got " << it << endl;
                lvl = 0;
                _stack_lvl.pop_back();
            }
            break;
        case 6:
            break;
        }
        ///cout << it << endl;
        ///
    }

	_stack.pop();

}



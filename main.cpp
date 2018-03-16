#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <unordered_map>

using namespace std;


struct xmlWriter {

public:
	enum state {NONE, ELEMENTSTART, CONTENT, TEXT, END};
private:
	ostream&		os;
	state			_state;
	vector<string>	elementsStack;

public:
	xmlWriter (ostream& _os) : os (_os), _state(state::NONE) {}
public:

	void writeXMLHeader () {

		if (_state != state::NONE)
			return;

		os << "<?xml";
		_state = state::ELEMENTSTART;
		writeAttribute ("version",		"1.0");
		writeAttribute ("encoding",		"UTF-8");
		writeAttribute ("standalone",	"yes");
		os << " ?>";
		_state = state::NONE;
	}

	void writeBeginElement (const string& name) {

		if (_state == state::ELEMENTSTART)
			os << '>' << endl << string(elementsStack.size(), '\t');
		else if (_state == state::CONTENT)
			os << endl << string(elementsStack.size(), '\t');
		else if (_state == state::NONE)
			os << endl;

		os << '<' << name;

		elementsStack.push_back(name);

		_state = state::ELEMENTSTART;
	}

	void writeEndElement () {

		size_t lvl = elementsStack.size();
		if (lvl == 0)
			return;
		--lvl;

		string elName = elementsStack[lvl];
		elementsStack.pop_back();

		if (_state == state::ELEMENTSTART)
		{
			os << " />";
		}
		else
		{
			os << endl;
			os << string(lvl, '\t');
			os << "</" << elName << ">";
		}
		if (lvl == 0)
			_state = state::END;
		else
			_state = state::CONTENT;
	}

	void writeAttribute (const string& name, const string& value) {

		if (_state != state::ELEMENTSTART)
			return;

		os << ' ' << name << '=' << '"' << value << '"';
	}

	void writeContent (const string _text) {

		if (_state == state::ELEMENTSTART)
		{
			os << '>';
			_state = state::TEXT;
		}

		if (_state == state::TEXT)
		{
			os << _text;
		}
	}

};

struct Streamer
{
private:
	istream &inp;
	char32_t _ch;

	size_t _line;
	size_t _col;

public:

	Streamer (istream& _inp) : inp (_inp), _ch(0), _line(0LL), _col (0LL) { _ch = next(); }

public:

	bool		eof()	{ return inp.eof(); }
	operator	bool ()	{ return !inp.eof(); }

	void get_pos (size_t& __line, size_t& __col) { __line = _line; __col = _col; }

	char32_t	peek ()	{ return _ch; }
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

		operator bool () const { return tt != typeToken::__END; }
	};

private:
	Streamer	_str;
	typeToken	_tt;
	string		_st;

private:
	bool _is_EOL (char32_t ch)		{ return ch == '\r' || ch == '\n'; }
	bool _is_blank (char32_t ch)	{ return ch == ' ' || ch == '\t' || _is_EOL(ch); }
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
	bool _is_ALNUM_ (char32_t ch) { return _is_AL_(ch) || _is_NUM(ch); }

	bool _is_ID_cont (char32_t ch) { return ch != 0 && !_is_blank(ch) && !_is_delim(ch); }

public:

	CMakeLists_Lexer (istream& _inp) : _str (_inp), _tt(typeToken::NONE) {}

public:

	void skip_blank ()	{ char32_t ch = _str.peek(); while(_str && _is_blank(ch)) ch = _str.next(); }
	void next_line ()	{ char32_t ch = _str.peek(); while(_str && !_is_EOL(ch)) ch = _str.next(); }

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

// enum typeToken {NONE, ID, STRING, COMMENT, LPAR, RPAR, CMD, __END};

ostream& operator<< (ostream& os, CMakeLists_Lexer::typeToken tt) {

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

ostream& operator<< (ostream& os, CMakeLists_Lexer::ResultType rt) {

	os << '{'
		<< rt.tt
		<< ','  << ' '
		<< '\''
		<< rt.st
		<< '\''
		<< '}';
	return os;
}


struct CMakeLists_Parser
{
private:
	istream& inp;
	char32_t _ch;

public:
	CMakeLists_Parser (istream& _inp) : inp (_inp)  {}

public:

	bool	 eof() 		{ return inp.eof(); }
	char32_t peek ()	{ return _ch; }
	char32_t next ()	{

		_ch = inp.get();

		return _ch;

	}

};


const char* _CMakeLists_txt = "CMakeLists.txt";

struct cbFile
{
	set<string> targets;
};

struct cbTarget
{
	string			outFileName;
	set<string> 	optionsCompiler;
	set<string> 	optionsLinker;
};

struct cbProject
{
	string 							title;
	set<string>						optionsCompiler;
	set<string>						optionsLinker;
	unordered_map<string, cbTarget>	targets;
	unordered_map<string, cbFile>	files;

	void writeOption (xmlWriter& w, const string& attrName, const string& value) {

		w.writeBeginElement("Option");
		w.writeAttribute(attrName, value);
		w.writeEndElement(); //  Option
	}

	void writeToStream (ostream& os) {

		xmlWriter w{os};

		w.writeXMLHeader();
		w.writeBeginElement("CodeBlocks_project_file");

		w.writeBeginElement("FileVersion");
		w.writeAttribute("major", "1");
		w.writeAttribute("minor", "6");
		w.writeEndElement(); // FileVersion

		w.writeBeginElement("Project");

		writeOption (w, "title", title);
		writeOption (w, "pch_mode", "2");
		writeOption (w, "compiler", "gcc");

		w.writeBeginElement("Build");

		for (auto itTarget : targets) {

			w.writeBeginElement("Target");
			w.writeAttribute("title", itTarget.first);

			w.writeBeginElement("Compiler");
			for (auto it : itTarget.second.optionsCompiler) {

				w.writeBeginElement("Add");
				w.writeAttribute("option", it);
				w.writeEndElement(); //
			}
			w.writeEndElement(); // Compiler

			w.writeBeginElement("Linker");
			for (auto it : itTarget.second.optionsLinker) {

				w.writeBeginElement("Add");
				w.writeAttribute("option", it);
				w.writeEndElement(); //
			}
			w.writeEndElement(); // Linker

			w.writeEndElement(); // Target
		}

		w.writeEndElement(); // Build

		w.writeBeginElement("Compiler");
		for (auto it : optionsCompiler) {

				w.writeBeginElement("Add");
				w.writeAttribute("option", it);
				w.writeEndElement(); //
		}
		w.writeEndElement(); // Compiler

		w.writeBeginElement("Linker");
		for (auto it : optionsLinker) {

			w.writeBeginElement("Add");
			w.writeAttribute("option", it);
			w.writeEndElement(); //
		}
		w.writeEndElement(); // Linker

		for (auto itFile : files) {

			w.writeBeginElement("Unit");
			w.writeAttribute("filename", itFile.first);

			for (auto _target : itFile.second.targets) {

				w.writeBeginElement("Option");
				w.writeAttribute("target", _target);
				w.writeEndElement(); // Option
			}

			w.writeEndElement(); // Unit
		}


		w.writeBeginElement("Extensions");
		w.writeBeginElement("code_completion");
		w.writeEndElement(); // code_completion
		w.writeBeginElement("envvars");
		w.writeEndElement(); // envvars
		w.writeBeginElement("debugger");
		w.writeEndElement(); // debugger
		w.writeEndElement(); // Extensions

		w.writeEndElement(); // Project

		w.writeEndElement(); // CodeBlocks_project_file
		os.flush();

	}
};

int main(int argc, char** argv)
{
	string path = "C:\\msys64\\Projects\\iceb\\iceBw-13_3\\";

	ifstream _cmake {path + _CMakeLists_txt};

	CMakeLists_Lexer _make_lex (_cmake);

	while (auto it = _make_lex.next())
	{
		cout << it << endl;
	}

	//***
	cbProject prj;
	ofstream out {"project.cbp"};

	///***
	prj.writeToStream (out);

    return 0;
}

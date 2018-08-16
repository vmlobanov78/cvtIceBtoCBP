#include "xmlWriter.h"

void xmlWriter::writeXMLHeader ()
{

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

void xmlWriter::writeBeginElement (const string& name)
{

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

void xmlWriter::writeEndElement ()
{

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

void xmlWriter::writeAttribute (const string& name, const string& value)
{

    if (_state != state::ELEMENTSTART)
        return;

    os << ' ' << name << '=' << '"' << value << '"';
}

void xmlWriter::writeContent (const string _text)
{

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



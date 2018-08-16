#ifndef XMLWRITER_H_INCLUDED
#define XMLWRITER_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>

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

	void writeXMLHeader ();

	void writeBeginElement (const string& name);

	void writeEndElement ();

	void writeAttribute (const string& name, const string& value);

	void writeContent (const string _text);

};



#endif // XMLWRITER_H_INCLUDED

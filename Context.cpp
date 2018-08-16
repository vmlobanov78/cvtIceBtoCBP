#include "Context.h"


void stackContext::push()
{
    _list.push_front(Context());
}
void stackContext::push(const string& _path)
{
	Context ctxt;
	ctxt.path = _path;
	_list.push_front(move(ctxt));
}
void stackContext::pop()
{
    if(_list.size()>0)
        _list.pop_front();
}

Context& stackContext::get_top ()
{
	return _list.front();
}

size_t stackContext::level()
{
    return _list.size();
}

bool stackContext::check (const string& name)
{

    for (Context& context : _list)
    {

        if (context.vars.check(name))
            return true;
    }
    if (_globals.check(name))
		return true;
    return false;
}

vector<string> stackContext::get (const string& name)
{

    for (Context& context : _list)
    {

        if (context.vars.check(name))
            return context.vars.get(name);
    }
    if (_globals.check(name))
		return _globals.get(name);
    return vector<string>();
}

void stackContext::set (const string& name, const string& value)
{

    if (level()==0)
        return;
    Context& context = _list.front();
    context.vars.set(name, value);
}

void stackContext::set (const string& name, const vector<string>& value)
{

    if (level()==0)
        return;
    Context& context = _list.front();
    context.vars.set(name, value);
}

void stackContext::append (const string& name, const string& value)
{

    if (level()==0)
        return;
    Context& context = _list.front();
    context.vars.append(name, value);
}
void stackContext::append (const string& name, const vector<string>& value)
{

    if (level()==0)
        return;
    Context& context = _list.front();
    context.vars.append(name, value);
}

void stackContext::remove (const string& name)
{

    if (level()==0)
        return;
    Context& context = _list.front();
    context.vars.remove(name);
}



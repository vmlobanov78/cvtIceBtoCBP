#include "Vars.h"
#include "pathUtil.h"

void Vars::clear()
{
    m_vars.clear();
}

void Vars::set(const string& name, const string& value)
{

    m_vars[name] = _disunion(value);
}

void Vars::set(const string& name, const vector<string>& value)
{

    m_vars[name] = value;
}

void Vars::append (const string& name, const string& value)
{

    if (m_vars.count(name) == 0)
    {
        set (name, value);
        return;
    }

    auto _iter = m_vars.find(name);

    for (auto it : _disunion(value))
        _iter->second.push_back(it);
}

void Vars::append (const string& name, const vector<string>& value)
{

    if (!check(name))
    {
        set (name, value);
        return;
    }

    auto _iter = m_vars.find(name);

    _iter->second.insert(_iter->second.end(), value.begin(), value.end());
}

void Vars::remove (const string& name)
{

    m_vars.erase(name);
}

bool Vars::check (const string& name)
{

    return m_vars.count(name)>0;
}
vector<string> Vars::get (const string& name)
{

    vector<string> res;

    if(check(name))
    {
        auto _strings = m_vars.find(name)->second;
        res.assign(_strings.begin(), _strings.end());
    }

    return res;
}




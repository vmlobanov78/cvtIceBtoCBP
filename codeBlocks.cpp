#include <iostream>
#include "codeBlocks.h"
#include "xmlWriter.h"
#include "pathUtil.h"

const char* toolOption::get_attr ()
{

    switch (type)
    {
    case Type::DIRS:
        return "directory";
    case Type::LIBS:
        return "library";
        break;
    default:
        ;
    }
    return "option";
}

toolOption::operator string () const
{

    string _value;

    switch (type)
    {
    default:
        _value = value;
    }

    return _value;
}


bool _is_short_opt (const string& _opts)
{
	if (_opts.size()>1)
	{
		return _opts[0] == '-' && _opts[1] != '-';
	}
	return false;
}

bool _is_short_opt (const string& _opts, char _opt)
{
	if (_opts.size()>1)
	{
		return _opts[0] == '-' && _opts[1] == _opt;
	}
	return false;
}

void toolOptions::add_dir (const string& _dir)
{
	if (_is_short_opt(_dir))
	{
		add_options(_dir);
		return;
	}

    _insert_opt(toolOption(toolOption::Type::DIRS, _dir));
}
void toolOptions::add_lib (const string& _lib)
{
	if (_is_short_opt(_lib))
	{
		add_options(_lib);
		return;
	}

    _insert_opt(toolOption(toolOption::Type::LIBS, _lib));
}
void toolOptions::add_options (const string& _opts)
{
	if( _is_short_opt(_opts))
	{
		if(_is_short_opt(_opts, 'I'))
		{
			_insert_opt(toolOption(toolOption::Type::DIRS, _opts.substr(2)));
			return;
		}
		else if(_is_short_opt(_opts, 'L'))
		{
			_insert_opt(toolOption(toolOption::Type::DIRS, _opts.substr(2)));
			return;
		}
		else if(_is_short_opt(_opts, 'l'))
		{
			_insert_opt(toolOption(toolOption::Type::LIBS, _opts.substr(2)));
			return;
		}
	}

    _insert_opt(toolOption(toolOption::Type::OPTIONS, _opts));
}

void toolOptions::_insert_opt (const toolOption& _opt)
{
	string key = string(_opt);
	if (_opt_idx.count(key)==0)
	{
		_opt_idx.insert(make_pair(key, _options.size()));
		_options.push_back(_opt);
	}
}

bool cbProject::has_target (const string& _name)
{
	return _targets_idx.count(_name)>0;
}

bool cbProject::has_file (const string& _name)
{
	return files.count(_name)>0;
}


cbTarget& cbProject::add_target (const string& _name, const string& _path)
{
    auto it = _targets_idx.find(_name);

    if (it != _targets_idx.end())
	{
		return targets[it->second];
	}

	_targets_idx.insert(make_pair(_name, targets.size()));
	//**
	targets.push_back(cbTarget());
    cbTarget& _target = targets.back();

    _target.name = _name;
    _target.outFileName = _path_merge(_path, _name);

    return _target;
}

cbFile& cbProject::add_file (const string& _name)
{
    cbFile& _file = files[_name];
    return _file;
}

void writeOption (xmlWriter& w, const string& attrName, const string& value)
{

    w.writeBeginElement("Option");
    w.writeAttribute(attrName, value);
    w.writeEndElement(); //  Option
}


void cbProject::writeToStream (ostream& os)
{

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

    for (cbTarget& _target : targets)
    {

        w.writeBeginElement("Target");
        w.writeAttribute("title", _target.name);

        w.writeBeginElement("Option");
        w.writeAttribute("output", _target.outFileName);
        w.writeAttribute("prefix_auto", "1");
        w.writeAttribute("extension_auto", "1");
        w.writeEndElement(); // Option

        w.writeBeginElement("Option");
        w.writeAttribute("type", to_string(int(_target.type))); // Program
        w.writeEndElement(); // Option


        w.writeBeginElement("Compiler");
        for (auto it : _target.optionsCompiler.get_opt())
        {

            w.writeBeginElement("Add");
            w.writeAttribute(it.get_attr(), it);
            w.writeEndElement(); //
        }
        w.writeEndElement(); // Compiler

        w.writeBeginElement("Linker");
        for (auto it : _target.optionsLinker.get_opt())
        {

            w.writeBeginElement("Add");
            w.writeAttribute(it.get_attr(), it);
            w.writeEndElement(); //
        }
        w.writeEndElement(); // Linker

        w.writeEndElement(); // Target
    }

    w.writeEndElement(); // Build

    w.writeBeginElement("Compiler");
    for (auto it : optionsCompiler.get_opt())
    {

        w.writeBeginElement("Add");
        w.writeAttribute(it.get_attr(), it);
        w.writeEndElement(); //
    }
    w.writeEndElement(); // Compiler

    w.writeBeginElement("Linker");
    for (auto it : optionsLinker.get_opt())
    {

        w.writeBeginElement("Add");
        w.writeAttribute(it.get_attr(), it);
        w.writeEndElement(); //
    }
    w.writeEndElement(); // Linker

    for (auto itFile : files)
    {

        w.writeBeginElement("Unit");
        w.writeAttribute("filename", itFile.first);

        for (auto _target : itFile.second.targets)
        {

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


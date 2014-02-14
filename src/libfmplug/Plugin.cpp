/*
libfmplug, a C++ library to create FileMaker Pro (TM) plugins.
Copyright (C) 2014 University of Lausanne, Switzerland
Author: Thibault Genessay
https://github.com/tibogens/libfmplug

This file is part of libfmplug.

libfmplug is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libfmplug is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libfmplug.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "pch.h"
#include <sstream>
#include <vector>
#include <assert.h>
#include <fmplug/Setup.h>
#include <fmplug/Plugin.h>
#include <fmplug/UtfConv.h>
#include <fmplug/Utils.h>
#include <fmplug/Error.h>
#include <FMWrapper/FMXTypes.h>
#include <FMWrapper/FMXCalcEngine.h>
#include <FMWrapper/FMXText.h>
using namespace fmplug;

Plugin::Plugin(const std::string& id, const std::string& name, const std::string& help)
	: _id(id),
	_name(name),
	_help(help),
	_nextFuncId(0)
{
	if (id.size() != 4) 
		throw Error("Invalid plugin string ID: '%s'. ID strings must be exactly 4 characters long.", id.c_str());

}

Plugin::~Plugin()
{
	fmx::QuadCharAutoPtr pluginId(_id[0], _id[1], _id[2], _id[3]);
	for (FunctionsById::iterator it=_functionsById.begin();it!=_functionsById.end();++it)
	{
		fmx::errcode err = fmx::ExprEnv::UnRegisterExternalFunction(*pluginId, it->second->getId());
		delete it->second;
	}
	_functionsById.clear();
	_functionsByName.clear();
}

std::string Plugin::getOptions() const
{
	assert(_id.size() == 4);
	std::stringstream ss;
	ss << _id << "1" << (hasPreferences() ? "Y" : "n") << "nYnnn";
	return ss.str();
}

bool Plugin::init()
{
	try
	{
		if (sizeof(fmx::uint16) != sizeof(wchar_t))
			throw Error("Platform unicode incompatibility: sizeof(uint16) != sizeof(wchar_t)");
		declareFunctions();
		return true;
	}
	catch (Error& err)
	{
		BasicDialogBox("Unhandled exception while initializing '" + _name + "'. This plugin will be unavailable.\n\nError details:\n" + err.getReason(), "Plugin initialization error", DLGSTYLE_ERROR);
		return false;
	}
}

void Plugin::declareFunctions()
{
}


FMX_PROC(fmx::errcode) g_funcall(short functionId, const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result);

void Plugin::registerFunction(Function* function)
{
	if (_functionsByName.find(function->getName()) != _functionsByName.end())
		throw Error("Error while registering function '%s': cannot register another function with the same name", function->getName().c_str());

	fmx::QuadCharAutoPtr pluginId(_id[0], _id[1], _id[2], _id[3]);
	short int funcId(_nextFuncId++);

	fmx::TextAutoPtr fname;
	fname->Assign(function->getName().c_str());
	
	fmx::TextAutoPtr fproto;
	fproto->Assign(function->getPrototype().c_str());
	
	fmx::errcode err = fmx::ExprEnv::RegisterExternalFunction(*pluginId, funcId, *fname, *fproto, 
		function->getMinArgs(), function->getMaxArgs(), fmx::ExprEnv::kDisplayInAllDialogs, g_funcall);
	if (err != 0)
		throw Error("Error while registering function '%s': code %d", function->getName().c_str(), err);
	function->bind(funcId);

	_functionsByName[function->getName()] = function;
	_functionsById[function->getId()] = function;
}


FMX_ExternCallPtr gFMX_ExternCallPtr=NULL;
Plugin* g_plugin = NULL;

void Plugin::main(FMX_ExternCallPtr pb, PluginFactory* factory)
{
    gFMX_ExternCallPtr = pb;

	static FMStringCache stringCache;

	switch (pb->whichCall)
	{
	case kFMXT_Shutdown:
		{
			bool failed = false;
			if (g_plugin)
			{
				try
				{
					delete g_plugin;
				}
				catch (Error&)
				{
					failed = true;
				}
			}
			g_plugin = NULL;
			stringCache.clear();
			gFMX_ExternCallPtr = NULL;
			pb->result = failed ? 1 : 0;
			return;
		}
	}

	if (!g_plugin)
	{
		try
		{
			g_plugin = factory->createInstance();
		}
		catch (Error&)
		{
			// We can't just return an error code here because the return
			// value expected by FileMaker depends on the request type. Thus,
			// we silently eat the error here and let each case below handle
			// the plugin==NULL case.
		}
	}

	switch (pb->whichCall)
	{
	case kFMXT_GetString:
		{
			if (g_plugin == NULL)
			{
				// string of length zero
				pb->result = 0;
			}
			else
			{
				bool found;
				std::wstring wvalue = stringCache.get(pb->parm1, &found); 
				if (!found)
				{
					std::string value;
					switch (pb->parm1)
					{
					case kFMXT_NameStr: value = g_plugin->getName(); break;
					case kFMXT_AppConfigStr: value = g_plugin->getHelp(); break;
					case kFMXT_OptionsStr: value = g_plugin->getOptions(); break;
					}
					wvalue = convertUTF8toUTF16(convertStringFromCurrentCodePageToUTF8(value));
					stringCache.set(pb->parm1, wvalue);
				}
				FMX_Unichar* out = (FMX_Unichar*)pb->result;
				wcsncpy_s((wchar_t*)out, pb->parm3, wvalue.c_str(), wvalue.size());
			}
		}
		break;

	case kFMXT_Init:
		{
			if (g_plugin == NULL || !g_plugin->init())
				pb->result = 0;
			else
				pb->result = kCurrentExtnVersion;
		}
		break;

	case kFMXT_External:
		{
			if (g_plugin == NULL)
			{
				pb->result = 1;
			}
			else
			{
				try
				{
					pb->result = 0;
				}
				catch (Error&)
				{
					pb->result = 2;
				}
			}
		}
		break;

	case kFMXT_DoAppPreferences:
		{
			if (g_plugin == NULL)
			{
				BasicDialogBox("No plugin instance exists. This is very unexpected, and (most probably) indicates a bug in the plugin", "Plugin error", DLGSTYLE_OK | DLGSTYLE_ERROR);
				pb->result = 1;
			}
			else
			{
				try
				{
					g_plugin->showPreferences();
					pb->result = 0;
				}
				catch (Error& err)
				{
					BasicDialogBox("Unhandled exception while setting the preferences. Error details:\n"+err.getReason(), "Error while setting preferences", DLGSTYLE_OK | DLGSTYLE_ERROR);
					pb->result = 2;
				}
			}
		}
	}
}

FMX_PROC(fmx::errcode) g_funcall(short functionId, const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
{
	if (!g_plugin)
	{
		BasicDialogBox("No plugin instance exists. This is very unexpected, and (most probably) indicates a bug in the plugin", "Plugin error", DLGSTYLE_OK | DLGSTYLE_ERROR);
		return 1;
	}
	else
	{
		try
		{
			g_plugin->callFunction(functionId, env, parms, result);
			return 0;
		}
		catch (Error& err)
		{
			std::stringstream ss;
			ss << "Unhandled exception in plugin '" << g_plugin->getName() << "'";
			const Function* func = NULL;
			try {
				func = g_plugin->getFunction(functionId);
			} catch (Error&) {}
					
			if (func)
			{
				ss << std::endl << err.getReason();
			}
			else
			{
				ss << " while calling function '" << func->getName() << "'" << std::endl << err.getReason();
				BasicDialogBox(ss.str(), "Error in external function call", DLGSTYLE_OK | DLGSTYLE_ERROR);
			}
			return 2;
		}
	}
}

void Plugin::callFunction(short int functionId, const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
{
	FunctionsById::iterator fit = _functionsById.find(functionId);
	if (fit == _functionsById.end())
		throw Error("No such function Id: %d", functionId);

	fit->second->call(env, parms, result);
}

const Function* const Plugin::getFunction(short int id) const
{
	FunctionsById::const_iterator fit = _functionsById.find(id);
	if (fit == _functionsById.end())
		return NULL;
	return fit->second;
}

void Plugin::showPreferences()
{
	throw Error("Plugin indicates it supports preferences, but no code has been written for them. Please slap the plugin developer.");
}

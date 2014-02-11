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

#ifndef _FMPLUG_PLUGIN_H
#define _FMPLUG_PLUGIN_H

#include <fmplug/Export.h>
#include <string>
#include <map>
#include <vector>
#include <FMWrapper/FMXExtern.h>
#include <FMWrapper/FMXTypes.h>
#include <FMWrapper/FMXData.h>
#include <FMWrapper/FMXText.h>
#include <FMWrapper/FMXFixPt.h>
#include <FMWrapper/FMXCalcEngine.h>
#include <fmplug/UtfConv.h>
#include <fmplug/Error.h>
#include <fmplug/Function.h>



namespace fmplug
{

class PluginFactory;

class Plugin
{
public:
	Plugin(const std::string& id, const std::string& name, const std::string& help);
	virtual ~Plugin();

	const std::string& getId() const { return _id; }
	const std::string& getName() const { return _name; }
	const std::string& getHelp() const { return _help; }
	std::string getOptions() const;
	virtual bool hasPreferences() const { return false; }

	bool init();

	void callFunction(short int functionId, const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result);

	virtual void showPreferences();

protected:
	virtual void declareFunctions();
	void registerFunction(Function* function);

private:
	std::string _id;
	std::string _name;
	std::string _help;

	typedef std::map<std::string,Function*> FunctionsByName;
	FunctionsByName _functionsByName;
	typedef std::map<short int,Function*> FunctionsById;
	FunctionsById _functionsById;
	short int _nextFuncId;

public:
	// This one has to be public because:
	// - we can't have it called by the base PluginFactory (otherwise we're doing it 
	// in the constructor, and hence we will not be able to called back to 
	// createInstance() properly - pure-virtual call). 
	// - we can't "friend" it either, because we do not know in advance who we
	// shall be friend to (friendship is not inheritable in C++).
	static void main(FMX_ExternCallPtr pb, PluginFactory* factory);
};

class PluginFactory
{
public:
	PluginFactory(FMX_ExternCallPtr pb) {}
	virtual Plugin* createInstance() const = 0;
};

};


#define DECLARE_PLUGIN( className ) \
	class className##Factory : public fmplug::PluginFactory \
	{ \
	public: \
		className##Factory(FMX_ExternCallPtr pb) : fmplug::PluginFactory(pb) { fmplug::Plugin::main(pb, this); } \
		fmplug::Plugin* createInstance() const { return new className(); } \
	}; \
	void FMExternCallProc(FMX_ExternCallPtr pb) \
	{ \
		MyPluginFactory factory(pb); \
	}


#endif //_FMPLUG_PLUGIN_H

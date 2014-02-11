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

#ifndef _FMPLUG_FUNCTION_H
#define _FMPLUG_FUNCTION_H

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


namespace fmplug
{

class FMPLUG_API Function
{
public:
	enum ArgumentType
	{
		AT_VARIANT,

		AT_INTEGER,
		AT_TEXT,
		AT_FLOAT,
		AT_DATETIME,
	};
	struct FMPLUG_API Argument
	{
		Argument(ArgumentType type, const std::string& name, bool optional=false) : type(type), name(name), optional(optional) {}

		ArgumentType type;
		std::string name;
		bool optional;

	};
	typedef std::vector<Argument> Arguments;

	enum Flags
	{
		F_UNLIMITED_ARGUMENTS		= 0x1,
	};

public:
	Function(const std::string& declaration, unsigned int flags = 0);
	Function(const std::string& name, const Arguments& args, unsigned int flags = 0);
	
	short int getId() const;
	const std::string& getName() const { return _name; }
	std::string getPrototype() const;

	int getMinArgs() const;
	int getMaxArgs() const;

	const Arguments& getArguments() const { return _args; }
	void addArgument(const Argument& arg);

	const ArgumentType getReturnType() const { return _retType; }
	void setReturnType(ArgumentType type) { _retType = type; }

	void call(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result);

	virtual void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result) = 0;

private:
	void bind(short int id);
	friend class Plugin;

	void parseDeclaration(const std::string& declaration);

private:
	std::string _declaration;
	std::string _name;
	short int _boundId;
	Arguments _args;
	ArgumentType _retType;
	unsigned int _flags;
};

class FMPLUG_API DummyFunction : public Function
{
public:
	DummyFunction(const std::string& declaration, unsigned int flags = 0) : Function(declaration, flags) {}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result) {}
};

};

#endif //_FMPLUG_FUNCTION_H
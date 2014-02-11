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

#ifndef _FMPLUG_TYPEMAPS_H
#define _FMPLUG_TYPEMAPS_H

#include <fmplug/Export.h>
#include <FMWrapper/FMXExtern.h>
#include <FMWrapper/FMXTypes.h>
#include <FMWrapper/FMXData.h>
#include <FMWrapper/FMXText.h>
#include <FMWrapper/FMXFixPt.h>
#include <FMWrapper/FMXCalcEngine.h>
#include <fmplug/UtfConv.h>
#include <fmplug/Error.h>
#include <fmplug/Function.h>

#include <string>
#include <map>
#include <vector>

namespace fmplug
{
	
class TMType {};
class TMInt : public TMType {
public:
	static const Function::ArgumentType AT = Function::AT_INTEGER;
	typedef int CT;
};
class TMText : public TMType {
public:
	static const Function::ArgumentType AT = Function::AT_TEXT;
	typedef std::string CT;
};
class TMFloat : public TMType {
public:
	static const Function::ArgumentType AT = Function::AT_FLOAT;
	typedef double CT;
};

template <class T>
static T TM_GetArg(const fmx::DataVect& parms, fmx::uint32 index)
{
	throw Error("Invalid conversion");
}
template <>
static int TM_GetArg(const fmx::DataVect& parms, fmx::uint32 index)
{
	try
	{
		const fmx::FixPt& num(parms.AtAsNumber(index));
		return num.AsLong();
	}
	catch (std::out_of_range&)
	{
		throw fmplug::Error("Index out of range: %u (max index=%u)", index, parms.Size());
	}
}
template <>
static std::string TM_GetArg(const fmx::DataVect& parms, fmx::uint32 index)
{
	try
	{
		const fmx::Text& text(parms.AtAsText(index));
		fmx::uint32 sz = text.GetSize();
		std::vector<fmx::uint16> ubuffer(sz+1);
		text.GetUnicode(&ubuffer[0], 0, sz+1);
		ubuffer[sz] = L'\0';
		std::wstring wres((wchar_t*)(&ubuffer[0]));
		return convertUTF16toUTF8(wres);
	}
	catch (std::out_of_range&)
	{
		throw fmplug::Error("Index out of range: %u (max index=%u)", index, parms.Size());
	}
}
template <>
static double TM_GetArg(const fmx::DataVect& parms, fmx::uint32 index)
{
	try
	{
		const fmx::FixPt& num(parms.AtAsNumber(index));
		return num.AsFloat();
	}
	catch (std::out_of_range&)
	{
		throw fmplug::Error("Index out of range: %u (max index=%u)", index, parms.Size());
	}
}

template <class T>
static void TM_SetArg(fmx::Data& result, const T& value)
{
	throw Error("Invalid conversion");
}
template <>
static void TM_SetArg(fmx::Data& result, const int& value)
{
	fmx::FixPtAutoPtr num;
	num->AssignInt(value);
	result.SetAsNumber(*num);
}
template <>
static void TM_SetArg(fmx::Data& result, const std::string& value)
{
	fmx::TextAutoPtr text;
	text->AssignUnicode((fmx::uint16*)convertUTF8toUTF16(value).c_str());
	result.SetAsText(*text, result.GetLocale());
}
template <>
static void TM_SetArg(fmx::Data& result, const double& value)
{
	fmx::FixPtAutoPtr num;
	num->AssignDouble(value);
	result.SetAsNumber(*num);
}






template <class r>
class SimpleFunction0 : public Function
{
public:
	typedef typename r::CT(*Hook)();
	SimpleFunction0(const std::string& name, Hook func)
		: Function(name, Arguments()), _hook(func)
	{
	}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		typename r::CT res = _hook();
		TM_SetArg(result, res);
	}
private:
	Hook _hook;
};

template <class r, class a1>
class SimpleFunction1 : public Function
{
public:
	typedef typename r::CT(*Hook)(const typename a1::CT&);
	SimpleFunction1(const std::string& name, const std::string& a1name, Hook func)
		: Function(name, Arguments()), _hook(func)
	{
		addArgument(Argument(a1::AT, a1name));
	}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		typename r::CT res = _hook(TM_GetArg<typename a1::CT>(parms,0));
		TM_SetArg(result, res);
	}
private:
	Hook _hook;
};

template <class r, class a1, class a2>
class SimpleFunction2 : public Function
{
public:
	typedef typename r::CT(*Hook)(const typename a1::CT&, const typename a2::CT&);
	SimpleFunction2(const std::string& name, const std::string& a1name, const std::string& a2name, Hook func)
		: Function(name, Arguments()), _hook(func)
	{
		addArgument(Argument(a1::AT, a1name));
		addArgument(Argument(a2::AT, a2name));
	}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		typename r::CT res = _hook(TM_GetArg<typename a1::CT>(parms,0), TM_GetArg<typename a2::CT>(parms,1));
		TM_SetArg(result, res);
	}
private:
	Hook _hook;
};

template <class r, class a1, class a2, class a3>
class SimpleFunction3 : public Function
{
public:
	typedef typename r::CT(*Hook)(const typename a1::CT&, const typename a2::CT&, const typename a3::CT&);
	SimpleFunction3(const std::string& name, const std::string& a1name, const std::string& a2name, const std::string& a3name, Hook func)
		: Function(name, Arguments()), _hook(func)
	{
		addArgument(Argument(a1::AT, a1name));
		addArgument(Argument(a2::AT, a2name));
		addArgument(Argument(a3::AT, a3name));
	}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		typename r::CT res = _hook(TM_GetArg<typename a1::CT>(parms,0), TM_GetArg<typename a2::CT>(parms,1), TM_GetArg<typename a3::CT>(parms,2));
		TM_SetArg(result, res);
	}
private:
	Hook _hook;
};

};

#endif //_FMPLUG_TYPEMAPS_H
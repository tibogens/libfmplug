/*
Copyright (c) 2014, University of Lausanne, Switzerland 
All rights reserved. 

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are 
met: 

1. Redistributions of source code must retain the above copyright 
notice, this list of conditions and the following disclaimer. 

2. Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution. 

3. Neither the name of the copyright holder nor the names of its 
contributors may be used to endorse or promote products derived from 
this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

*/

#include "pch.h"
#include <iostream>
#include <fmplug/Setup.h>
#include <fmplug/Plugin.h>
#include <FMWrapper/FMXText.h>
#include <fmplug/TypeMaps.h>
#include <fmplug/Utils.h>

std::string testproc2_hook(const int& x, const std::string& y)
{
	return "Hello world ! (from C++ template)\r"+y;
}

class MyFunction : public fmplug::Function
{
public:
	MyFunction() : Function("testproc3(int x, string y):string") {}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		int x = fmplug::TM_GetArg<int>(parms, 0);
		std::string y = fmplug::TM_GetArg<std::string>(parms, 1);
		std::string value = "Hello world ! (from low-level fmx function)\r"+y;

		fmplug::TM_SetArg(result, value);
	}
};

class BasicPlugin : public fmplug::Plugin
{
public:
	BasicPlugin() : fmplug::Plugin("LFMP", "BasicPlugin", "This is some help text that appears in the Edit/Preferences/Plug-ins panel when BasicPlugin is selected. This plugin defines three dummy functions named testproc1-2-3 that take an int and a string, and return a string.") {}
	bool hasPreferences() const { return true; }
	void showPreferences() {
		fmplug::BasicDialogBox("This is a dummy configuration dialog for BasicPlugin. You will need to use your own windowing code to show a real preferences dialog until libfmplug supports some basic controls (e.g. using wxWidgets/Qt)", "BasicPlugin settings", fmplug::DLGSTYLE_INFO);
	};
protected:
	void declareFunctions()
	{
		registerFunction(new fmplug::DummyFunction("testproc(int x, string y):string"));
		registerFunction(new fmplug::SimpleFunction2<fmplug::TMText,fmplug::TMInt,fmplug::TMText>("testproc2", "x", "y", &testproc2_hook));
		registerFunction(new MyFunction());
	}
};

DECLARE_PLUGIN(BasicPlugin);

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

#include <iostream>
#include <fmpesc/Plugin.h>
#include <FMWrapper/FMXText.h>
#include <fmpesc/TypeMaps.h>
#include <fmpesc/Utils.h>

std::string testproc2_hook(const int& x, const std::string& y)
{
	return "Hello world ! (from C++ template)\r"+y;
}

class MyFunction : public fmpesc::Function
{
public:
	MyFunction() : Function("testproc3(int x, string y):string") {}
	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		int x = fmpesc::TM_GetArg<int>(parms, 0);
		std::string y = fmpesc::TM_GetArg<std::string>(parms, 1);
		std::string value = "Hello world ! (from low-level fmx function)\r"+y;

		fmpesc::TM_SetArg(result, value);
	}
};

class MyPlugin : public fmpesc::Plugin
{
public:
	MyPlugin() : fmpesc::Plugin("ESCC", "MyPlugin", "This is some help text") {}
	bool hasPreferences() const { return true; }
	void showPreferences() {
		fmpesc::BasicDialogBox("This is the configuration dialog for MyPlugin", "MyPlugin settings", fmpesc::DLGSTYLE_INFO);
	};
protected:
	void declareFunctions()
	{
		registerFunction(new fmpesc::DummyFunction("testproc(int x, string y):string"));
		registerFunction(new fmpesc::SimpleFunction2<fmpesc::TMText,fmpesc::TMInt,fmpesc::TMText>("testproc2", "x", "y", &testproc2_hook));
		registerFunction(new MyFunction());
	}
};

DECLARE_PLUGIN(MyPlugin);

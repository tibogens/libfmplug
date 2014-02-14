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
#include <fmutils/Popen.h>

#define BUFFER_SIZE 256

std::string getTextParam(const fmx::DataVect& dataVect, fmx::uint32 index)
{
	const fmx::Text& param = dataVect.AtAsText(index);
	char buffer[256];
	fmx::uint32 sz = std::min((fmx::uint32)255, param.GetSize());
	param.GetBytes(buffer, 255, 0);
	buffer[sz] = '\0';
	return std::string(buffer);
}

void fillPopen(fmutils::Popen& popen, const fmx::DataVect& dataVect, bool hasStdIn)
{
	for (fmx::uint32 i=0;i<dataVect.Size();++i)
	{
		std::string arg = getTextParam(dataVect, i);
		if (i==0)
			popen.setCommand(arg);
		else if (hasStdIn && i==1)
			popen.setStdIn(arg);
		else
			popen.getArguments().push_back(arg);
	}
}

std::string CRLF_to_CR(const std::string& in)
{
	std::stringstream ssin, ssout;
	ssin << in;
	std::string line;
	std::getline(ssin, line);
	while (ssin)
	{
		ssout << line;
		std::getline(ssin, line);
	}
	return ssout.str();
}
class SubprocessCall : public fmplug::Function
{
public:
	SubprocessCall() : Function("subprocess_call", Function::Arguments(), fmplug::Function::F_UNLIMITED_ARGUMENTS) 
	{
		addArgument(Argument(AT_TEXT, "command"));
		setReturnType(Function::AT_TEXT);
	}


	void operator()(const fmx::ExprEnv& env, const fmx::DataVect& parms, fmx::Data& result)
	{
		std::string command = fmplug::TM_GetArg<std::string>(parms, 0);

		fmutils::Popen proc;

		int min_ = getMinArgs();
		int max_ = getMaxArgs();
		if (max_ <= -1) max_ = (int)parms.Size();

		for (int i=min_;i<max_;++i)
		{
			std::string arg = fmplug::TM_GetArg<std::string>(parms, i);
			proc.getArguments().push_back(arg);
		}

		proc.setCommand(command);

		try
		{
			proc.start();
			int ret = proc.wait();
			std::string output = CRLF_to_CR(proc.getStdOut());
			fmplug::TM_SetArg(result, output);
		}
		catch (fmplug::Error& ex)
		{
			std::stringstream ss;
			ss << "Unhandled exception\n\n" << ex;
			fmplug::BasicDialogBox(ss.str().c_str(), "Popen plugin error");
		}
	}
};

class BasicPlugin : public fmplug::Plugin
{
public:
	BasicPlugin() : fmplug::Plugin("LFM2", "PopenPlugin", "Demonstrates various uses of the fmutils::Popen class") {}
protected:
	void declareFunctions()
	{
		registerFunction(new SubprocessCall());
	}
};

DECLARE_PLUGIN(BasicPlugin);

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

#include <iostream>
#include <fmpesc/Plugin.h>
#include <fmpesc/Error.h>

int main(int argc, char* argv[])
{
	try
	{
		fmpesc::DummyFunction f("testfunc(int a, float b, text c):int");
	}
	catch (fmpesc::Error& err)
	{
		std::cout << "ERROR: " << err << std::endl;
		return 1;
	}
	return 0;
}

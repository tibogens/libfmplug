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

#ifndef _FMUTILS_EXPORT_H
#define _FMUTILS_EXPORT_H

#ifdef _WIN32
	#ifdef libfmplug_STATIC
		#define FMUTILS_API
	#else
		#ifdef libfmutils_EXPORTS
			#define FMUTILS_API __declspec(dllexport)
		#else
			#define FMUTILS_API __declspec(dllimport)
		#endif
	#endif
#else
	#define FMUTILS_API
#endif


#endif //_FMUTILS_EXPORT_H
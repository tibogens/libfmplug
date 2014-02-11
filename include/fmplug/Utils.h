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

#ifndef _FMPLUG_UTILS_H
#define _FMPLUG_UTILS_H

#include <fmplug/Export.h>
#include <string>
#include <map>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace fmplug
{

class FMPLUG_API FMStringCache
{
public:
	const std::wstring& get(int key, bool* found) const;
	void set(int key, const std::wstring& value);
	void clear();
private:
	typedef std::map<int,std::wstring> Data;
	Data _data;
	std::wstring _sink;
};



// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}


enum BasicDialogBoxStyle
{
	DLGSTYLE_OK				= 0x0000,
	DLGSTYLE_OKCANCEL		= 0x0001,
	DLGSTYLE_YESNO			= 0x0002,
	DLGSTYLE_YESNOCANCEL	= 0x0003,

	DLGSTYLE_INFO			= 0x0100,
	DLGSTYLE_QUESTION		= 0x0200,
	DLGSTYLE_WARNING		= 0x0300,
	DLGSTYLE_ERROR			= 0x0400,
};

FMPLUG_API int BasicDialogBox(const std::string& message, const std::string& title, unsigned int style=DLGSTYLE_OK);

};

#endif //_FMPLUG_UTILS_H

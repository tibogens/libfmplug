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

#ifndef _FMPLUG_UTFCONV_H
#define _FMPLUG_UTFCONV_H

#include <fmplug/Export.h>
#include <string>

namespace fmplug
{

std::string convertUTF16toUTF8(const wchar_t* source, unsigned sourceLength);
std::wstring convertUTF8toUTF16(const char* source, unsigned sourceLength);

std::string convertUTF16toUTF8(const std::wstring& s);
std::string convertUTF16toUTF8(const wchar_t* s);

std::wstring convertUTF8toUTF16(const std::string& s);
std::wstring convertUTF8toUTF16(const char* s);

std::string convertStringFromCurrentCodePageToUTF8(const char* source, unsigned sourceLength);
std::string convertStringFromUTF8toCurrentCodePage(const char* source, unsigned sourceLength);

std::string convertStringFromCurrentCodePageToUTF8(const std::string& s);
std::string convertStringFromCurrentCodePageToUTF8(const char* s);

std::string convertStringFromUTF8toCurrentCodePage(const std::string& s);
std::string convertStringFromUTF8toCurrentCodePage(const char* s);

};

#endif //_FMPLUG_UTFCONV_H

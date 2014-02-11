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

FMPLUG_API std::string convertUTF16toUTF8(const wchar_t* source, unsigned sourceLength);
FMPLUG_API std::wstring convertUTF8toUTF16(const char* source, unsigned sourceLength);

FMPLUG_API std::string convertUTF16toUTF8(const std::wstring& s);
FMPLUG_API std::string convertUTF16toUTF8(const wchar_t* s);

FMPLUG_API std::wstring convertUTF8toUTF16(const std::string& s);
FMPLUG_API std::wstring convertUTF8toUTF16(const char* s);

FMPLUG_API std::string convertStringFromCurrentCodePageToUTF8(const char* source, unsigned sourceLength);
FMPLUG_API std::string convertStringFromUTF8toCurrentCodePage(const char* source, unsigned sourceLength);

FMPLUG_API std::string convertStringFromCurrentCodePageToUTF8(const std::string& s);
FMPLUG_API std::string convertStringFromCurrentCodePageToUTF8(const char* s);

FMPLUG_API std::string convertStringFromUTF8toCurrentCodePage(const std::string& s);
FMPLUG_API std::string convertStringFromUTF8toCurrentCodePage(const char* s);

};

#endif //_FMPLUG_UTFCONV_H

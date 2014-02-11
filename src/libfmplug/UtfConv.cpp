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

#include "pch.h"
#include <fmplug/UtfConv.h>
#include <windows.h>
using namespace fmplug;

std::string fmplug::convertUTF16toUTF8(const std::wstring& s){return convertUTF16toUTF8(s.c_str(), s.length());}
std::string fmplug::convertUTF16toUTF8(const wchar_t* s){return convertUTF16toUTF8(s, wcslen(s));}

std::wstring fmplug::convertUTF8toUTF16(const std::string& s){return convertUTF8toUTF16(s.c_str(), s.length());}
std::wstring fmplug::convertUTF8toUTF16(const char* s){return convertUTF8toUTF16(s, strlen(s));}

std::string fmplug::convertStringFromCurrentCodePageToUTF8(const std::string& s){return convertStringFromCurrentCodePageToUTF8(s.c_str(), s.length());}
std::string fmplug::convertStringFromCurrentCodePageToUTF8(const char* s){return convertStringFromCurrentCodePageToUTF8(s, strlen(s));}

std::string fmplug::convertStringFromUTF8toCurrentCodePage(const std::string& s){return convertStringFromUTF8toCurrentCodePage(s.c_str(), s.length());}
std::string fmplug::convertStringFromUTF8toCurrentCodePage(const char* s){return convertStringFromUTF8toCurrentCodePage(s, strlen(s));}

std::string fmplug::convertUTF16toUTF8(const wchar_t* source, unsigned sourceLength)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	if (sourceLength == 0)
	{
		return std::string();
	}

	int destLen = WideCharToMultiByte(CP_UTF8, 0, source, sourceLength, 0, 0, 0, 0);
	if (destLen <= 0)
	{
		//OSG_WARN << "Cannot convert UTF-16 string to UTF-8." << std::endl;
		return std::string();
	}

	std::string sDest(destLen, '\0');
	destLen = WideCharToMultiByte(CP_UTF8, 0, source, sourceLength, &sDest[0], destLen, 0, 0);

	if (destLen <= 0)
	{
		//OSG_WARN << "Cannot convert UTF-16 string to UTF-8." << std::endl;
		return std::string();
	}

	return sDest;
#else
	//TODO: Implement for other platforms
	//OSG_WARN << "ConvertUTF16toUTF8 not implemented." << std::endl;
	return std::string();
#endif
}

std::wstring fmplug::convertUTF8toUTF16(const char* source, unsigned sourceLength)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	if (sourceLength == 0)
	{
		return std::wstring();
	}

	int destLen = MultiByteToWideChar(CP_UTF8, 0, source, sourceLength, 0, 0);
	if (destLen <= 0)
	{
		//OSG_WARN << "Cannot convert UTF-8 string to UTF-16." << std::endl;
		return std::wstring();
	}

	std::wstring sDest(destLen, L'\0');
	destLen = MultiByteToWideChar(CP_UTF8, 0, source, sourceLength, &sDest[0], destLen);

	if (destLen <= 0)
	{
		//OSG_WARN << "Cannot convert UTF-8 string to UTF-16." << std::endl;
		return std::wstring();
	}

	return sDest;
#else
	//TODO: Implement for other platforms
	//OSG_WARN << "ConvertUTF8toUTF16 not implemented." << std::endl;
	return std::wstring();
#endif
}

std::string fmplug::convertStringFromCurrentCodePageToUTF8(const char* source, unsigned sourceLength)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	if (sourceLength == 0)
	{
		return std::string();
	}

	int utf16Length = MultiByteToWideChar(CP_ACP, 0, source, sourceLength, 0, 0);
	if (utf16Length <= 0)
	{
		//OSG_WARN << "Cannot convert multi-byte string to UTF-8." << std::endl;
		return std::string();
	}

	std::wstring sUTF16(utf16Length, L'\0');
	utf16Length = MultiByteToWideChar(CP_ACP, 0, source, sourceLength, &sUTF16[0], utf16Length);
	if (utf16Length <= 0)
	{
		// OSG_WARN << "Cannot convert multi-byte string to UTF-8." << std::endl;
		return std::string();
	}

	return convertUTF16toUTF8(sUTF16);
#else
	return source;
#endif
}

std::string fmplug::convertStringFromUTF8toCurrentCodePage(const char* source, unsigned sourceLength)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	if (sourceLength == 0)
	{
		return std::string();
	}

	std::wstring utf16 = convertUTF8toUTF16(source, sourceLength);
	sourceLength = utf16.length();

	int destLen = WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), sourceLength, 0, 0, 0, 0);
	if (destLen <= 0)
	{
		//OSG_WARN << "Cannot convert multi-byte string to UTF-8." << std::endl;
		return std::string();
	}

	std::string sDest(destLen, '\0');
	destLen = WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), sourceLength, &sDest[0], destLen, 0, 0);
	if (destLen <= 0)
	{
		//OSG_WARN << "Cannot convert multi-byte string to UTF-8." << std::endl;
		return std::string();
	}

	return sDest;
#else
	return source;
#endif
}

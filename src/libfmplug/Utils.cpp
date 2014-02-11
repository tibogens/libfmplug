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
#include <fmplug/Setup.h>
#include <fmplug/Utils.h>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace fmplug;

const std::wstring& FMStringCache::get(int key, bool* found) const
{
	Data::const_iterator it = _data.find(key);
	if (it != _data.end())
	{
		*found = true;
		return it->second;
	}
	else
	{
		*found = false;
		return _sink;
	}
}

	
void FMStringCache::set(int key, const std::wstring& value)
{
	_data[key] = value;
}

void FMStringCache::clear()
{
	_data.clear();
}


int fmplug::BasicDialogBox(const std::string& message, const std::string& title, unsigned int style)
{
	UINT win32style = 0;
	
	switch (style & 0x00ff)
	{
	case DLGSTYLE_OK:
		win32style |= MB_OK;
		break;
	case DLGSTYLE_OKCANCEL:
		win32style |= MB_OKCANCEL;
		break;
	case DLGSTYLE_YESNO:
		win32style |= MB_YESNO;
		break;
	case DLGSTYLE_YESNOCANCEL:
		win32style |= MB_YESNOCANCEL;
		break;
	}

	switch (style & 0xff00)
	{
	case DLGSTYLE_ERROR:
		win32style |= MB_ICONHAND;
		break;
	case DLGSTYLE_QUESTION:
		win32style |= MB_ICONQUESTION;
		break;
	case DLGSTYLE_WARNING:
		win32style |= MB_ICONEXCLAMATION;
		break;
	case DLGSTYLE_INFO:
		win32style |= MB_ICONASTERISK;
		break;
	}

	HWND hwnd = ::GetForegroundWindow(); // This is hacky, but allows the dialog to be modal.
	int answer = ::MessageBox(hwnd, message.c_str(), title.c_str(), win32style);

	return answer;
}

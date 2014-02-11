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

#ifndef _FMPLUG_ERROR_H
#define _FMPLUG_ERROR_H

#include <fmplug/Export.h>
#include <string>
#include <stdarg.h>

namespace fmplug
{

int LastError();

class Error
{
public:
	Error();

	Error(const char* reason, ...);

	static Error System(const std::string& api);

	virtual ~Error();

	inline const std::string& getReason() const { return _reason; }

	friend std::ostream& operator<<(std::ostream& stream, const Error& err);

protected:
	void set(const char* reason, ...);
	void format(const char* in, va_list arglist);

private:
	std::string _reason;
};

std::ostream& operator<<(std::ostream& stream, const Error& err);

};

#endif // _FMPLUG_ERROR_H

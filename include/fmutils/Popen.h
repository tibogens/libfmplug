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

#ifndef _FMPLUG_POPEN_H
#define _FMPLUG_POPEN_H

#include <string>
#include <vector>

namespace fmutils
{

class Popen
{
public:
	typedef std::vector<std::string> Arguments;


public:
	
	Popen();
	virtual ~Popen();

	const std::string& getCommand() const { return _command; }
	void setCommand(const std::string& command);

	const Arguments& getArguments() const { return _args; }
	Arguments& getArguments();

	void setStdIn(const std::string& in);
	
	bool hasFailed() const; // returns true if the status is 5 (see below)
	int getExitCode() const;
	const std::string& getStdOut() const;
	const std::string& getStdErr() const;

	void start();
	void reset();
	int wait();

private:
	std::string _command;
	Arguments _args;
	std::string _stdin, _stdout, _stderr;

	// State graph of the Popen instance
	// 0 uninit
	// 1 ready
	// 2 started
	// 3 stopped/success
	// 4 stopped/fail
	// 5 error
	// 6 destroyed
	//                  /->  3  \
	//  0  ->   1  ->  2         -->  1(loop)
	//            \     \->  4  /   /
	//             \->     5      -/
	// 6 can happen at any time (need to implement proper cleanup for all possible states)
	// 5 can happen only in 1
	//
	// Constraints:
	// - clients may call setCommand/getArguments/setStdIn during 1 only
	// - clients may call getStdOut/getStdErr only in 3
	// - reset() only in 3 or 4
	//
	// Design:
	// Each transition in the graph needs to be implemented as a method for those that
	// are client-driven, and by event handlers/waiters for the system ones (2->3/4, mostly)
	// 0->1 is the constructor
	// 1->2 is the start() method
	// 3->1 and 4->1 is the reset() method 
	enum Status
	{
		UNINITIALIZED	= 0,
		READY			= 1,
		STARTED			= 2,
		CHILD_SUCCESS	= 3,
		CHILD_FAILURE	= 4,
		INTERNAL_ERROR	= 5,
	};

	Status _status;
	void checkStatus(Status expected) const;

public:
	class Implementation {
	public:
		virtual ~Implementation();
	};
protected:
private:
	Implementation* _pimpl;
	int _retcode;
};

};

#endif //_FMPLUG_POPEN_H

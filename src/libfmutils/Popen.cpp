#include "pch.h"
#include <fmutils/Popen.h>
#include <fmplug/Error.h>
#include <windows.h>
#include <sstream>
using namespace fmutils;

class Win32Impl : public Popen::Implementation
{
public:
	Win32Impl()
	{
		_ready = true;
		_failed = false;
		hOutputRead = NULL;
		hOutputWrite = NULL;
		hInputRead = NULL;
		hInputWrite = NULL;
		hErrorWrite = NULL;
		hChildProcess = NULL;
		//hStdIn = NULL;

		// Set up the security attributes struct.
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
	}

	virtual ~Win32Impl()
	{
		//if (hStdIn) CloseHandle(hStdIn);
		if (hOutputRead) CloseHandle(hOutputRead);
		if (hOutputWrite) CloseHandle(hOutputWrite);
		if (hInputRead) CloseHandle(hInputRead);
		if (hInputWrite) CloseHandle(hInputWrite);
		if (hErrorWrite) CloseHandle(hErrorWrite);
		if (hChildProcess) CloseHandle(hChildProcess);
	}

public:
	void start(const std::string& command, const Popen::Arguments& args)
	{
		createHandles();
		launch(command, args);
	}
	int communicate(std::ostream& output, std::string* input)
	{
		if (input)
		{
			throw fmplug::Error("stdin: Unimplemented");
		}
		else
		{
			if (hInputRead) CloseHandle(hInputRead);
			hInputRead = NULL;
			if (hInputWrite) CloseHandle(hInputWrite);
			hInputWrite = NULL;
		}

		readOutput(output);

		if (WaitForSingleObject(hChildProcess, INFINITE) == WAIT_FAILED)
			throw fmplug::Error::System("WaitForSingleObject");
		
		DWORD retcode = 0;
		if (!GetExitCodeProcess(hChildProcess, &retcode))
			throw fmplug::Error::System("GetExitCodeProcess");
		return retcode;
	}
private:
	void createHandles()
	{
		if (!_ready)
			throw fmplug::Error("Internal error");
		_ready = false;
		_failed = false;

		HANDLE hOutputReadTmp=NULL, hInputWriteTmp=NULL;
		try
		{
			// Create the child output pipe.
			if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0))
				throw fmplug::Error::System("CreatePipe");


			// Create a duplicate of the output write handle for the std error
			// write handle. This is necessary in case the child application
			// closes one of its std output handles.
			if (!DuplicateHandle(GetCurrentProcess(), hOutputWrite,
				GetCurrentProcess(), &hErrorWrite, 0,
				TRUE, DUPLICATE_SAME_ACCESS))
				throw fmplug::Error::System("DuplicateHandle");


			// Create the child input pipe.
			if (!CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0))
				throw fmplug::Error::System("CreatePipe");


			// Create new output read handle and the input write handles. Set
			// the Properties to FALSE. Otherwise, the child inherits the
			// properties and, as a result, non-closeable handles to the pipes
			// are created.
			if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
				GetCurrentProcess(),
				&hOutputRead, // Address of new handle.
				0,FALSE, // Make it uninheritable.
				DUPLICATE_SAME_ACCESS))
				throw fmplug::Error::System("DuplicateHandle");

			if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
				GetCurrentProcess(),
				&hInputWrite, // Address of new handle.
				0,FALSE, // Make it uninheritable.
				DUPLICATE_SAME_ACCESS))
				throw fmplug::Error::System("DuplicateHandle");


			// Close inheritable copies of the handles you do not want to be
			// inherited.
			if (!CloseHandle(hOutputReadTmp)) throw fmplug::Error::System("CloseHandle");
			hOutputReadTmp = NULL;
			if (!CloseHandle(hInputWriteTmp)) throw fmplug::Error::System("CloseHandle");
			hInputWriteTmp = NULL;

			// Get std input handle so you can close it and force the ReadFile to
			// fail when you want the input thread to exit.
			//if ( (hStdIn = GetStdHandle(STD_INPUT_HANDLE)) ==
			//	INVALID_HANDLE_VALUE )
			//	throw ce::Error::System("GetStdHandle");
		}
		catch (fmplug::Error&)
		{
			_failed = true;
			if (hOutputReadTmp) CloseHandle(hOutputReadTmp);
			if (hInputWriteTmp) CloseHandle(hInputWriteTmp);
			throw;
		}
	}

	void launch(const std::string& command, const Popen::Arguments& args)
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		try
		{
			// Set up the start up info struct.
			ZeroMemory(&si,sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdOutput = hOutputWrite;
			si.hStdInput  = hInputRead;
			si.hStdError  = hErrorWrite;
			// Use this if you want to hide the child:
			//     si.wShowWindow = SW_HIDE;
			// Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
			// use the wShowWindow flags.


			std::string cmdLine = escapeWin32Argument(command);
			if (!args.empty())
				cmdLine += " " + escapeWin32CommandLine(args);
			// Convert the command line to a non-const char array (required by CreateProcess)
			std::vector<char> cmdLinev;
			cmdLinev.assign(cmdLine.begin(), cmdLine.end());
			cmdLinev.push_back('\0');
			
			// Launch the process that you want to redirect (in this case,
			// Child.exe). Make sure Child.exe is in the same directory as
			// redirect.c launch redirect from a command line to prevent location
			// confusion.
			if (!CreateProcess(command.c_str(), &cmdLinev[0], NULL, NULL, TRUE,
				CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
				throw fmplug::Error::System("CreateProcess");


			// Set global child process handle to cause threads to exit.
			hChildProcess = pi.hProcess;


			// Close any unnecessary handles.
			if (!CloseHandle(pi.hThread)) throw fmplug::Error::System("CloseHandle");

			// Close pipe handles (do not continue to modify the parent).
			// You need to make sure that no handles to the write end of the
			// output pipe are maintained in this process or else the pipe will
			// not close when the child process exits and the ReadFile will hang.
			if (!CloseHandle(hOutputWrite)) throw fmplug::Error::System("CloseHandle");
			hOutputWrite = NULL;
			if (!CloseHandle(hInputRead)) throw fmplug::Error::System("CloseHandle");
			hInputRead = NULL;
			if (!CloseHandle(hErrorWrite)) throw fmplug::Error::System("CloseHandle");
			hErrorWrite = NULL;
		}
		catch (fmplug::Error&)
		{
			_failed = true;
			if (pi.hThread) CloseHandle(pi.hThread);
			if (pi.hProcess && !hChildProcess) CloseHandle(pi.hProcess);
			throw;
		}
	}

/*	void cleanup()
	{
		//if (WaitForSingleObject(hThread,INFINITE) == WAIT_FAILED)
		//	throw ce::Error::System("WaitForSingleObject");

		if (!CloseHandle(hOutputRead)) throw ce::Error::System("CloseHandle");
		if (!CloseHandle(hInputWrite)) throw ce::Error::System("CloseHandle");
	}*/

	void readOutput(std::ostream& output)
	{
		CHAR lpBuffer[256];
		DWORD nBytesRead;

		while(TRUE)
		{
			if (!ReadFile(hOutputRead,lpBuffer,sizeof(lpBuffer),
				&nBytesRead,NULL) || !nBytesRead)
			{
				if (GetLastError() == ERROR_BROKEN_PIPE)
					break; // pipe done - normal exit path.
				else
					throw fmplug::Error::System("ReadFile"); // Something bad happened.
			}

			lpBuffer[min(255,nBytesRead)] = '\0';
			output << lpBuffer;
			// Display the character read on the screen.
			//if (!WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),lpBuffer,
			//	nBytesRead,&nCharsWritten,NULL))
			//	throw ce::Error::System("WriteConsole");
		}
	}

	// Properly escapes an argument to be passed to CreateProcess() on Windows. Please read the
	// very interesting article (that demonstrates how CreateProcess() is broken by the way)
	// http://blogs.msdn.com/b/twistylittlepassagesallalike/archive/2011/04/23/everyone-quotes-arguments-the-wrong-way.aspx
	// Maybe one day Microsoft will publish something like CreateProcessEx() that takes an array
	// of arguments, instead of a string it then splits back using CommandLineToArgvW(). But in the
	// meantime, we're forced to use this.
	static std::string escapeWin32Argument(const std::string& argument, bool force=false);
	static std::string escapeWin32CommandLine(const Popen::Arguments& arguments, bool force=false);

private:
	bool _ready, _failed;
	HANDLE hOutputRead, hOutputWrite;
	HANDLE hInputRead, hInputWrite;
	HANDLE hErrorWrite;
	//HANDLE hThread;
	DWORD ThreadId;
	SECURITY_ATTRIBUTES sa;
	HANDLE hChildProcess;
	//HANDLE hStdIn; // Handle to parents std input.
	//BOOL bRunThread;
};

DWORD WINAPI GetAndSendInputThread(LPVOID lpvThreadParam);

std::string Win32Impl::escapeWin32Argument(const std::string& argument, bool force)
{
	if (!force && !argument.empty() && argument.find_first_of(" \t\n\v\"") == std::string::npos)
	{
		// Argument does not require quoting, and force is off -> return the original, unquoted argument
		return argument;
	}
	else 
	{
		std::stringstream ss;
		ss << '"';
		for (std::string::const_iterator it = argument.begin(); it != argument.end(); ++it)
		{
			int numBackslashes = 0;

			while (it != argument.end() && *it == '\\') 
			{
				++it;
				++numBackslashes;
			}

			if (it == argument.end()) 
			{
				// Escape all backslashes, but let the terminating
				// double quotation mark we add below be interpreted
				// as a metacharacter.
				ss << std::string(2*numBackslashes, '\\');
				break;
			}
			else if (*it == L'"') 
			{
				// Escape all backslashes and the following
				// double quotation mark.
				ss << std::string(1+2*numBackslashes, '\\') << *it;
			}
			else 
			{
				// Backslashes aren't special here.
				ss << std::string(2*numBackslashes, '\\') << *it;
			}
		}
		ss << '"';
		return ss.str();
	}

}

std::string Win32Impl::escapeWin32CommandLine(const Popen::Arguments& arguments, bool force)
{
	std::stringstream ss;
	int i=0;
	for (Popen::Arguments::const_iterator it = arguments.begin(); it != arguments.end(); ++it)
	{
		if (i++ > 0)
			ss << " ";
		ss << escapeWin32Argument(*it);
	}
	return ss.str();
}


/*

void main ()
{



	




	// Launch the thread that gets the input and sends it to the child.
	hThread = CreateThread(NULL,0,GetAndSendInputThread,
		(LPVOID)hInputWrite,0,&ThreadId);
	if (hThread == NULL) throw ce::Error::System("CreateThread");


	// Read the child's output.
	ReadAndHandleOutput(hOutputRead);
	// Redirection is complete


	// Force the read on the input to return by closing the stdin handle.
	if (!CloseHandle(hStdIn)) throw ce::Error::System("CloseHandle");


	// Tell the thread to exit and wait for thread to die.
	bRunThread = FALSE;

}


/////////////////////////////////////////////////////////////////////// 
// GetAndSendInputThread
// Thread procedure that monitors the console for input and sends input
// to the child process through the input pipe.
// This thread ends when the child application exits.
/////////////////////////////////////////////////////////////////////// 
DWORD WINAPI GetAndSendInputThread(LPVOID lpvThreadParam)
{
	CHAR read_buff[256];
	DWORD nBytesRead,nBytesWrote;
	HANDLE hPipeWrite = (HANDLE)lpvThreadParam;

	// Get input from our console and send it to child through the pipe.
	while (bRunThread)
	{
		if(!ReadConsole(hStdIn,read_buff,1,&nBytesRead,NULL))
			throw ce::Error::System("ReadConsole");

		read_buff[nBytesRead] = '\0'; // Follow input with a NULL.

		if (!WriteFile(hPipeWrite,read_buff,nBytesRead,&nBytesWrote,NULL))
		{
			if (GetLastError() == ERROR_NO_DATA)
				break; // Pipe was closed (normal exit path).
			else
				throw ce::Error::System("WriteFile");
		}
	}

	return 1;
}

*/

Popen::Implementation::~Implementation()
{
}

#define PIMPL( a ) Win32Impl* a = dynamic_cast<Win32Impl*>(_pimpl)

Popen::Popen()
{
	_status = UNINITIALIZED;
	
	_pimpl = new Win32Impl();
	_retcode = -1;

	_status = READY;
}

Popen::~Popen()
{
	if (_pimpl)
		delete _pimpl;
	_pimpl = NULL;
}


void Popen::checkStatus(Status expected) const
{
	if (_status != expected)
		throw fmplug::Error("ce::Popen::checkStatus() : current status is %d, API only allows this call when status is %d", _status, expected); 
}

void Popen::start()
{
	PIMPL(self);
	try
	{
		self->start(_command, _args);
		_status = STARTED;
	}
	catch (fmplug::Error&)
	{
		_status = INTERNAL_ERROR;
		throw;
	}
}

int Popen::wait()
{
	PIMPL(self);
	try
	{
		std::stringstream output;
		_retcode = self->communicate(output, _stdin.empty() ? NULL : &_stdin);
		_stdout = output.str();
		if (_retcode == 0)
			_status = CHILD_SUCCESS;
		else
			_status = CHILD_FAILURE;
		return _retcode;
	}
	catch (fmplug::Error&)
	{
		_status = INTERNAL_ERROR;
		_retcode = -1;
		throw;
	}
}

void Popen::reset()
{
	if (_status != CHILD_SUCCESS && _status != CHILD_FAILURE)
		throw fmplug::Error("ce::Popen::checkStatus() : current status is %d, API does not allow that.", _status); 
	
	_command = "";
	_args.clear();
	_retcode = -1;
	_stdout = "";
	_stderr = "";
	_stdin = "";
	if (_pimpl)
		delete _pimpl;
	_pimpl = new Win32Impl();
	_status = READY;
}

void Popen::setCommand(const std::string &command)
{
	checkStatus(READY);
	_command = command;
}

Popen::Arguments& Popen::getArguments()
{
	checkStatus(READY);
	return _args;
}

void Popen::setStdIn(const std::string& in)
{
	checkStatus(READY);
	_stdin = in;
}

const std::string& Popen::getStdOut() const
{
	if (_status != CHILD_SUCCESS && _status != CHILD_FAILURE)
		throw fmplug::Error("ce::Popen::checkStatus() : current status is %d, API does not allow that.", _status); 
	return _stdout;
}

bool Popen::hasFailed() const
{
	return _status == INTERNAL_ERROR;
}

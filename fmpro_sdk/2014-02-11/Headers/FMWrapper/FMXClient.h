/*
 
 Copyright © 1998 - 2012  FileMaker, Inc.
 All rights reserved.
 
 FileMaker, Inc. grants you a non-exclusive limited license to use this file solely to enable
 licensees of FileMaker Pro Advanced to compile plug-ins for use with FileMaker products.
 Redistribution and use in source and binary forms, without modification, are permitted provided
 that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of
 conditions and the following disclaimer.
 
 * The name FileMaker, Inc. may not be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY FILEMAKER, INC. ''AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL FILEMAKER, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */

#ifndef FMXCLIENT_H_INCLUDED
#define FMXCLIENT_H_INCLUDED

#include <stdexcept>
#include <memory>
#include <new>

#include "FMWrapper/FMXExtern.h"

extern "C++"
{
	namespace fmx
	{
		// This is used by other files for exception handling.
		struct _fmxcpt
		{
			enum
			{
				kNoErr,
				kBadAlloc,
				kUnknown
			};
			
			inline _fmxcpt ()
				: m_vers ( 1 )
				, m_code ( kNoErr )
			{
			}
			
			inline void Check ()
			{
				switch ( m_code )
				{
				case kNoErr:
					break;
				case kBadAlloc:
					throw std::bad_alloc ();
				case kUnknown:
					throw std::exception ();
				}
			}
			
			int32 m_vers;
			int32 m_code;
		};
	}
}

#endif // FMXCLIENT_H_INCLUDED

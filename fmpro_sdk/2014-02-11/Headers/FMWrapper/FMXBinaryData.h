/*
 
 Copyright © 1998 - 2013  FileMaker, Inc.
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

#ifndef _h_BinaryData_
#define _h_BinaryData_

#include "FMWrapper/FMXClient.h"
#include "FMWrapper/FMXTypes.h"

extern "C++"
{
    namespace fmx
    {
        // A BinaryData type is a collection of streams. Each stream has two parts, a TypeId and Data.
        //              There can be multiple related streams, but there should never be more than one
        //              stream of the same type.  Streams should not be empty.
        // Container fields can be considered to be of type BinaryData when data is stored in the database
        //          as opposed to objects inserted by reference (path of file as text).
        //              
        // To iterate through the streams of a BinaryData, use GetCount and then call GetIndex to
        //              get the TypeID for each stream.
        //
        // To create a new BinaryData object, use the autoptr version to construct the object, then
        //      Add() streams.
        class BinaryData
        {
        public:
            // Here are some common QuadChar types you may encounter, and notes on required usage.
            // The actual format of the data should adhere to the standard documented by
            // the format creator.
            //
            //  "FNAM" is a filename stream. This is used either to display the filename of the file
            //          in the field (non-images or images that cannot be displayed) and for a initial
            //          suggested filename when exporting field contents. Please use the special accessors
            //          below ("GetFNAMData and AddFNAMData) to get a text representation of this data.
            //
            //  Images:
            //          { "JPEG", "GIFf", "EPS ", "META", "PNGf", "BMPf", "PDF " }
            //          Images must have at least a JPEG, GIFf, or PNGf stream in addition to any
            //          other image type. A "SIZE" stream is also required for FileMaker to display
			//			the image. Please use the special accessors below (GetSIZEData and AddSIZEData)
			//			to get the height and width values for the image.
			//
            // When generating other kinds of graphical data, use JPEG or PNGf for a preview,               
            // and the other format will be preferred if possible. For example, to generate         
            // a TIFF picture, you have to add "TIFF" and "JPEG" streams.            
            //
            //  Files:
            //          { "FILE" (binary file data), "FORK" (Mac only resource fork) }
            //          
            //  Sound: 
            //          { "snd " }
            //          Always Mac raw format.
			//
			//  Other types:
			//			{ "MAIN" }
			//			When saving the contents of a container field to disk, FileMaker will first
			//			check if there is a MAIN stream and if present read the first four bytes as
			//			the QuadChar of the stream that is considered the main stream for the container.
			//			If this does not existed it falls back to the default rules for picking the
			//			main stream.

            // Any other types you encounter are not documented for plug-in use. Use them at your own risk.

            inline BinaryData &operator = ( const BinaryData &source );
            inline bool operator == ( const BinaryData &compareData ) const;
            inline bool operator != ( const BinaryData &compareData ) const;
            inline int32 GetCount () const;
            inline int32 GetIndex ( const QuadChar& dataType ) const;
            inline uint32 GetTotalSize () const;
            inline void GetType ( int32 index, QuadChar& dataType ) const;
            inline uint32 GetSize ( int32 index ) const;
            inline errcode GetData ( int32 index, uint32 offset, uint32 amount, void *buffer ) const;
            
            // You should exercise great care in using the following functions. Removing or modifying a required stream
            // could have undesirable and undefined results.
            inline errcode Add ( const QuadChar& dataType, uint32 amount, void *buffer );
            inline bool Remove ( const QuadChar& dataType );
            inline void RemoveAll ();

            // Special accessors for FileMaker specific storage formats  
            //          [ ONLY IN FM8 (API VERSION 51) AND LATER ]
            inline errcode GetFNAMData ( Text &filepathlist ) const;
            inline errcode AddFNAMData ( const Text &filepathlist );
            inline errcode GetSIZEData ( short &width, short &height ) const;
            inline errcode AddSIZEData ( short width, short height );

            inline void operator delete ( void *obj );

        private:
            BinaryData ();
            BinaryData ( const BinaryData &source );

        };

        class BinaryDataAutoPtr : public std::auto_ptr<BinaryData>
        {
            typedef BinaryDataAutoPtr   UpCaster;
        public:
            inline BinaryDataAutoPtr ();
            inline BinaryDataAutoPtr ( const BinaryData &sourceData );

			// This method sets the FNAM, SIZE, main stream, and all other information streams
			// for the given data. The extension of the file name passed in is used to determine
			// the type of data passed in the buffer. This routine does not read data from the
			// disk itself. Please use this to do "Insert from..." type operations.
			inline BinaryDataAutoPtr ( const Text &name, uint32 amount, void *buffer );
        };
    }
}


// These functions are only useful for C-only environments or for porting to other languages (ex., Pascal).
extern "C"
{
    fmx::BinaryData FMX_API *FM_BinaryData_Constructor1 ( fmx::_fmxcpt &_x ) throw ();
    fmx::BinaryData FMX_API *FM_BinaryData_Constructor2 ( const fmx::BinaryData &sourceData, fmx::_fmxcpt &_x ) throw ();
    fmx::BinaryData FMX_API *FM_BinaryData_Constructor3 ( const fmx::Text &name, fmx::uint32 amount, void *buffer, fmx::_fmxcpt &_x ) throw ();
    fmx::BinaryData FMX_API *FM_BinaryData_operatorAS ( void *_self, const fmx::BinaryData &source, fmx::_fmxcpt &_x ) throw ();
    bool FMX_API FM_BinaryData_operatorEQ ( const void *_self, const fmx::BinaryData &compareData, fmx::_fmxcpt &_x ) throw ();
    bool FMX_API FM_BinaryData_operatorNE ( const void *_self, const fmx::BinaryData &compareData, fmx::_fmxcpt &_x ) throw ();
	fmx::int32 FMX_API FM_BinaryData_GetCount ( const void *_self, fmx::_fmxcpt &_x ) throw ();
    fmx::int32 FMX_API FM_BinaryData_GetIndex ( const void *_self, const fmx::QuadChar& dataType, fmx::_fmxcpt &_x ) throw ();
    fmx::uint32 FMX_API FM_BinaryData_GetTotalSize ( const void *_self, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_BinaryData_GetType ( const void *_self, fmx::int32 index, fmx::QuadChar& dataType, fmx::_fmxcpt &_x ) throw ();
    fmx::uint32 FMX_API FM_BinaryData_GetSize ( const void *_self, fmx::int32 index, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_BinaryData_GetData ( const void *_self, fmx::int32 index, fmx::uint32 offset, fmx::uint32 amount, void *buffer, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_BinaryData_Add ( void *_self, const fmx::QuadChar& dataType, fmx::uint32 amount, void *buffer, fmx::_fmxcpt &_x ) throw ();
    bool FMX_API FM_BinaryData_Remove ( void *_self, const fmx::QuadChar& dataType, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_BinaryData_RemoveAll ( void *_self, fmx::_fmxcpt &_x ) throw ();
    void FMX_API FM_BinaryData_Delete ( void *_self, fmx::_fmxcpt &_x ) throw ();

    fmx::errcode FMX_API FM_BinaryData_GetFNAMData ( const void *_self, fmx::Text &filepathlist, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_BinaryData_AddFNAMData ( void *_self, const fmx::Text &filepathlist, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_BinaryData_GetSIZEData ( const void *_self, short &width, short &height, fmx::_fmxcpt &_x ) throw ();
    fmx::errcode FMX_API FM_BinaryData_AddSIZEData ( void *_self, short width, short height, fmx::_fmxcpt &_x ) throw ();
}


extern "C++"
{
    namespace fmx
    {
        inline BinaryData &BinaryData::operator = ( const BinaryData &source )
        {
            _fmxcpt _x;
            BinaryData *_rtn = FM_BinaryData_operatorAS ( this, source, _x );
            _x.Check ();
            return *_rtn;
        }
        inline bool BinaryData::operator == ( const BinaryData &compareData ) const
        {
            _fmxcpt _x;
            bool _rtn = FM_BinaryData_operatorEQ ( this, compareData, _x );
            _x.Check ();
            return _rtn;
        }
        inline bool BinaryData::operator != ( const BinaryData &compareData ) const
        {
            _fmxcpt _x;
            bool _rtn = FM_BinaryData_operatorNE ( this, compareData, _x );
            _x.Check ();
            return _rtn;
        }
        inline int32 BinaryData::GetCount () const
        {
            _fmxcpt _x;
            int32 _rtn = FM_BinaryData_GetCount ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline int32 BinaryData::GetIndex ( const QuadChar& dataType ) const
        {
            _fmxcpt _x;
            int32 _rtn = FM_BinaryData_GetIndex ( this, dataType, _x );
            _x.Check ();
            return _rtn;
        }
        inline uint32 BinaryData::GetTotalSize () const
        {
            _fmxcpt _x;
            uint32 _rtn = FM_BinaryData_GetTotalSize ( this, _x );
            _x.Check ();
            return _rtn;
        }
        inline void BinaryData::GetType ( int32 index, QuadChar& dataType ) const
        {
            _fmxcpt _x;
            FM_BinaryData_GetType ( this, index, dataType, _x );
            _x.Check ();
        }
        inline uint32 BinaryData::GetSize ( int32 index ) const
        {
            _fmxcpt _x;
            uint32 _rtn = FM_BinaryData_GetSize ( this, index, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode BinaryData::GetData ( int32 index, uint32 offset, uint32 amount, void *buffer ) const
        {
            _fmxcpt _x;
            errcode _rtn = FM_BinaryData_GetData ( this, index, offset, amount, buffer, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode BinaryData::Add ( const QuadChar& dataType, uint32 amount, void *buffer )
        {
            _fmxcpt _x;
            errcode _rtn = FM_BinaryData_Add ( this, dataType, amount, buffer, _x );
            _x.Check ();
            return _rtn;
        }
        inline bool BinaryData::Remove ( const QuadChar& dataType )
        {
            _fmxcpt _x;
            bool _rtn = FM_BinaryData_Remove ( this, dataType, _x );
            _x.Check ();
            return _rtn;
        }
        inline void BinaryData::RemoveAll ()
        {
            _fmxcpt _x;
            FM_BinaryData_RemoveAll ( this, _x );
            _x.Check ();
        }
        inline void BinaryData::operator delete ( void *obj )
        {
            _fmxcpt _x;
            FM_BinaryData_Delete ( obj, _x );
            _x.Check ();
        }

        inline BinaryDataAutoPtr::BinaryDataAutoPtr ()
        {
            _fmxcpt _x;
            FMX_AUTOPTRRESET ( FM_BinaryData_Constructor1 ( _x ) );
            _x.Check ();
        }
        inline BinaryDataAutoPtr::BinaryDataAutoPtr ( const BinaryData &sourceData )
        {
            _fmxcpt _x;
            FMX_AUTOPTRRESET ( FM_BinaryData_Constructor2 ( sourceData, _x ) );
            _x.Check ();
        }
        inline BinaryDataAutoPtr::BinaryDataAutoPtr ( const Text &name, uint32 amount, void *buffer )
        {
            _fmxcpt _x;
            FMX_AUTOPTRRESET ( FM_BinaryData_Constructor3 ( name, amount, buffer, _x ) );
            _x.Check ();
        }

        inline errcode BinaryData::GetFNAMData ( Text &filepathlist ) const
        {
            _fmxcpt _x;
            errcode _rtn = FM_BinaryData_GetFNAMData ( this, filepathlist, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode BinaryData::AddFNAMData ( const Text &filepathlist )
        {
            _fmxcpt _x;
            errcode _rtn = FM_BinaryData_AddFNAMData ( this, filepathlist, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode BinaryData::GetSIZEData ( short &width, short &height ) const
        {
            _fmxcpt _x;
            errcode _rtn = FM_BinaryData_GetSIZEData ( this, width, height, _x );
            _x.Check ();
            return _rtn;
        }
        inline errcode BinaryData::AddSIZEData ( short width, short height )
        {
            _fmxcpt _x;
            errcode _rtn = FM_BinaryData_AddSIZEData ( this, width, height, _x );
            _x.Check ();
            return _rtn;
        }
    }
}


#endif /* _h_BinaryData_ */

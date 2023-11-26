////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Mitov.h>
#include <SPI.h>
#include <Mitov_MicroSDCard.h>

//#if defined( VISUINO_ANY_ESP32 )
//    #define private public
//    #include <SD.h>
//    #undef private
//#else
//    #include <SdFat.h>
//#endif

#ifdef VISUINO_ANY_ESP32
	#include "vfs_api.h"
	#include "sd_diskio.h"
	#include "ff.h"
	#include "FS.h"
#endif

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

#ifdef VISUINO_ANY_ESP32
	typedef fs::SDFS SDClass;
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_ErrorInfoOutputPin,
		typename T_FRunning,
		typename T_FailedOutputPin,
		typename T_SuccessOutputPin,
		typename T_TYPE
	> class MicroSDCardBasic :
		public T_Enabled,
		public T_FailedOutputPin,
		public T_FRunning,
		public T_SuccessOutputPin,
		public T_ErrorInfoOutputPin
	{
	public:
		_V_PIN_( FailedOutputPin )
		_V_PIN_( SuccessOutputPin )
		_V_PIN_( ErrorInfoOutputPin )

	public:
		_V_PROP_( Enabled )

	public:
		_V_PROP_( FRunning )

	public:
		bool DoExists( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

//			Serial.println( "COOL1" );
//			Serial.println( APathName );
			return FCard.exists( APathName );
		}

		bool DoCreate( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return FCard.mkdir( APathName );
		}

		bool DoRemoveDir( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return FCard.rmdir( APathName );
		}

		bool DoRemoveFile( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return FCard.remove( APathName );
		}

		bool DoOpen( Mitov::String APathName, MITOV_FILE_MODE mode, File &AFile )
		{
//			Serial.print( "O1 :" );
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			if( ! APathName.startsWith( "/" ))
				APathName = Mitov::String( "/" ) + APathName;

//			char *ATmp = new char[ APathName.length() + 1 ];
//			memcpy( ATmp, APathName.c_str(), APathName.length() + 1 );

//			Serial.print( "T1 :" );
//			Serial.println( ATmp );
			AFile = FCard.open( APathName.c_str(), mode );
//			AFile = FCard->open( "/LOG.TXT", mode );
//			delete [] ATmp;

//			Serial.println( AFile );

			return AFile;
		}

	public:
#if defined( VISUINO_ANY_ESP32 ) || defined( VISUINO_TEENSY )
		T_TYPE	&FCard;
#else
		T_TYPE	FCard;
#endif

	public:
		inline void SendErrorInfo( const char *AMessage )
		{
			T_ErrorInfoOutputPin::SetPinValue( AMessage );
		}


#if defined( VISUINO_ANY_ESP32 ) || defined( VISUINO_TEENSY )
	public:
		inline MicroSDCardBasic() :
			FCard( SD )
		{
		}
#endif

	};
//---------------------------------------------------------------------------
	template <
		int PIN_NUMBER,
		typename T_Enabled,
		typename T_ErrorInfoOutputPin,
		typename T_FRunning,
		typename T_FailedOutputPin,
		typename T_SuccessOutputPin,
		typename T_TYPE
	> class MicroSDCard :
	    public MicroSDCardBasic<
			T_Enabled,
			T_ErrorInfoOutputPin,
			T_FRunning,
			T_FailedOutputPin,
			T_SuccessOutputPin,
			T_TYPE
		>
	{
		typedef MicroSDCardBasic<
			T_Enabled,
			T_ErrorInfoOutputPin,
			T_FRunning,
			T_FailedOutputPin,
			T_SuccessOutputPin,
			T_TYPE
		>	inherited;

	public:
		void UpdateEnabled()
		{
			if( inherited::Enabled() )
			{
#ifdef VISUINO_M5STACK_CORE2
				inherited::FRunning() = inherited::FCard.begin( PIN_NUMBER, SPI, 40000000 );
#else // VISUINO_M5STACK_CORE2
				inherited::FRunning() = inherited::FCard.begin( PIN_NUMBER );
#endif // VISUINO_M5STACK_CORE2
				if( inherited::FRunning() )
					T_SuccessOutputPin::ClockPin();

				else
				{
//					Serial.println( "FAIL" );
					T_FailedOutputPin::ClockPin();
#ifdef _MICROSD_USE_ERROR_INFO_PIN_
					T_ErrorInfoOutputPin::SetPinValue( "Fail to open microSD" );
#endif
					inherited::Enabled() = false;
					UpdateEnabled();
				}
//				Serial.println( "COOL" );
			}

			else
			{
#ifndef VISUINO_TEENSY
				inherited::FCard.end();
#endif
			}
		}

	public:
		inline void SystemInit()
		{
			UpdateEnabled();
		}

	};
//---------------------------------------------------------------------------
#if defined( VISUINO_ANY_ESP32 )
	template <
		typename T_SPI, T_SPI &C_SPI,
		int PIN_NUMBER,
		typename T_Enabled,
		typename T_ErrorInfoOutputPin,
		typename T_FRunning,
		typename T_FailedOutputPin,
		typename T_SuccessOutputPin,
		typename T_TYPE
	> class MicroSDCardCustom :
		public T_FailedOutputPin,
		public T_SuccessOutputPin,
		public T_ErrorInfoOutputPin,
		public T_FRunning,
		public T_Enabled
	{
	public:
		_V_PIN_( FailedOutputPin )
		_V_PIN_( SuccessOutputPin )
		_V_PIN_( ErrorInfoOutputPin )

	public:
		_V_PROP_( Enabled )

	public:
		_V_PROP_( FRunning )

	public:
		bool DoExists( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

//			Serial.println( "COOL1" );
//			Serial.println( APathName );
			return FCard.exists( APathName );
		}

		bool DoCreate( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return FCard.mkdir( APathName );
		}

		bool DoRemoveDir( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return FCard.rmdir( APathName );
		}

		bool DoRemoveFile( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return FCard.remove( APathName );
		}

		bool DoOpen( Mitov::String APathName, MITOV_FILE_MODE mode, File &AFile )
		{
//			Serial.print( "O1 :" );
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			if( ! APathName.startsWith( "/" ))
				APathName = Mitov::String( "/" ) + APathName;

//			char *ATmp = new char[ APathName.length() + 1 ];
//			memcpy( ATmp, APathName.c_str(), APathName.length() + 1 );

//			Serial.print( "T1 :" );
//			Serial.println( ATmp );
			AFile = FCard.open( APathName, mode );
//			AFile = FCard->open( "/LOG.TXT", mode );
//			delete [] ATmp;

//			Serial.println( AFile );

			return AFile;
		}

	public:
		T_TYPE	FCard;

	public:
		void UpdateEnabled()
		{
			if( Enabled() )
			{
//				Serial.println( "TEST1" );
#if defined( VISUINO_ANY_ESP32 )
				FRunning() = FCard.begin( PIN_NUMBER, C_SPI );
#else
				FRunning() = FCard.begin( PIN_NUMBER );
#endif
				if( FRunning() )
					T_SuccessOutputPin::ClockPin();

				else
				{
//					Serial.println( "FAIL" );
					T_FailedOutputPin::ClockPin();
#ifdef _MICROSD_USE_ERROR_INFO_PIN_
					T_ErrorInfoOutputPin::SetPinValue( "Fail to open microSD" );
#endif
					Enabled() = false;
					UpdateEnabled();
				}
//				Serial.println( "COOL" );
			}
			else
			{
				FCard.end();
//				delete FCard;
//				FCard = nullptr;
			}
		}

	public:
		inline void SystemInit()
		{
			UpdateEnabled();
		}

	public:
		MicroSDCardCustom() :
			FCard( FSImplPtr( new VFSImpl()) )
		{
		}
/*
	public:
		virtual ~MicroSDCard()
		{
			if( FCard )
				delete FCard;
		}
*/
	};
//---------------------------------------------------------------------------
#endif // defined( VISUINO_ANY_ESP32 )
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

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
#include <SD_MMC.h>
#include <Mitov_MicroSDCard.h>


#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_ErrorInfoOutputPin,
		typename T_FRunning,
		typename T_FailedOutputPin,
		typename T_SuccessOutputPin
	> class MicroSD_MMCCard :
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
		inline uint32_t GetAvailableSize()
		{
			return SD_MMC.totalBytes();
		}

		inline uint32_t GetCardSize()
		{
			return SD_MMC.cardSize();
		}

		inline uint32_t GetUsedSize()
		{
			return SD_MMC.usedBytes();
		}

	public:
		bool DoExists( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

//			Serial.println( "COOL1" );
//			Serial.println( APathName );
			return SD_MMC.exists( APathName );
		}

		bool DoCreate( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return SD_MMC.mkdir( APathName );
		}

		bool DoRemoveDir( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return SD_MMC.rmdir( APathName );
		}

		bool DoRemoveFile( Mitov::String APathName )
		{
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			return SD_MMC.remove( APathName );
		}

		bool DoOpen( Mitov::String APathName, MITOV_FILE_MODE mode, File &AFile )
		{
//			Serial.println( "TEST1" );
			if( ! ( FRunning().GetValue() && Enabled().GetValue() ) )
				return false;

			if( ! APathName.startsWith( "/" ))
				APathName = Mitov::String( "/" ) + APathName;

			AFile = SD_MMC.open( APathName, mode );

//			Serial.println( AFile );

			return AFile;
		}

	public:
		void UpdateEnabled()
		{
			if( Enabled() )
			{
//				Serial.println( "TEST1" );
				FRunning() = SD_MMC.begin();
//				Serial.println( "TEST1" );
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
				SD_MMC.end();

		}

	public:
		inline void SystemInit()
		{
			UpdateEnabled();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

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

#define private public
#include <SD.h>
#undef private

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_AvailableOutputPin,
		typename T_CardOutputPin,
		typename T_UsedOutputPin
	> class TArduinoMicroSDSize :
		public T_AvailableOutputPin,
		public T_CardOutputPin,
		public T_UsedOutputPin
	{
	public:
		_V_PIN_( AvailableOutputPin )
		_V_PIN_( CardOutputPin )
		_V_PIN_( UsedOutputPin )

	protected:
		inline void ReadValues()
		{
			if( ! C_OWNER.FRunning().GetValue() )
				return;

			if( T_AvailableOutputPin::GetPinIsConnected() )
				T_AvailableOutputPin::SetPinValue( C_OWNER.GetAvailableSize() );

			if( T_CardOutputPin::GetPinIsConnected() )
				T_CardOutputPin::SetPinValue( C_OWNER.GetCardSize() );

			if( T_UsedOutputPin::GetPinIsConnected() )
				T_UsedOutputPin::SetPinValue( C_OWNER.GetUsedSize() );

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ReadValues();
		}

	public:
		inline void SystemStart()
		{
			ReadValues();
		}

		inline void SystemLoopBegin()
		{
			ReadValues();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OutputPin
	> class MicroSDCardElementOperation :
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_CheckExists,
		typename T_Create,
		typename T_PathName,
		typename T_Remove
	> class MicroSDCardElementDirectory :
		public T_CheckExists,
		public T_Create,
		public T_Remove,
		public T_PathName
	{
	public:
		_V_PROP_( PathName )

	public:
		_V_PROP_( CheckExists )
		_V_PROP_( Create )
		_V_PROP_( Remove )

	public:
		inline bool DoExists( Mitov::String APathName )
		{
			return C_OWNER.DoExists( JoinPath( APathName ));
		}

		inline bool DoCreate( Mitov::String APathName )
		{
			return C_OWNER.DoCreate( JoinPath( APathName ));
		}

		inline bool DoRemoveDir( Mitov::String APathName )
		{
			return C_OWNER.DoRemoveDir( JoinPath( APathName ));
		}

		inline bool DoRemoveFile( Mitov::String APathName )
		{
			return C_OWNER.DoRemoveFile( JoinPath( APathName ));
		}

		inline bool DoOpen( Mitov::String APathName, MITOV_FILE_MODE mode, File &AFile )
		{
			return C_OWNER.DoOpen( JoinPath( APathName ), mode, AFile );
		}

	protected:
		Mitov::String JoinPath( Mitov::String AValue )
		{
			Mitov::String APath = PathName();
			if( ! APath.endsWith( "/" ) )
				APath = APath + "/";

			if( AValue.startsWith( "/" ) )
				AValue.remove( 0, 1 );

			return APath + AValue;
		}

	public:
		void CheckExists_ClockInputPin_o_Receive( void *_Data )
		{
			CheckExists().SetPinValue( C_OWNER.DoExists( PathName() ));
		}

		void Create_ClockInputPin_o_Receive( void *_Data )
		{
			Create().SetPinValue( C_OWNER.DoCreate( PathName() ));
		}

		void Remove_ClockInputPin_o_Receive( void *_Data )
		{
			Remove().SetPinValue( C_OWNER.DoRemoveDir( PathName() ));
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FailedOutputPin,
		typename T_OutputPin,
		typename T_PathName,
		typename T_Recursive
	> class MicroSDCardElementList :
		public T_OutputPin,
		public T_FailedOutputPin,
		public T_Recursive,
		public T_PathName
	{
	public:
		_V_PIN_( OutputPin )
//#ifdef MICROSD_LIST_USE_FAILED_OUTPUT_PIN
		_V_PIN_( FailedOutputPin )
//#endif

	public:
		_V_PROP_( PathName )
		_V_PROP_( Recursive )

	protected:
		void ProcessLevel( Mitov::String APath, Mitov::String APrefix )
		{
			File AFile;
			if( ! C_OWNER.DoOpen( APath, FILE_READ, AFile ))
			{
//#ifdef MICROSD_LIST_USE_FAILED_OUTPUT_PIN
				T_FailedOutputPin::ClockPin();
//#endif
#ifdef _MICROSD_USE_ERROR_INFO_PIN_
				C_OWNER.SendErrorInfo( "Missing Path" );
#endif
				return;
			}

			if( APath.endsWith( "/" ))
				APath = APath.substring( 0, APath.length() - 1 );

#if defined( VISUINO_ANY_ESP32 )
			delay( 0 );
#endif

			while (true)
			{
				File AEntry =  AFile.openNextFile();
				if( ! AEntry )
				  // no more files
				  break;

				Mitov::String AItemName = APrefix + AEntry.name();
				T_OutputPin::SetPinValue( AItemName.c_str() );

				if( Recursive() )
					if( AEntry.isDirectory())
						ProcessLevel( APath + '/' + AEntry.name(), AItemName + '/' );

				AEntry.close();
			  }

			AFile.close();
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
//			Serial.println( "" );
//			Serial.println( PathName );
//			Serial.println( "" );
			if( PathName() == "" )
				ProcessLevel( "/", "" );

			else
				ProcessLevel( PathName(), "" );
/*
			File AFile;
			if( ! C_OWNER.DoOpen( PathName, FILE_READ, AFile ))
			{
				FailedOutputPin.Notify( nullptr );
				return;
			}

			while (true)
			{
				File AEntry =  AFile.openNextFile();
				if (! AEntry)
				  // no more files
				  break;

				OutputPin.Notify( AEntry.name() );
				AEntry.close();
			  }

			AFile.close();
*/
		}

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Append,
		typename T_CacheSize,
		typename T_Enabled,
		typename T_FCurrentPosition,
		typename T_FHasWrite,
		typename T_FInWriteMode,
		typename T_FPositionStored,
		typename T_FReadWritePosition,
		typename T_FWriteCount,
//		typename T_FReadMoved,
//		typename T_FWriteMoved,
		typename T_KeepClosed,
		typename T_KeepFlushed,
		typename T_NewLine,
		typename T_PathName,
		typename T_ReadPositionOutputPin,
		typename T_Remove,
		typename T_Size,
		typename T_WritePositionOutputPin
	> class MicroSDCardElementFile :
		public T_ReadPositionOutputPin,
		public T_WritePositionOutputPin,
		public T_Append,
		public T_CacheSize,
		public T_Enabled,
		public T_Remove,
		public T_Size,
		public T_NewLine,
		public T_KeepFlushed,
		public T_KeepClosed,
		public T_PathName,
		public T_FInWriteMode,
		public T_FPositionStored,
		public T_FReadWritePosition,
		public T_FWriteCount,
//		public T_FReadMoved,
//		public T_FWriteMoved,
		public T_FCurrentPosition,
		public T_FHasWrite
	{
	public:
		_V_PIN_( ReadPositionOutputPin )
		_V_PIN_( WritePositionOutputPin )

	public:
		_V_PROP_( Append )
		_V_PROP_( PathName )
		_V_PROP_( CacheSize )
		_V_PROP_( Enabled )
		_V_PROP_( NewLine )
		_V_PROP_( KeepFlushed )
		_V_PROP_( KeepClosed )
		_V_PROP_( Size )
		_V_PROP_( Remove )

	public:
		_V_PROP_( FHasWrite )

	protected:
//		_V_PROP_( FReadMoved )
//		_V_PROP_( FWriteMoved )

		_V_PROP_( FWriteCount )
		_V_PROP_( FInWriteMode )
		_V_PROP_( FPositionStored )
		_V_PROP_( FCurrentPosition )
		_V_PROP_( FReadWritePosition )

//		uint32_t	FWritePosition = 0;
//		uint32_t	FReadPosition = 0;

	public:
		File FFile;

	public:
/*
		inline void SetHasWrite()
		{
			FHasWrite() = true;
		}
*/
		inline int Read()
		{
			if( ! TryOpen() )
				return -1;

			SwitchToReadWriteMode( false );
			return FFile.read();
		}

		inline int Peek()
		{
			if( ! TryOpen() )
				return -1;

			SwitchToReadWriteMode( false );
			return FFile.peek();
		}

		inline size_t ReadBytes( uint8_t *AData, size_t ASize )
		{
			if( ! TryOpen() )
				return 0;

//			Serial.println( "ReadBytes" );
			SwitchToReadWriteMode( false );

//			Serial.print( "POS: " );
//			Serial.println( FFile.position() );

#if( defined( VISUINO_ANY_ESP32 ) || defined( VISUINO_ESP8266 ) || defined( VISUINO_TEENSY ))
			int AResult = FFile.readBytes( (char *)AData, ASize );
#else
			int AResult = FFile.readBytes( (uint8_t *)AData, ASize );
#endif
//			Serial.print( "POS1: " );
//			Serial.println( FFile.position() );
			return AResult;
		}

		inline bool DataAvailable()
		{
			if( ! TryOpen() )
				return false;

			SwitchToReadWriteMode( false );
			return FFile.available();
		}

	public:
		void PrintChar( char AValue )
		{
			if( ! TryOpen() )
				return;

			SwitchToReadWriteMode( true );
			FFile.print( AValue );
			if( AValue == 10 )
				UpdateWrite();

		}

		template <typename T> void Print( T AValue )
		{
			if( ! TryOpen() )
				return;

			SwitchToReadWriteMode( true );

//			Serial.println( "TEST2" );
//			Serial.println( AValue );

//			Serial.println( FFile.position() );

			if( NewLine() )
				FFile.println( AValue );

			else
				FFile.print( AValue );

//			Serial.println( FFile.position() );

			UpdateWrite();
		}

		void Write( void *_Data, size_t ASize )
		{
			if( ! TryOpen() )
				return;

//			Serial.println( "TEST1" );

			SwitchToReadWriteMode( true );
			FFile.write( (uint8_t *)_Data, ASize );
			FWriteCount() = FWriteCount().GetValue() + ASize;
			if( FWriteCount().GetValue() > CacheSize().GetValue() )
			{
				FWriteCount() = 0;
				UpdateWrite();
			}

			else
				ReportWrite();

		}

		void BeginRead()
		{
//			Serial.println( "BeginRead()" );
			FWriteCount() = 0;
			UpdateWrite();

			SwitchToReadWriteMode( false );
		}

//		inline void ResetReadPosition()
//		{
//			FFile.seek( FReadPosition().GetValue() );
//		}

		inline void SeekRead( uint32_t AValue )
		{
			if( FInWriteMode() )
				FReadWritePosition() = AValue;

			else
				FFile.seek( AValue );

			T_ReadPositionOutputPin::SetPinValue( AValue );
		}

		inline void SeekWrite( uint32_t AValue )
		{
			if( FInWriteMode() )
				FFile.seek( AValue );

			else
				FReadWritePosition() = AValue;

			T_WritePositionOutputPin::SetPinValue( AValue );
		}

	protected:
		void SwitchToReadWriteMode( bool AInWriteMode )
		{
			if( ! FHasWrite().GetValue() )
				return;

//			Serial.print( "SwitchToReadWriteMode ---- : " );
//			Serial.println( AInWriteMode );
			if( FInWriteMode().GetValue() == AInWriteMode )
				return;

//			Serial.print( "SwitchToReadWriteMode : " );
//			Serial.println( AInWriteMode );

			uint32_t APosition;
			if( FFile )
 				APosition = FFile.position();

			else
				APosition = FCurrentPosition();

			FFile.seek( FReadWritePosition().GetValue() );
			FReadWritePosition() = APosition;
			FInWriteMode() = AInWriteMode;
		}

		void ReportWrite()
		{
			if( T_WritePositionOutputPin::GetPinIsConnected() )
				T_WritePositionOutputPin::SetPinValue( FFile.position() ); // Read BEFORE Close!!!

		}

		void UpdateWrite()
		{
			ReportWrite();

//			Serial.println( "UpdateWrite()" );
//			Serial.println( FFile.position() );

			if( KeepClosed() )
			{
				if( ! FCurrentPosition().GetIsConstant() )
					if( FFile )
						FCurrentPosition() = FFile.position();

				FFile.close();
			}

			else if( KeepFlushed() )
				FFile.flush();

		}

	public:
		bool TryOpen()
		{
			if( FFile )
				return true;

//			Serial.println( "OPEN1" );
			if( ! Enabled() )
				return false;

			if( PathName().GetValue() == "" )
				return false;

//			Serial.println( PathName );
//			Serial.println( FHasWrite().GetValue() );
			bool AResult;
			if( FHasWrite() )
				AResult = C_OWNER.DoOpen( PathName(), FILE_WRITE, FFile );

			else
				AResult = C_OWNER.DoOpen( PathName(), FILE_READ, FFile );

			if( ! FPositionStored().GetValue() )
			{
				FCurrentPosition() = FFile.size();
				FPositionStored() = true;
			}

//			Serial.println( "GET:" );
//			Serial.println( FCurrentPosition() );

//			Serial.println( FFile.position() );

			FFile.seek( FCurrentPosition());

//			Serial.println( FFile.position() );

			return AResult;
		}

	public:
		inline void CloseInputPin_o_Receive( void *_Data )
		{
			if( ! FCurrentPosition().GetIsConstant() )
				if( FFile )
					FCurrentPosition() = FFile.position();

			FFile.close();
		}

		inline void FlushInputPin_o_Receive( void *_Data )
		{
			FFile.flush();
		}

		inline void Size_ClockInputPin_o_Receive( void *_Data )
		{
//#ifdef MicroSDCardElementFile_Size
			Size().OutputPin().SetPinValue( FFile.size() );
//#endif
		}

		inline void Remove_ClockInputPin_o_Receive( void *_Data )
		{
			Remove().SetPinValue( C_OWNER.DoRemoveFile( PathName() ));
		}

	public:
		inline MicroSDCardElementFile() // bool AHasWrite )
		{
//			FHasWrite() = AHasWrite;
			FInWriteMode() = false;
			FWriteCount() = 0;
			FReadWritePosition() = 0;
			FCurrentPosition() = 0;
			FPositionStored() = false;
//			FReadMoved() = false;
//			FWriteMoved() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_OutputPin,
		typename T_PathName
	> class MicroSDCardElementExists :
		public T_OutputPin,
		public T_PathName
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( PathName )

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			T_OutputPin::SetPinValue( C_OWNER.DoExists( PathName() ));
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

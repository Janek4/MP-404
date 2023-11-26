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
#include <Mitov_StandardSerial.h>

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_SERIAL_TYPE, T_SERIAL_TYPE &C_SERIAL,
		typename T_AfterSendingDelay,
		uint16_t C_DIRECTION_PIN,
		typename T_DataBits,
		typename T_Enabled,
		typename T_FEndTime,
		typename T_FSending,
		typename T_OutputPin,
		typename T_Parity,
		typename T_SendingOutputPin,
		typename T_Speed,
		typename T_StopBits
	> class Serial_RS485_Port : public BasicSerialWrapper<T_SERIAL_TYPE, C_SERIAL, T_Enabled, T_OutputPin>,
		public T_AfterSendingDelay,
		public T_DataBits,
		public T_FEndTime,
		public T_FSending,
		public T_Parity,
		public T_SendingOutputPin,
		public T_Speed,
		public T_StopBits
	{
		typedef BasicSerialWrapper<T_SERIAL_TYPE, C_SERIAL, T_Enabled, T_OutputPin> inherited;

	public:
		_V_PIN_( SendingOutputPin )

	public:
		_V_PROP_( Speed )

	public:
//		_V_PROP_( Enabled )
		_V_PROP_( AfterSendingDelay )

	protected:
		_V_PROP_( FSending )

	public:
//#ifndef VISUINO_CHIPKIT
		_V_PROP_( Parity )
		_V_PROP_( StopBits )
		_V_PROP_( DataBits )
//#endif // VISUINO_CHIPKIT

	protected:
		_V_PROP_( FEndTime )
//#ifdef _MITOV_SERIAL_SENDING_PIN_
//		uint32_t	FEndTime;
//#endif

/*
#ifdef VISUINO_ESP8266
		bool	ReceiveEnabled : 1;
		bool	TransmitEnabled : 1;
		bool	UseAlternativePins : 1;

#endif // VISUINO_ESP8266
*/
	#ifndef VISUINO_CHIPKIT
		uint32_t GetByteSpeed()
		{
			return Speed().GetValue() / ( StopBits().GetValue() + DataBits().GetValue() + 1 );
		}
	#endif // VISUINO_CHIPKIT

	public:
		void ResetSerial()
		{
			C_SERIAL.end();
			StartPort();
		}

	protected:
		inline void SerialEnd() {} // Placeholder

		void StartPort()
		{
#if ( defined( VISUINO_CHIPKIT ) || defined( VISUINO_STMDUINO ) || defined( VISUINO_INTEL_EDISON ) || defined( VISUINO_DIGISPARK_PRO ) || defined( VISUINO_DIGISPARK ) || defined( NO_SERIAL_FORMAT ) || defined( VISUINO_K210 ) )
			C_SERIAL.begin( Speed() );

#else // VISUINO_CHIPKIT

			int AIndex = ((int)Parity()) * 8 + ( StopBits() - 1 ) * 4 + ( DataBits() - 5);
			C_SERIAL.begin( Speed(), CSerialInits[ AIndex ] );
/*
#ifdef VISUINO_ESP8266
			if( ReceiveEnabled && TransmitEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_FULL );

			else if( ReceiveEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_RX_ONLY );

			else if( TransmitEnabled )
				C_SERIAL.begin( Speed, CSerialInits[ AIndex ], SERIAL_TX_ONLY );

			if( UseAlternativePins )
				C_SERIAL.swap();

#else // VISUINO_ESP8266
*/
#ifdef VISUINO_MACCHINA_M2

			C_SERIAL.begin( Speed(), UARTClass::UARTModes( CSerialInits[ AIndex ] ));

#else
#endif // VISUINO_MACCHINA_M2
//#endif // VISUINO_ESP8266

#endif // VISUINO_CHIPKIT

#ifdef VISUINO_ESP32
			volatile bool ADummy = false; // Force the library to build properly!
			if( ADummy )
				C_SERIAL.println(); // Work around ESP32 library bug

			C_SERIAL.flush();
#endif // VISUINO_ESP32
		}


/*
#ifdef _MITOV_SERIAL_SENDING_PIN_
		virtual uint32_t GetByteSpeed()
		{
			return 9600 / 11;
		}
#endif
*/
	protected:
//	#if ( defined( VISUINO_CONTROLLINO_MAXI ) || defined( VISUINO_CONTROLLINO_MEGA ) || defined( VISUINO_M_DUINO_ETHERNET_58 ))
//		virtual
//	#endif
		inline void UpdateSending( bool ASending )
		{
//			T_PARENT::UpdateSending( ASending );
            Digital.Write( C_DIRECTION_PIN, ASending );
			T_SendingOutputPin::SetPinValue( ASending );
		}

		void SendingCount( uint32_t ACount )
		{
//			Serial.println( ACount );

			ACount = 1; // Most conrtrollers can send only one character at a time.
#ifdef __AVR
			ACount = 1; // The AVR can send only one character at a time.
#endif
			if( ! FSending() )
			{
				FSending() = true;
				UpdateSending( FSending() );
				uint32_t ATime = micros();
				FEndTime() = ATime + AfterSendingDelay() + ACount * ( 10000000 / GetByteSpeed() );
//				Serial.println( 10000000 / GetByteSpeed() );
//				Serial.println( int32_t( FEndTime - ATime ));
			}

			else
			{
				UpdateSending( FSending() );
				uint32_t ATime = micros();
				if( int32_t( FEndTime().GetValue() - ATime ) >= AfterSendingDelay() )
					FEndTime() = FEndTime().GetValue() + ACount * ( 10000000 / GetByteSpeed() );

				else
					FEndTime() = ATime + AfterSendingDelay() + ACount * ( 10000000 / GetByteSpeed() );

//					Serial.println( 10000000 / GetByteSpeed() );
//					Serial.println( int32_t( FEndTime - ATime ));
			}

//			Serial.println( 10000000 / GetByteSpeed() );
//			Serial.println( int32_t( FEndTime - ATime ));
		}

	public:
		template <typename T> void Print( T AValue )
		{
			if( inherited::Enabled() )
			{
				SendingCount( 1 );
				C_SERIAL.println( AValue );
#ifdef __AVR
//				SendingCount( 1 );
#endif
			}
		}

		void Print( Mitov::String AValue )
		{
			if( inherited::Enabled() )
			{
				SendingCount( AValue.length() );
				C_SERIAL.println( AValue );
#ifdef _MITOV_SERIAL_SENDING_PIN_
#ifdef __AVR
//				SendingCount( 1 );
#endif
#endif
			}
		}

		void Print( char *AValue )
		{
			if( inherited::Enabled() )
			{
				SendingCount( strlen( AValue ) );
				C_SERIAL.println( AValue );
#ifdef __AVR
//				SendingCount( 1 );
#endif
			}
		}

		void PrintChar( char AValue )
		{
			if( inherited::Enabled() )
			{
				SendingCount( 1 );
				C_SERIAL.print( AValue );
			}
		}

		size_t Write( uint8_t *AData, uint32_t ASize )
		{
			if( ! inherited::Enabled().GetValue() )
				return 0;

			SendingCount( ASize );
			return C_SERIAL.write( AData, ASize );
		}

	    size_t Write(uint8_t byte)
		{
			if( ! inherited::Enabled().GetValue() )
				return 0;

			SendingCount( 1 );
			return C_SERIAL.write( byte );
		}

		inline int AvailableForWrite()
		{
			return C_SERIAL.availableForWrite();
		}

	public:
		inline void SystemLoopBegin()
		{
			if( FSending() )
			{
				unsigned long ACurrentMicros = micros();
				if( int32_t( FEndTime().GetValue() - ACurrentMicros ) < 0 )
				{
//					Serial.print( "STOP: " );
//					Serial.println( int32_t( FEndTime - ACurrentMicros ) );
					FSending() = false;
					UpdateSending( FSending() );
				}
			}

		}

		inline void SystemInit()
		{
			if( inherited::Enabled() )
				StartPort();

		}

	public:
        void RestartPort() // Used by setters
		{
			if( ! inherited::Enabled() )
				return;

			C_SERIAL.end();
			StartPort();
		}

	public:
		void UpdateEnabled()
		{
			if( inherited::Enabled() )
				StartPort();

			else
			{
				C_SERIAL.end();
				SerialEnd();
			}
		}

	};
//---------------------------------------------------------------------------
} // Mitov

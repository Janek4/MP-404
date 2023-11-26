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

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	enum TArduinoRangerBasicUnits { rdCm, rdInch };
//---------------------------------------------------------------------------
	template <
		typename T_I2C, T_I2C &C_I2C,
		typename T_Address,
		typename T_Enabled,
		typename T_FClocked,
		typename T_FProcessing,
		typename T_OutOfRangeOutputPin,
		typename T_OutputPin,
		typename T_Units
	> class TArduinoM5StackUltrasonicRangerUnit :
		public T_Address,
		public T_Enabled,
		public T_FClocked,
		public T_FProcessing,
		public T_OutOfRangeOutputPin,
		public T_OutputPin,
		public T_Units
	{
	public:
		_V_PIN_( OutOfRangeOutputPin )
		_V_PIN_( OutputPin )

	public:
	    _V_PROP_( Address )
		_V_PROP_( Enabled )
		_V_PROP_( Units )

	protected:
		_V_PROP_( FClocked )
		_V_PROP_( FProcessing )

	protected:
		unsigned long	FStartTime;

	public:
		inline void SystemStart()
		{
			T_OutOfRangeOutputPin::SetPinValue( false, false );
		}

		inline void SystemLoopBegin()
		{
			if( ! Enabled() )
				return;

			if( FProcessing() )
			{
			    if( millis() - FStartTime < 120 )
    			    return;

				FProcessing() = false;

                C_I2C.requestFrom( uint8_t( Address().GetValue() ), uint8_t( 3 ));  // Request 3 bytes from Ultrasonic Unit.

                uint32_t AData = C_I2C.read();
                AData <<= 8;
                AData |= C_I2C.read();
                AData <<= 8;
                AData |= C_I2C.read();

                float AValue = float( AData ) / 10000;

                if( Units() == rdInch )
                    AValue /= 2.54;

                T_OutputPin::SetPinValue( AValue );
			    return;
			}

			if( FClocked() )
			{
			    FClocked() = false;

                C_I2C.beginTransmission( Address().GetValue() );
                C_I2C.write( 0x01 );
                C_I2C.endTransmission();

                FStartTime = millis();
                FProcessing() = true;
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			FClocked() = true;
		}

	public:
		TArduinoM5StackUltrasonicRangerUnit()
		{
			FClocked() = false;
			FProcessing() = false;
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
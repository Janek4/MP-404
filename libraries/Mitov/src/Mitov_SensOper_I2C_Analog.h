////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Mitov_TexasInstruments_ADS1X1X.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
    template <
        typename T_OWNER, T_OWNER &C_OWNER,
        uint8_t C_INDEX,
        uint8_t C_OFFSET,
        typename T_OutputPin
    > class TArduinoESP32_SensOper_ADS1115AnalogChannel :
        public T_OutputPin
    {
	public:
		_V_PIN_( OutputPin )

	public:
		inline void GetIndex( uint8_t &AInIndex, uint8_t & AOutIndex )
        {
            if( ! AInIndex )
				AOutIndex = C_INDEX - C_OFFSET;

			-- AInIndex;
		}

		inline void SetValue( float AValue )
		{
//            AValue = Func::MapRange<float>( AValue, 0.1629, 0.8134, 4, 20 );

			T_OutputPin::SetPinValue( AValue );
		}

    };
//---------------------------------------------------------------------------
    template <
        typename T_OWNER, T_OWNER &C_OWNER,
        uint8_t C_Address,
        typename T_FIndex,
        uint8_t C_OFFSET
    > class TArduinoESP32_SensOper_ADS1115Analog :
        public T_FIndex
    {
	protected:
		_V_PROP_( FIndex )

	protected:
		void writeRegister( uint8_t AReg, uint16_t AValue )
		{
			Wire.beginTransmission( C_Address );
			Wire.write( uint8_t( AReg ));
			Wire.write( uint8_t( AValue >> 8 ));
			Wire.write( uint8_t( AValue & 0xFF ));
			Wire.endTransmission();
		}

		int16_t readRegister( uint8_t AReg )
		{
			Wire.beginTransmission( C_Address );
			Wire.write( AReg );
			Wire.endTransmission();
			Wire.requestFrom( C_Address, uint8_t( 2 ));
			return ((int16_t( int8_t( Wire.read() )) << 8) | Wire.read());
		}

	protected:
		void StartRead()
		{
    	    uint8_t AChannelIndex = C_OWNER.Analog_GetIndexCall( FIndex().GetValue() );


	        uint16_t AValue =	0x8000 | // Start Single Shot
//								( uint16_t( 0b010 ) << 9 ) | // Gain +/-2.048V
								( uint16_t( 0b000 ) << 9 ) | // Gain +/-6.144V
//								( Comparator().Latching() ? ( uint16_t( 1 ) << 2 ) : 0 ) |
//								( Comparator().ActiveHigh() ? ( uint16_t( 1 ) << 3 ) : 0 ) |
//								( Comparator().WindowMode() ? ( uint16_t( 1 ) << 4 ) : 0 ) |
//								( uint16_t( 0b100 ) << 5 ) | // 128 SPS
								( uint16_t( 0b000 ) << 5 ) | // 8 SPS
								( uint16_t( 1 ) << 8 ) | // Single shot
								( uint16_t( 1 ) << 14 ) | // Single ended
								0b11 | // No comparer
								( uint16_t( AChannelIndex ) << 12 );

		    writeRegister( ADS1X1XConst::REG_POINTER_CONFIG, AValue );
		}

	public:
		inline void SystemStart()
		{
			FIndex() = 0;
			StartRead();
		}

		inline void SystemLoopBegin()
		{
		    uint16_t AStatus = readRegister( ADS1X1XConst::REG_POINTER_CONFIG );
//			Serial.println( AStatus, HEX );
			if(( AStatus & 0x8000 ) == 0 )
				return;

			int16_t	AValue = readRegister( ADS1X1XConst::REG_POINTER_CONVERT ); // >> 4;
//			Serial.println( AValue );

			float AFloatValue = Func::MapRange<float>( float( AValue ), 12851.0, 24542.0, 0.0, 1.0 ); // / 0x7FFF;

//			Serial.println( AValue );

            uint8_t AChannelIndex = C_OWNER.Analog_GetIndexCall( FIndex().GetValue() );
//            Serial.println( AChannelIndex );

			C_OWNER.Analog_SetValue( AChannelIndex + C_OFFSET, AFloatValue );

    	    FIndex() = FIndex() + 1;
    	    if( FIndex() >= C_OWNER.GetAnalogChannelsCount() )
    	        FIndex() = 0;

			StartRead();
		}

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

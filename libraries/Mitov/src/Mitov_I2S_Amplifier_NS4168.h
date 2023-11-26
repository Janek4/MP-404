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
	template <
        uint32_t MAX_SPEED,
		typename T_RightChannel
    > class TArduinoI2SAmplifierNS4168 :
		public T_RightChannel
    {
    public:
		_V_PROP_( RightChannel )

    public:
		inline void ControlInputPin_o_Receive( void *_Data )
		{
		    TI2SInfo &AInfo = *(TI2SInfo *)_Data;

		    AInfo.Output.Used = true;

//            Serial.println( "TEST1" );
//            delay( 10000 );

		    for( int i = 0; i < AInfo.Output.Modes.Size; ++ i )
		    {
                if( ! AInfo.Output.Modes.Data[ i ].Enabled )
                    continue;

                if( AInfo.Output.Modes.Data[ i ].Mode == TI2SMode::Standard )
                {
                    AInfo.Output.Modes.Data[ i ].SetBitsPerSample( 12, 12 );
                    AInfo.Output.Modes.Data[ i ].SetSampleRate( 8000, MAX_SPEED );

                    if( RightChannel() )
                        AInfo.Output.Modes.Data[ i ].ChannelsMap |= 0b10;

                    else
                        AInfo.Output.Modes.Data[ i ].ChannelsMap |= 0b01;

                }

		    }

//		    if( AInfo )

//            Serial.println( "TEST2" );
//            delay( 1000 );
		}

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
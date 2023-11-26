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
        typename T_RightChannel
    > class TArduinoI2SMicrophoneSPH0645LM4H :
        public T_RightChannel
    {
    public:
        _V_PROP_( RightChannel )

    public:
		inline void ControlInputPin_o_Receive( void *_Data )
		{
		    TI2SInfo &AInfo = *(TI2SInfo *)_Data;

		    AInfo.Input.Used = true;

		    for( int i = 0; i < AInfo.Input.Modes.Size; ++ i )
		    {
                if( AInfo.Input.Modes.Data[ i ].Mode != TI2SMode::Standard )
                    AInfo.Input.Modes.Data[ i ].Enabled = false;

                else
                {
                    AInfo.Input.Modes.Data[ i ].SetBitsPerSample( 8, 32 );
                    AInfo.Input.Modes.Data[ i ].SetSampleRate( 8000, 192000 );
                    if( RightChannel() )
                        AInfo.Input.Modes.Data[ i ].ChannelsMap |= 0b10;

                    else
                        AInfo.Input.Modes.Data[ i ].ChannelsMap |= 0b01;

                }
            }
		}

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
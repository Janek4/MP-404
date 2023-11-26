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
    namespace TArduinoI2SAmplifierMAX98357Mode
    {
        enum TArduinoI2SAmplifierMAX98357Mode
        {
            Left,
            Right,
            Merge
        };
    }
//---------------------------------------------------------------------------
	template <
	    typename T_Channel,
	    typename T_Enabled
    > class TArduinoI2SAmplifierMAX98357TDMMode :
	    public T_Channel,
	    public T_Enabled
    {
    public:
		_V_PROP_( Channel )
		_V_PROP_( Enabled )

    };
//---------------------------------------------------------------------------
	template <
		typename T_I2SMode,
		typename T_TDMMode
    > class TArduinoI2SAmplifierMAX98357 :
		public T_I2SMode,
		public T_TDMMode
    {
    public:
		_V_PROP_( I2SMode )
		_V_PROP_( TDMMode )

    public:
		inline void ControlInputPin_o_Receive( void *_Data )
		{
		    TI2SInfo &AInfo = *(TI2SInfo *)_Data;

		    AInfo.Output.Used = true;

		    for( int i = 0; i < AInfo.Output.Modes.Size; ++ i )
		    {
                if( ! AInfo.Output.Modes.Data[ i ].Enabled )
                    continue;

                if( AInfo.Output.Modes.Data[ i ].Mode == TI2SMode::Standard )
                {
                    AInfo.Output.Modes.Data[ i ].SetBitsPerSample( 16, 32 );
                    AInfo.Output.Modes.Data[ i ].SetSampleRate( 8000, 96000 );

/*
                    if( AInfo.Output.Modes.Data[ i ].MinSampleRate < 8000 )
                        AInfo.Output.Modes.Data[ i ].MinSampleRate = 8000;

                    if( AInfo.Output.Modes.Data[ i ].MaxSampleRate > 96000 )
                        AInfo.Output.Modes.Data[ i ].MaxSampleRate = 96000;

                    uint8_t ABits[] = { 32, 24, 16 };
                    AInfo.Output.Modes.Data[ i ].RestictBits( ABits, 3 );
*/
                    AInfo.Output.Modes.Data[ i ].ChannelsMap |= uint8_t( I2SMode().GetValue() ) + 1;
                }

                else if( ( AInfo.Output.Modes.Data[ i ].Mode == TI2SMode::TDM ) && TDMMode().Enabled() )
                {
                    AInfo.Output.Modes.Data[ i ].SetBitsPerSample( 16, 32 );
                    AInfo.Output.Modes.Data[ i ].SetSampleRate( 8000, 96000 );
/*
                    if( AInfo.Output.Modes.Data[ i ].MinSampleRate < 8000 )
                        AInfo.Output.Modes.Data[ i ].MinSampleRate = 8000;

                    if( AInfo.Output.Modes.Data[ i ].MaxSampleRate > 96000 )
                        AInfo.Output.Modes.Data[ i ].MaxSampleRate = 96000;

                    uint8_t ABits[] = { 32, 16 };
                    AInfo.Output.Modes.Data[ i ].RestictBits( ABits, 2 );
*/
                    AInfo.Output.Modes.Data[ i ].ChannelsMap |= ( 1 << TDMMode().Channel().GetValue() );
                }

                else
                    AInfo.Output.Modes.Data[ i ].Enabled = false;

		    }

//		    if( AInfo )


		}

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
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
//#include <WiFi.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#define Console UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Enabled,
		typename T_SignalStrengthOutputPin
	> class TArduinoNetworkWiFiSignalStrengthOperation :
		public T_Enabled,
		public T_SignalStrengthOutputPin
	{
	public:
		_V_PIN_( SignalStrengthOutputPin )

	public:
		_V_PROP_( Enabled )

	public:
		inline void TryStart() {}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			if( Enabled() )
				if( C_OWNER.FIsStarted() )
					if( T_SignalStrengthOutputPin::GetPinIsConnected() )
					{
						int32_t ASetrength = WiFi.RSSI();
						T_SignalStrengthOutputPin::SetPinValue( ASetrength );
					}

		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#undef Console
#endif


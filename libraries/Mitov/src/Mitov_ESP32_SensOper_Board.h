////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
    template <
        typename T_Analog_GetIndex,
        typename T_Analog_SetValue,
        uint8_t C_COUNT_Analog
    > class TArduinoESP32_SensOper_Board
    {
	public:
		uint8_t	GetAnalogChannelsCount() { return C_COUNT_Analog; }

		uint8_t	Analog_GetIndexCall( uint8_t AInIndex )
		{
			uint8_t	AOutIndex = 255;
			T_Analog_GetIndex::Call( AInIndex, AOutIndex );
			return AOutIndex;
		}

		void Analog_SetValue( int32_t AIndex, float AValue )
		{
			T_Analog_SetValue::Call( AIndex, AValue );
		}

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

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
namespace SensOper_ButtonMap
{
    struct TMapItem
    {
        uint16_t    Input;
        uint8_t     Buttons;
    };

    const TMapItem C_Map[] =
    {
        { 0,    0b000 },
        { 754,  0b100 },
        { 1878, 0b010 },
        { 2436, 0b110 },
        { 2436, 0b110 },
        { 2952, 0b001 },
        { 3336, 0b101 },
        { 3429, 0b011 },
        { 3508, 0b111 }
    };
/*
    const TMapItem C_Map[] =
    {
        { 0,    0b000 },
        { 754,  0b100 },
        { 1878, 0b010 },
        { 2436, 0b110 },
        { 2436, 0b110 },
        { 2952, 0b001 },
        { 3336, 0b101 },
        { 3329, 0b011 },
        { 3400, 0b111 }
    };
*/
}
//---------------------------------------------------------------------------
	template <
	    uint8_t C_PIN,
		typename T_OutputPins_Up,
		typename T_OutputPins_Down,
		typename T_OutputPins_Select
	> class TArduinoESP32_SensOper_ButtonsModule :
		public T_OutputPins_Up,
		public T_OutputPins_Down,
		public T_OutputPins_Select
	{
	public:
		_V_PIN_( OutputPins_Up )
		_V_PIN_( OutputPins_Down )
		_V_PIN_( OutputPins_Select )

	protected:
        void ReadSensor()
        {
            uint16_t AValue = analogRead( C_PIN );

//            Serial.println( AValue );

            uint8_t AIndexPart = ( sizeof( SensOper_ButtonMap::C_Map ) / sizeof( SensOper_ButtonMap::C_Map[ 0 ] ) ) / 2;

            uint8_t AIndex = 0;

            while( AIndexPart )
            {
                if( AValue > SensOper_ButtonMap::C_Map[ AIndex + AIndexPart ].Input )
                    AIndex += AIndexPart;

                AIndexPart >>= 1;
            }

			for(;;)
			{
				if( AIndex < ( sizeof( SensOper_ButtonMap::C_Map ) / sizeof( SensOper_ButtonMap::C_Map[ 0 ] ) - 1 ) )
					if( AValue > SensOper_ButtonMap::C_Map[ AIndex + 1 ].Input )
					{
						++ AIndex;
						continue;
					}

				break;
			}

            uint8_t AButtons = SensOper_ButtonMap::C_Map[ AIndex ].Buttons;

//            Serial.println( AIndex );
//            Serial.println( AButtons, BIN );
//            Serial.println( "" );
//            return;

            T_OutputPins_Up::SetPinValue( ( AButtons & 0b100 ) != 0 );
            T_OutputPins_Down::SetPinValue( ( AButtons & 0b010 ) != 0 );
            T_OutputPins_Select::SetPinValue( ( AButtons & 0b001 ) != 0 );
        }

	public:
		inline void SystemStart()
		{
		    ReadSensor();
		}

		inline void SystemLoopBegin()
		{
		    ReadSensor();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif


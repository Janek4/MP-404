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
		typename T_Enabled,
		typename T_InvalidOutputPin,
		typename T_OutputPin
	> class Text_MACAddress_ToUInt64 :
		public T_Enabled,
		public T_InvalidOutputPin,
		public T_OutputPin
	{
	public:
		_V_PROP_( Enabled )

	public:
		_V_PIN_( InvalidOutputPin )
		_V_PIN_( OutputPin )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			char * AText = (char*)_Data;

			char *pos = AText;
//            uint8_t octets[ 4 ];
			uint64_t AAddress = 0;
			for (int i = 0; i < 6; i++)
			{
				char *endptr;
				uint64_t octet = strtol( pos, &endptr, 16 );

				if ( octet < 0 || octet > 255 || endptr == pos || ( *endptr != '-' && *endptr != '\0' ) )
				{
					T_InvalidOutputPin::ClockPin();
					return;
				}

//				octets[i] = (int) octet;
				AAddress <<= 8;
				AAddress |= octet;
				pos = endptr + 1;
			}

			T_OutputPin::SetPinValue( AAddress );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_OutputPin
	> class UInt64_MACAddress_ToText :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PROP_( Enabled )

	public:
		_V_PIN_( OutputPin )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			uint64_t AValue = *(uint64_t *)_Data;

			uint8_t AOctets[ 6 ];
			for (int i = 0; i < 6; i++)
			{
				AOctets[ i ] = AValue;
				AValue >>= 8;
			}

			char AText[ 3 * 6 ]; // Includes '-' and '\0'

			sprintf( AText, "%02X-%02X-%02X-%02X-%02X-%02X", AOctets[ 5 ], AOctets[ 4 ], AOctets[ 3 ], AOctets[ 2 ], AOctets[ 1 ], AOctets[ 0 ] );
			T_OutputPin::SetPinValue( AText );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_OctetsOutputPins_SetPinValue
	> class UInt64_MACAddress_ToOctets :
		public T_Enabled
	{
	public:
		_V_PROP_( Enabled )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			uint64_t AValue = *(uint64_t *)_Data;
			for (int i = 0; i < 6; i++)
			{
				T_OctetsOutputPins_SetPinValue::Call( 5 - i, AValue & 0xFF );
				AValue >>= 8;
			}
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_InitialValue,
		typename T_OutputPin
	> class Octets_MACAddress_ToUInt64 :
		public T_Enabled,
		public T_InitialValue,
		public T_OutputPin
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialValue )

	public:
		_V_PIN_( OutputPin )

	public:
		inline void OctetsInputPins_o_Receive( uint8_t AIndex, void *_Data )
		{
			uint64_t AValue = ( *(uint32_t *)_Data );
			if( AValue > 255 )
                AValue = 255;

			uint64_t AInitialValue = InitialValue();

            uint8_t AShift = ( 5 - AIndex ) * 8;
			AInitialValue &= ~( 0xFF << AShift );
            AInitialValue |= AValue << AShift;

			InitialValue() = AInitialValue;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			T_OutputPin::SetPinValue( InitialValue() );
		}

	public:
		inline void SystemStart()
		{
			T_OutputPin::SetPinValue( InitialValue() );
		}

		inline void SystemLoopEnd()
		{
			T_OutputPin::SetPinValue( InitialValue() );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
        typename T_FValue,
		typename T_InitialValue,
		typename T_OutputPin
	> class Octets_MACAddress_ToText :
		public T_Enabled,
        public T_FValue,
		public T_InitialValue,
		public T_OutputPin
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialValue )

	public:
		_V_PIN_( OutputPin )

	protected:
		_V_PIN_( FValue )

	protected:
		void SendOutput()
		{
			char AText[ 3 * 6 ]; // Includes '-' and '\0'

			if( FValue().GetIsConstant() )
				sprintf( AText, "%02X-%02X-%02X-%02X-%02X-%02X", uint32_t( InitialValue().Octet1() ), uint32_t( InitialValue().Octet2() ), uint32_t( InitialValue().Octet3() ), uint32_t( InitialValue().Octet4() ), uint32_t( InitialValue().Octet5() ), uint32_t( InitialValue().Octet6() ) );

			else
			{
				uint64_t AValue = FValue();
				uint8_t AOctets[ 6 ];
				for (int i = 0; i < 6; i++)
				{
					AOctets[ i ] = AValue;
					AValue >>= 8;
				}

				sprintf( AText, "%02X-%02X-%02X-%02X-%02X-%02X", AOctets[ 5 ], AOctets[ 4 ], AOctets[ 3 ], AOctets[ 2 ], AOctets[ 1 ], AOctets[ 0 ] );
			}

			T_OutputPin::SetPinValue( AText );
		}

	public:
		inline void OctetsInputPins_o_Receive( uint8_t AIndex, void *_Data )
		{
			uint64_t AValue = ( *(uint32_t *)_Data );
			if( AValue > 255 )
				AValue = 255;

			uint8_t AOffset = ( 5 - AIndex ) * 8;

			uint64_t AIntValue = FValue();

			AIntValue &= ~ uint64_t( uint64_t( 0xFF ) << AOffset );
            AIntValue |= AValue << AOffset;

            FValue() = AIntValue;
		}

		inline void ClockInputPin_o_Receive( void *_Data )
		{
			SendOutput();
		}

	public:
		inline void SystemInit()
		{
			FValue() = InitialValue().GetUInt64Value();
		}

		inline void SystemStart()
		{
			SendOutput();
		}

		inline void SystemLoopEnd()
		{
			SendOutput();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
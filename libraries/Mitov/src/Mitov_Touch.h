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
		typename T_CountOutputPin,
		typename T_EmptyOutputPin,
		typename T_Enabled,
		typename T_IDOutputPin,
		typename T_IndexOutputPin,
		typename T_PressedOutputPin,
		typename T_XOutputPin,
		typename T_YOutputPin
	> class TArduinoParseTouch :
		public T_CountOutputPin,
		public T_EmptyOutputPin,
		public T_Enabled,
		public T_IDOutputPin,
		public T_IndexOutputPin,
		public T_PressedOutputPin,
		public T_XOutputPin,
		public T_YOutputPin
	{
	public:
		_V_PIN_( CountOutputPin )
		_V_PIN_( EmptyOutputPin )
		_V_PIN_( IDOutputPin )
		_V_PIN_( IndexOutputPin )
		_V_PIN_( PressedOutputPin )
		_V_PIN_( XOutputPin )
		_V_PIN_( YOutputPin )

	public:
		_V_PROP_( Enabled )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			TArduinoTouchInfo &ATouch = *((TArduinoTouchInfo *)_Data );

			T_CountOutputPin::SetPinValue( ATouch.Size );
			if( ! ATouch.Size )
				T_EmptyOutputPin::ClockPin();

			else
			{
				for( int i = 0; i < ATouch.Size; ++ i )
				{
					T_IndexOutputPin::SetPinValue( i );
					T_IDOutputPin::SetPinValue( ATouch.Data[ i ].ID );
					T_XOutputPin::SetPinValue( ATouch.Data[ i ].X );
					T_YOutputPin::SetPinValue( ATouch.Data[ i ].Y );
					T_PressedOutputPin::SetPinValue( ATouch.Data[ i ].Status != TTouchStatus::Up );
				}
            }
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_ID,
		typename T_PressedOutputPin,
		typename T_XOutputPin,
		typename T_YOutputPin
	> class TArduinTouchIDData :
		public T_Enabled,
		public T_ID,
		public T_PressedOutputPin,
		public T_XOutputPin,
		public T_YOutputPin
	{
	public:
		_V_PIN_( PressedOutputPin )
		_V_PIN_( XOutputPin )
		_V_PIN_( YOutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( ID )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			TArduinoTouchInfo &ATouch = *((TArduinoTouchInfo *)_Data );

			for( int i = 0; i < ATouch.Size; ++ i )
				if( ATouch.Data[ i ].ID == ID() )
				{
					T_XOutputPin::SetPinValue( ATouch.Data[ i ].X );
					T_YOutputPin::SetPinValue( ATouch.Data[ i ].Y );
					T_PressedOutputPin::SetPinValue( ATouch.Data[ i ].Status != TTouchStatus::Up );
				}

		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif


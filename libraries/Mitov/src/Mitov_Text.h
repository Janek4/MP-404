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
		typename T_Elements_TryActive,
		typename T_FStarted,
		typename T_OutputPin,
		typename T_Value
	> class TextValue :
		public T_FStarted,
		public T_OutputPin,
		public T_Value
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Value )

	protected:
		_V_PROP_( FStarted )

	public:
		inline void UpdateValue()
		{
			T_OutputPin::SetPinValue( Value() );
		}

		void ExternalSetValue( Mitov::String AValue, bool AFromState )
		{
			if( Value().GetValue() == AValue )
				return;

			if( ! FStarted().GetValue() )
			{
				FStarted() = true;
				T_OutputPin::SetPinValue( Value() );
			}

			Value() = AValue;
			T_OutputPin::SetPinValue( AValue );
		}
/*
		inline void SetValue( Mitov::String AValue )
		{
			ExternalSetValue( AValue );
		}
*/
	public:
		inline void SystemStart()
		{
			if( ! FStarted().GetValue() )
				T_OutputPin::SetPinValue( Value() );

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			T_OutputPin::SetPinValue( Value() );
		}

	public:
		inline TextValue()
		{
            FStarted() = false;
        }

	};
//---------------------------------------------------------------------------
/*
	class BindableTextValue : public TextValue
	{
		typedef TextValue inherited;

	protected:
		Mitov::String OldValue = inherited::Value;

	public:
		inline void SystemInit()
		{
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin()
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		BindableTextValue( char *AValue ) :
			inherited( AValue ),
			OldValue( AValue )
		{
		}

	};
*/
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Value
	> class ArduinoSetTextValueElement :
		public T_Value
	{
	public:
		_V_PROP_( Value )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_OWNER.ExternalSetValue( Value(), false );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FInputValue,
		typename T_Value
	> class ArduinoStateSetTextValueElement :
		public T_FInputValue,
		public T_Value
	{
	public:
		_V_PROP_( Value )

	protected:
		_V_PROP_( FInputValue )

	public:
		void TryActive( bool &AResult )
		{
			if( FInputValue() )
			{
				C_OWNER.ExternalSetValue( Value(), true );
				AResult = true;
			}
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FInputValue() == *(bool *)_Data )
				return;

			FInputValue() = *(bool *)_Data;
			if( FInputValue() )
				C_OWNER.ExternalSetValue( Value(), true );

			else
				C_OWNER.ResetValue();
		}

	public:
		ArduinoStateSetTextValueElement()
		{
			FInputValue() = false;
		}
	};
//---------------------------------------------------------------------------
	template <
		typename T_Elements_TryActive,
		typename T_FStarted,
		typename T_FStateActive,
		typename T_OutputPin,
		typename T_Value
	> class TextValueState :
		public T_FStarted,
		public T_FStateActive,
		public T_OutputPin,
		public T_Value
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Value )

	public:
		_V_PROP_( FStarted )
		_V_PROP_( FStateActive )

	protected:
		Mitov::String FCurrentValue;

	public:
		void SetValue( Mitov::String AValue )
		{
			if( Value().GetValue() == AValue )
				return;

			Value() = AValue;
			if( FStateActive() )
				return;

			T_OutputPin::SetPinValue( AValue );
		}

		void ExternalSetValue( Mitov::String AValue, bool AFromState )
		{
			if( FCurrentValue == AValue )
				return;

			if( ! FStarted().GetValue() )
			{
				FStarted() = true;
				T_OutputPin::SetPinValue( Value() );
			}

			FCurrentValue = AValue;
			if( AFromState )
				FStateActive() = true;

			T_OutputPin::SetPinValue( FCurrentValue );
		}

		void ResetValue()
		{
//			for( int i = FElements.size(); i -- ; )
//				if( FElements[ i ]->TryActive( this ) )
//					return;

			bool AResult = false;
			T_Elements_TryActive::Call( AResult );
			if( AResult )
				return;

			FStateActive() = false;
			FCurrentValue = Value();
			T_OutputPin::SetPinValue( FCurrentValue );
		}

	public:
		inline void SystemStart()
		{
			if( ! FStarted().GetValue() )
			{
				FCurrentValue = Value();
				T_OutputPin::SetPinValue( Value() );
			}
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			T_OutputPin::SetPinValue( FCurrentValue );
		}

	public:
		inline TextValueState()
		{
			FStateActive() = false;
		}
	};
//---------------------------------------------------------------------------
/*
	class BindableTextStateValue : public TextStateValue
	{
		typedef TextValue inherited;

	protected:
		Mitov::String OldValue;

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			OldValue = inherited::Value;
		}

		inline void SystemLoopBegin()
		{
			if( inherited::Value == OldValue )
				return;

			OldValue = inherited::Value;
			ResetValue();
//			inherited::OutputPin.Notify( (void *)OldValue.c_str() );
		}

	public:
		using inherited::inherited;

	};
*/
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_OutputPin
	> class TextLength :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			int32_t AValue = strlen( (char *)_Data );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_IgnoreCase,
		typename T_OutputPin,
		typename T_Text
	> class FindTextPosition :
		public T_Enabled,
		public T_IgnoreCase,
		public T_OutputPin,
		public T_Text
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( IgnoreCase )
		_V_PROP_( Text )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			Mitov::String AText = Text();
			Mitov::String ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase() )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.indexOf( AText );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_IgnoreCase,
		typename T_OutputPin,
		typename T_Text
	> class FindLastTextPosition :
		public T_Enabled,
		public T_IgnoreCase,
		public T_OutputPin,
		public T_Text
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( IgnoreCase )
		_V_PROP_( Text )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			Mitov::String AText = Text();
			Mitov::String ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase() )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			int32_t AValue = ATextValue.lastIndexOf( AText );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_IgnoreCase,
		typename T_OutputPin,
		typename T_Text
	> class ContainsText :
		public T_Enabled,
		public T_IgnoreCase,
		public T_OutputPin,
		public T_Text
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( IgnoreCase )
		_V_PROP_( Text )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			Mitov::String AText = Text();
			Mitov::String ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase() )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ( ATextValue.indexOf( AText ) >= 0 );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_IgnoreCase,
		typename T_OutputPin,
		typename T_Text
	> class TextStartsWith :
		public T_Enabled,
		public T_IgnoreCase,
		public T_OutputPin,
		public T_Text
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( IgnoreCase )
		_V_PROP_( Text )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			Mitov::String AText = Text();
			Mitov::String ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase() )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.startsWith( AText );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_IgnoreCase,
		typename T_OutputPin,
		typename T_Text
	> class TextEndsWith :
		public T_Enabled,
		public T_IgnoreCase,
		public T_OutputPin,
		public T_Text
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( IgnoreCase )
		_V_PROP_( Text )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			Mitov::String AText = Text();
			Mitov::String ATextValue = Mitov::String( (char *)_Data );

			if( IgnoreCase() )
			{
				AText.toUpperCase();
				ATextValue.toUpperCase();
			}

			bool AValue = ATextValue.endsWith( AText );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_FromValue,
//		typename T_IgnoreCase,
		typename T_OutputPin,
		typename T_ToValue
	> class ReplaceText :
		public T_Enabled,
		public T_FromValue,
//		public T_IgnoreCase,
		public T_OutputPin,
		public T_ToValue
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( FromValue )
		_V_PROP_( ToValue )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue = (char *)_Data;

			AValue.replace( FromValue(), ToValue() );

			T_OutputPin::SetPinValue( AValue, false );
        }
	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Length,
		typename T_OutputPin,
		typename T_Position,
		typename T_WrapAround
	> class SubText :
		public T_Enabled,
		public T_Length,
		public T_OutputPin,
		public T_Position,
		public T_WrapAround
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( Length )
		_V_PROP_( Position )
		_V_PROP_( WrapAround )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue;
			if( WrapAround() )
			{
				Mitov::String AInValue = Mitov::String( (char *)_Data );
				uint32_t AInLength = AInValue.length();
				uint32_t ALength = Length();
				uint32_t APosition = Position();
				while( ALength )
				{
					Mitov::String ASubValue = AInValue.substring( APosition, APosition + ALength );
					AValue += ASubValue;
					uint32_t ASublenght = ASubValue.length();
					APosition += ASublenght;
					ALength -= ASublenght;
					if( APosition >= AInLength )
						APosition -= AInLength;

				}
			}

			else
				AValue = Mitov::String( (char *)_Data ).substring( Position(), Position() + Length() );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Length,
		typename T_OutputPin
	> class LeftSubText :
		public T_Enabled,
		public T_Length,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( Length )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data ).substring( 0, Length() );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Length,
		typename T_OutputPin
	> class RightSubText :
		public T_Enabled,
		public T_Length,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( Length )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue = AValue.substring( AValue.length() - Length() );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Length,
		typename T_OutputPin,
		typename T_Position
	> class DeleteSubText :
		public T_Enabled,
		public T_Length,
		public T_OutputPin,
		public T_Position
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( Length )
		_V_PROP_( Position )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.remove( Position(), Length() );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Length,
		typename T_OutputPin
	> class DeleteLeftSubText :
		public T_Enabled,
		public T_Length,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( Length )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.remove( 0, Length() );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Length,
		typename T_OutputPin
	> class DeleteRightSubText :
		public T_Enabled,
		public T_Length,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
		_V_PROP_( Length )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
			{
				T_OutputPin::PinNotify( _Data );
                return;
			}

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.remove( AValue.length() - Length() );

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_OutputPin
	> class ToUpperCase :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
            {
				T_OutputPin::PinNotify( _Data );
                return;
            }

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.toUpperCase();

			T_OutputPin::SetPinValue( AValue, false );
        }
	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_OutputPin
	> class ToLowerCase :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
            {
				T_OutputPin::PinNotify( _Data );
                return;
            }

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.toLowerCase();

			T_OutputPin::SetPinValue( AValue, false );
        }
	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_OutputPin
	> class TrimText :
		public T_Enabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
            {
				T_OutputPin::PinNotify( _Data );
                return;
            }

			Mitov::String AValue = Mitov::String( (char *)_Data );
			AValue.trim();

			T_OutputPin::SetPinValue( AValue, false );
        }

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif


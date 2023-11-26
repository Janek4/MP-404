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
	template <
		typename T_FOutputStarted,
        typename T_FOutputValue,
		typename T_FPopulated,
		typename T_FValue,
		typename T_IgnoreSame,
		typename T_InitialEnabled,
		typename T_OutputPin,
		typename T_SendOnEnable,
		typename T_TYPE
	> class CommonSwitch :
		public T_FOutputStarted,
		public T_FOutputValue,
		public T_FPopulated,
		public T_FValue,
		public T_IgnoreSame,
		public T_InitialEnabled,
        public T_SendOnEnable,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialEnabled )
		_V_PROP_( IgnoreSame )
        _V_PROP_( SendOnEnable )

	protected:
		_V_PROP_( FPopulated )
		_V_PROP_( FOutputStarted )
		_V_PROP_( FOutputValue )
		_V_PROP_( FValue )

	protected:
		void SendOutput( T_TYPE AValue )
		{
			if( InitialEnabled() )
			{
				if( IgnoreSame() )
				{
					if( ! FOutputStarted() )
						FOutputStarted() = true;

					else if( FOutputValue().GetValue() == AValue )
						return;

				}

				FOutputStarted() = true;
				FOutputValue() = AValue;

				T_OutputPin::SetPinValue( AValue );
			}
		}

	public:
		inline void EnableInputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialEnabled() == AValue )
				return;

			InitialEnabled() = AValue;
			if( ! FPopulated().GetValue() )
				return;

            if( SendOnEnable() )
                SendOutput( FValue() );
            
		}

		inline void InputPin_o_Receive( void *_Data )
		{
//			FValue = *(T_TYPE *)_Data;
			T_TYPE AValue = ::Mitov::template ExtractValue<T_TYPE>( _Data );
			FValue() = AValue;
			FPopulated() = true;
			SendOutput( AValue );
		}

	public:
		inline CommonSwitch( T_TYPE AValue )
		{
			FOutputStarted() = false;
			FPopulated() = false;
			FValue() = AValue;
			FOutputValue() = AValue;
		}

		inline CommonSwitch()
		{
			FOutputStarted() = false;
			FPopulated() = false;
		}

	};
//---------------------------------------------------------------------------
/*
	template<> void CommonSwitch<Mitov::String>::InputPin_o_Receive( void *_Data )
	{
			FValue = (char *)_Data;
			FPopulated = true;
			SendOutput();
	}
*/
//---------------------------------------------------------------------------
	template <
		uint8_t C_INDEX,
		typename T_TYPE
	> class CommonSwitch_Value
	{
	public:
		inline T_TYPE GetValue() { return T_TYPE(); }
		inline void SetValue( const T_TYPE &AValue ) {}

	};
//---------------------------------------------------------------------------
	template <
		uint8_t C_INDEX,
		typename T_TYPE
	> class CommonSwitch_Value_Variable
	{
	protected:
		T_TYPE FValue;

	public:
		inline T_TYPE GetValue() { return FValue; }
		inline void SetValue( const T_TYPE &AValue ) { FValue = AValue; }

	};
//---------------------------------------------------------------------------
	template <
		typename T_0_IMPLEMENTATION_OUTPUT_VALUE,
		typename T_0_IMPLEMENTATION_VALUE,
		typename T_FOutputStarted,
		typename T_FPopulated,
		typename T_IgnoreSame,
		typename T_InitialEnabled,
		typename T_OutputPin,
		typename T_SendOnEnable,
		typename T_TYPE
	> class CommonSwitch_Object :
		public T_0_IMPLEMENTATION_OUTPUT_VALUE,
		public T_0_IMPLEMENTATION_VALUE,
		public T_FOutputStarted,
		public T_FPopulated,
		public T_IgnoreSame,
		public T_InitialEnabled,
        public T_SendOnEnable,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialEnabled )
		_V_PROP_( IgnoreSame )
        _V_PROP_( SendOnEnable )

	protected:
		_V_PROP_( FPopulated )
		_V_PROP_( FOutputStarted )

	protected:
		void SendOutput( T_TYPE AValue )
		{
			if( InitialEnabled() )
			{
				if( IgnoreSame() )
				{
					if( ! FOutputStarted() )
						FOutputStarted() = true;

					else if( T_0_IMPLEMENTATION_OUTPUT_VALUE::GetValue() == AValue )
						return;

				}

				FOutputStarted() = true;
				T_0_IMPLEMENTATION_OUTPUT_VALUE::SetValue( AValue );

				T_OutputPin::SetPinValue( AValue );
			}
		}

	public:
		inline void EnableInputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialEnabled() == AValue )
				return;

			InitialEnabled() = AValue;
			if( ! FPopulated().GetValue() )
				return;

            if( SendOnEnable() )
                SendOutput( T_0_IMPLEMENTATION_VALUE::GetValue() );

		}

		inline void InputPin_o_Receive( void *_Data )
		{
//			FValue = *(T_TYPE *)_Data;
			T_TYPE AValue = ::Mitov::template ExtractValue<T_TYPE>( _Data );
			T_0_IMPLEMENTATION_VALUE::SetValue( AValue );
			FPopulated() = true;
			SendOutput( AValue );
		}

	public:
		inline CommonSwitch_Object( T_TYPE AValue )
		{
			FOutputStarted() = false;
			FPopulated() = false;
			T_0_IMPLEMENTATION_VALUE::SetValue( AValue );
			T_0_IMPLEMENTATION_OUTPUT_VALUE::SetValue( AValue );
		}

		inline CommonSwitch_Object()
		{
			FOutputStarted() = false;
			FPopulated() = false;
		}

	};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
	template < 
		typename T_FOutputStarted,
		typename T_FOutputValue,
		typename T_FPopulated,
		typename T_FValue,
		typename T_IgnoreSame,
		typename T_InitialEnabled,
		typename T_OutputPin
	> class DigitalSwitch :
		public T_FOutputStarted,
		public T_FOutputValue,
		public T_FPopulated,
		public T_FValue,
		public T_IgnoreSame,
		public T_InitialEnabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialEnabled )
		_V_PROP_( IgnoreSame )

	protected:
		_V_PROP_( FPopulated )
		_V_PROP_( FOutputStarted )
		_V_PROP_( FOutputValue )
		_V_PROP_( FValue )

	protected:
		void SendOutput()
		{
			if( InitialEnabled() )
			{
				if( IgnoreSame() )
				{
					if( ! FOutputStarted() )
						FOutputStarted() = true;

					else
						if( FOutputValue() == FValue() )
							return;

				}

				FOutputStarted() = true;
				FOutputValue() = FValue();

				T_OutputPin::SetPinValue( FOutputValue() );
			}
		}

	public:
		void EnableInputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool *)_Data;
			if( InitialEnabled() == AValue )
				return;

			InitialEnabled() = AValue;
			if( ! FPopulated() )
				return;

			SendOutput();
		}

		void InputPin_o_Receive( void *_Data )
		{
			FValue() = *(bool *)_Data;
			FPopulated() = true;
			SendOutput();
		}

	public:
		inline DigitalSwitch()
		{
			FOutputStarted() = false;
			FPopulated() = false;
			FValue() = false;
			FOutputValue() = false;
		}

	};
//---------------------------------------------------------------------------
	template < 
		typename T_InitialEnabled,
		typename T_OutputPin
	> class ClockSwitch :
		public T_InitialEnabled,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( InitialEnabled )

	public:
		inline void EnableInputPin_o_Receive( void *_Data )
		{
			InitialEnabled() = *(bool *)_Data;
		}

		void InputPin_o_Receive( void *_Data )
		{
			if( InitialEnabled() )
				T_OutputPin::ClockPin();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
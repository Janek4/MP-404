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
		typename T_Text,
		typename T_ITEM, T_ITEM &C_ITEM
	> class TFormattedTextStringItemIndexed :
		public T_Text
	{
//	public:
//		_V_PROP_( Text )

	public:
		inline void Process( Mitov::String &AText )
		{
			AText += T_Text::GetValue() + C_ITEM.GetText();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Text
	> class TFormattedTextStringItemText :
		public T_Text
	{
	public:
		inline void Process( Mitov::String &AText )
		{
			AText += T_Text::GetValue();
		}

	};
//---------------------------------------------------------------------------
	template <
		bool C_Clock_IsConnected,
		uint16_t C_COUNT,
		typename T_Elements_CanProcess,
		typename T_Elements_Process,
		typename T_Elements_Processed,
		typename T_Enabled,
		typename T_FModified,
		typename T_OutputPin
	> class FormattedText_Fixed :
		public T_Enabled,
		public T_FModified,
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )

	public:
		_V_PROP_( FModified )

	protected:
		void ProcessSendOutput( bool ANeedsCanSend )
		{
			if( ! Enabled().GetValue() )
				return;

			bool ACanSend = true;
			T_Elements_CanProcess::Call( ACanSend );
			if( ! ACanSend )
				return;

			Mitov::String AText;
			T_Elements_Process::Call( AText );

//			Serial.println( AText );
			T_OutputPin::SetPinValue( AText.c_str() );
			FModified() = false;
			T_Elements_Processed::Call();
		}

	public:
		void SetModified()
		{
			FModified() = true;
			if( ! C_Clock_IsConnected )
				ProcessSendOutput( true );
		}

	public:
		inline void SystemStart()
		{
		}

		inline void SystemStartSend()
		{
			ProcessSendOutput( false );
		}

		inline void SystemLoopEnd()
		{
			if( FModified() )
				ProcessSendOutput( false );

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ProcessSendOutput( false );
		}

	public:
		FormattedText_Fixed()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		bool C_Clock_IsConnected,
		uint16_t C_COUNT,
		typename T_Elements_CanProcess,
		typename T_Elements_Process,
		typename T_Elements_Processed,
		typename T_Enabled,
		typename T_FModified,
		typename T_OutputPin,
		typename T_Text
	> class FormattedText :
		public T_Enabled,
		public T_FModified,
		public T_OutputPin,
		public T_Text
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( Text )

	public:
		_V_PROP_( FModified )

	protected:
		struct TStringItem
		{
			Mitov::String	Text;
			int8_t			Index;
		};

	public:
		void SetModified()
		{
			FModified() = true;
			if( ! C_Clock_IsConnected )
				ProcessSendOutput( true );

		}

	protected:
		Mitov::SimpleList<TStringItem>	FReadyElements;

	protected:
		void AddReadyElement( Mitov::String ATextItem, int16_t AIndex )
		{
//			Serial.print( "AddElement: " ); Serial.print( ATextItem ); Serial.println( AIndex );

			TStringItem	AItem;
			AItem.Text = ATextItem;
//			Serial.print( "AddElement: " ); Serial.println( AItem->Text );
			if( AIndex < C_COUNT )
				AItem.Index = AIndex;

			else
				AItem.Index = -1;

			FReadyElements.push_back( AItem );

//			Serial.println( FReadyElements[ FReadyElements.size() - 1 ]->Text );
//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	public:
		inline void InitElements( const String &AOldValue, const String &Value )
		{
		}

		void InitElements()
		{
			FReadyElements.clear();
			Mitov::String	ATextItem;
			Mitov::String	AIndexText;
			bool	AInEscape = false;

//			Serial.println( "INIT" );
//			Serial.println( Text );
//			delay( 1000 );

			uint32_t AAutoIndex = 0;

			for( unsigned int i = 0; i < Text().GetValue().length(); ++ i )
			{
				char AChar = Text().GetValue()[ i ];
				if( AInEscape )
				{
					if( AChar >= '0' && AChar <= '9' )
						AIndexText += AChar;

					else
					{
						if( AChar == '%' )
						{
							if( AIndexText.length() == 0 )
							{
								ATextItem += '%';
								AInEscape = false;
							}

							else
							{
								AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
								AIndexText = "";
							}

						}

						else
						{
							if( AIndexText.length() == 0 )
								AddReadyElement( ATextItem, AAutoIndex ++ );

							else
								AddReadyElement( ATextItem, AIndexText.toInt() );

							ATextItem = AChar;
						}

						if( AChar != '%' )
							AInEscape = false;
					}
				}

				else
				{
					if( AChar == '%' )
					{
						AInEscape = true;
						AIndexText = "";
					}

					else
						ATextItem += AChar;

				}

			}

			if( AInEscape )
				AddReadyElement( ATextItem, AIndexText.toInt() );

			else if( ATextItem.length() )
			{
				TStringItem	AItem;

				AItem.Text = ATextItem;
				AItem.Index = -1;

				FReadyElements.push_back( AItem );
			}

//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	protected:
		void ProcessSendOutput( bool ANeedsCanSend )
		{
			if( ! Enabled().GetValue() )
				return;

			bool ACanSend = true;
			T_Elements_CanProcess::Call( ACanSend );
			if( ! ACanSend )
				return;

//			Serial.println( "ProcessSendOutput" );
			Mitov::String AText;
			TStringItem *AElements = FReadyElements.AsPointer();
			for( int i = 0; i < FReadyElements.size(); ++i )
			{
				AText += AElements[ i ].Text;
//				if( T_Elements_Process::ChainHasItems() )
				if( C_COUNT > 0 )
					if( AElements[ i ].Index >= 0 )
						T_Elements_Process::Call( AElements[ i ].Index, AText );

			}

//			Serial.println( AText );
			T_OutputPin::SetPinValue( AText.c_str() );
			FModified() = false;
			T_Elements_Processed::Call();
		}
	public:
		inline void SystemLoopEnd()
		{
			if( FModified() )
				ProcessSendOutput( false );

		}

		inline void SystemStart()
		{
			InitElements();
		}

		inline void SystemStartSend()
		{
			ProcessSendOutput( false );
		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			ProcessSendOutput( false );
		}

	public:
		FormattedText()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FModified,
		typename T_FillCharacter,
		typename T_InitialValue,
		typename T_Length
	> class FormattedTextElementText :
		public T_FModified,
		public T_FillCharacter,
		public T_InitialValue,
		public T_Length
	{
	public:
		_V_PROP_( FillCharacter )
		_V_PROP_( InitialValue )
		_V_PROP_( Length )

	protected:
		_V_PROP_( FModified )

	public:
//		void SetValue( Mitov::String AValue )
		void SetValue( Mitov::String AValue )
		{
//			Serial.println( AValue );
			InitialValue() = AValue;
			FModified() = true;
			C_OWNER.SetModified();
//			FModified = true;
		}

	public:
		inline Mitov::String GetText()
		{
			return InitialValue();
		}

		inline void AppendText( Mitov::String &AText )
		{
			AText += InitialValue().GetValue();
		}

		inline void Processed()
		{
			FModified() = false;
		}

		inline void CanProcess( bool &AResult )
		{
			AResult &= FModified().GetValue();
		}

	public:
		inline void SystemStart()
		{
		}

	public:
		inline FormattedTextElementText()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <typename T> class FormattedTextLengthElementTyped : public T
	{
		typedef T inherited;

	public:
		inline Mitov::String GetText()
		{
			Mitov::String AResult = inherited::GetText();
			for( int i = AResult.length(); i < inherited::Length(); ++i )
				AResult = inherited::FillCharacter().GetValue() + AResult;

			return AResult;
		}

		inline void AppendText( Mitov::String &AText )
		{
			AText += GetText();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Base,
		typename T_FModified,
		typename T_FillCharacter,
		typename T_InitialValue,
		typename T_Length
	> class FormattedTextElementInteger :
		public T_Base,
		public T_FModified,
		public T_FillCharacter,
		public T_InitialValue,
		public T_Length
	{
	public:
		_V_PROP_( Base )
		_V_PROP_( FillCharacter )
		_V_PROP_( InitialValue )
		_V_PROP_( Length )

	protected:
		_V_PROP_( FModified )

	public:
		inline Mitov::String GetText()
		{
			char AText[ 16 ];
			ltoa( InitialValue(), AText, Base() );
			return AText;
		}

		inline void AppendText( Mitov::String &AText )
		{
			AText += GetText();
		}

		inline void Processed()
		{
			FModified() = false;
		}

		inline void CanProcess( bool &AResult )
		{
			AResult &= FModified().GetValue();
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			int32_t AValue = *(int32_t*)_Data;
			if( AValue == InitialValue() )
				return;

			InitialValue() = AValue;
			FModified() = true;
			C_OWNER.SetModified();
		}

	public:
		inline void SystemStart() // Placeholder for inherited classes
		{
		}

	public:
		inline FormattedTextElementInteger()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Base,
		typename T_FModified,
		typename T_FillCharacter,
		typename T_InitialValue,
		typename T_Length
	> class FormattedTextElementUnsigned :
		public T_Base,
		public T_FModified,
		public T_FillCharacter,
		public T_InitialValue,
		public T_Length
	{
	public:
		_V_PROP_( Base )
		_V_PROP_( FillCharacter )
		_V_PROP_( InitialValue )
		_V_PROP_( Length )

	protected:
		_V_PROP_( FModified )

	public:
		inline Mitov::String GetText()
		{
			char AText[ 16 ];
			ultoa( InitialValue(), AText, Base().GetValue() );
			return AText;
//			Serial.println( AText );
		}

		inline void AppendText( Mitov::String &AText )
		{
			AText += GetText();
		}

		inline void Processed()
		{
			FModified() = false;
		}

		inline void CanProcess( bool &AResult )
		{
			AResult &= FModified().GetValue();
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			uint32_t AValue = *(uint32_t*)_Data;
			//Serial.println( AValue );
			if( AValue == InitialValue() )
				return;

			InitialValue() = AValue;
			FModified() = true;
			//Serial.println( InitialValue().GetValue() );
			C_OWNER.SetModified();
		}

	public:
		inline void SystemStart() // Placeholder for inherited classes
		{
		}

	public:
		inline FormattedTextElementUnsigned()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FModified,
		typename T_FillCharacter,
		typename T_InitialValue,
		typename T_Length,
		typename T_MinWidth,
		typename T_Precision
	> class FormattedTextElementAnalog :
		public T_FModified,
		public T_FillCharacter,
		public T_InitialValue,
		public T_Length,
		public T_MinWidth,
		public T_Precision
	{
	public:
		_V_PROP_( FillCharacter )
		_V_PROP_( InitialValue )
		_V_PROP_( Length )
		_V_PROP_( MinWidth )
		_V_PROP_( Precision )

	protected:
		_V_PROP_( FModified )

	public:
		inline Mitov::String GetText()
		{
			char AText[ __VISUINO_FLOAT_TO_STR_LEN__ ];
			dtostrf( InitialValue(),  MinWidth(), Precision(), AText );
//			Serial.println( AText );

			return AText;
		}

		inline void AppendText( Mitov::String &AText )
		{
			AText += GetText();
		}

		inline void Processed()
		{
			FModified() = false;
		}

		inline void CanProcess( bool &AResult )
		{
			AResult &= FModified().GetValue();
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			float AValue = *(float*)_Data;
			if( AValue == InitialValue() )
				return;

			InitialValue() = AValue;
			FModified() = true;
			C_OWNER.SetModified();
		}

	public:
		inline void SystemStart() // Placeholder for inherited classes
		{
		}


	public:
		inline FormattedTextElementAnalog()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FModified,
		typename T_FalseValue,
		typename T_FillCharacter,
		typename T_InitialValue,
		typename T_Length,
		typename T_TrueValue
	> class FormattedTextElementDigital :
		public T_FModified,
		public T_FalseValue,
		public T_FillCharacter,
		public T_InitialValue,
		public T_Length,
		public T_TrueValue
	{
	public:
		_V_PROP_( FillCharacter )
		_V_PROP_( Length )
		_V_PROP_( TrueValue )
		_V_PROP_( FalseValue )
		_V_PROP_( InitialValue )

	protected:
		_V_PROP_( FModified )

	public:
		inline Mitov::String GetText()
		{
			if( InitialValue() )
				return TrueValue();

			return FalseValue();
		}

		inline void AppendText( Mitov::String &AText )
		{
			AText += GetText();
		}

		inline void Processed()
		{
			FModified() = false;
		}

		inline void CanProcess( bool &AResult )
		{
			AResult &= FModified().GetValue();
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			bool AValue = *(bool*)_Data;
			if( AValue == InitialValue() )
				return;

			InitialValue() = AValue;
			FModified() = true;
			C_OWNER.SetModified();
		}

	public:
		inline void SystemStart() // Placeholder for inherited classes
		{
		}

	public:
		inline FormattedTextElementDigital()
		{
			FModified() = false;
		}

	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD, typename T> class TextFormatElementInput
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( Mitov::String( *(T*)_Data ));
		}
	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD> class TextFormatElementInput<T_LCD, C_LCD, uint64_t>
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( Func::UInt64ToString( *(uint64_t*)_Data ) );
		}
	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD> class TextFormatElementInput<T_LCD, C_LCD, int64_t>
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( Func::Int64ToString( *(int64_t*)_Data ) );
		}
	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD, typename T> class TextFormatElementInput_String
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( (char*)_Data );
		}

	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD> class TextFormatElementInput_Clock
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( "(Clock)" );
		}

	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD, typename T_OBJECT> class TextFormatElementInput_Object
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( ((T_OBJECT *)_Data)->ToString() );
		}

	};
//---------------------------------------------------------------------------
	template <typename T_LCD, T_LCD &C_LCD, typename T_OBJECT> class TextFormatElementInput_Binary
	{
	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			C_LCD.SetValue( ((T_OBJECT *)_Data)->ToString() );
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


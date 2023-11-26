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
	struct TSplitTextStringItem
	{
	public:
		Mitov::String	Text;
		int8_t			Index;
			
		void *FValue;

	public:
		inline bool NotEmptyText() { return Text[ 0 ] != '\0'; }

		inline bool IsSeparator( char *&AText )
		{
			const char * APtr = Text.c_str();
//			Serial.println( "IsSeparator" );
//			Serial.println( APtr );
			while( *APtr )
			{
				if( ! *AText )
					break;

//				Serial.print( *AText );
//				Serial.print( " - " );
//				Serial.println( *APtr );

				if( *APtr ++ != *AText ++ )
					break;
			}

			return( ! *APtr );
		}

		inline void ProcessTextOnly( char *&AText, bool &ACanSend )
		{
			if( ! ACanSend )
				return;

//			Serial.println( "Process1" );

			const char * APtr = Text.c_str();
			while( *APtr )
			{
				if( ! *AText )
					break;

				if( *APtr++ != *AText++ )
					break;
			}

			ACanSend = ! *APtr;
//			Serial.println( ACanSend );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_OutputPin
	> class SplitTextElementText :
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		template <typename T_DELEGATOR> inline void Process( T_DELEGATOR *ADelegator, char *&AText, bool &ACanSend )
		{
//			Mitov::String AUnsignedText;
//			const char *ASeparatorPtr = ASeparatorText;

			ADelegator->FValue = "";
			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( ADelegator->NotEmptyText() && ADelegator->IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator2" );
//					Serial.println( ADelegator->FValue );

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( ADelegator->FValue );

					return; // Exit without double checking separator
				}

				ADelegator->FValue += *AText ++;
			}

//			Serial.println( ADelegator->FValue );
			ACanSend = ADelegator->IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

	public:
		inline void CreateStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "CreateStorage1" );
			AItem.FValue = new Mitov::String;
		}

		inline void DestroyStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "DestroyStorage1" );
			delete (Mitov::String *)AItem.FValue;
		}

		void ProcessDynamic( Mitov::TSplitTextStringItem &AItem, char *&AText, bool &ACanSend )
		{
//			Serial.println( "ProcessDynamic1" );
			Mitov::String &AValue = *( Mitov::String *)AItem.FValue;
			AValue = "";
			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( AItem.NotEmptyText() && AItem.IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator1" );
//					Serial.println( AValue );

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( AValue );

					return; // Exit without double checking separator
				}

				AValue += *AText ++;
			}

			ACanSend = AItem.IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( AValue );

		}

		void SendDataDynamic( Mitov::TSplitTextStringItem &AItem, bool &ACanSend )
		{
//			Serial.println( "SendDataDynamic" );
//			Serial.println( ACanSend );
//			Serial.println( *( uint32_t *)AItem.FValue );
			Mitov::String &AValue = *( Mitov::String *)AItem.FValue;

			if( ACanSend )
				T_OutputPin::SetPinValue( AValue );

			AValue = "";
		}

	public:
		template <typename T_DELEGATOR> inline void SendData( T_DELEGATOR *ADelegator, bool ACanSend )
		{
			if( ACanSend )
				T_OutputPin::SetPinValue( ADelegator->FValue );

			ADelegator->FValue = "";
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_OutputPin
	> class SplitTextElementUnsigned :
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		template <typename T_DELEGATOR> inline void Process( T_DELEGATOR *ADelegator, char *&AText, bool &ACanSend )
		{
//			Mitov::String AUnsignedText;
//			const char *ASeparatorPtr = ASeparatorText;

			ADelegator->FValue = 0;
			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( ADelegator->NotEmptyText() && ADelegator->IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator1" );
//					Serial.println( ADelegator->FValue );
					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( ADelegator->FValue );

					return; // Exit without double checking separator
				}

				char AChar = *AText;
				if( ! isdigit( AChar ))
					break;

				ADelegator->FValue *= 10;
				ADelegator->FValue += ( AChar - '0' );

				++ AText; // Do not increment before checking for isdigit!
			}

			ACanSend = ADelegator->IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

	public:
		inline void CreateStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "CreateStorage1" );
			AItem.FValue = new uint32_t;
		}

		inline void DestroyStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "DestroyStorage1" );
			delete (uint32_t *)AItem.FValue;
		}

		void ProcessDynamic( Mitov::TSplitTextStringItem &AItem, char *&AText, bool &ACanSend )
		{
//			Serial.println( "ProcessDynamic1" );
			uint32_t &AValue = *(uint32_t *)AItem.FValue;
			AValue = 0;
			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( AItem.NotEmptyText() && AItem.IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator1" );
//					Serial.println( AValue );
					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( AValue );

					return; // Exit without double checking separator
				}

				char AChar = *AText;
				if( ! isdigit( AChar ))
					break;

				AValue *= 10;
				AValue += ( AChar - '0' );

				++ AText; // Do not increment before checking for isdigit!
			}

			ACanSend = AItem.IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( AValue );

		}

		void SendDataDynamic( Mitov::TSplitTextStringItem &AItem, bool &ACanSend )
		{
//			Serial.println( "SendDataDynamic" );
//			Serial.println( ACanSend );
//			Serial.println( *( uint32_t *)AItem.FValue );
			if( ACanSend )
				T_OutputPin::SetPinValue( *( uint32_t *)AItem.FValue );
		}

	public:
		template <typename T_DELEGATOR> inline void SendData( T_DELEGATOR *ADelegator, bool ACanSend )
		{
			if( ACanSend )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_OutputPin
	> class SplitTextElementInteger :
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		template <typename T_DELEGATOR> inline void Process( T_DELEGATOR *ADelegator, char *&AText, bool &ACanSend )
		{
//			Mitov::String AUnsignedText;
//			const char *ASeparatorPtr = ASeparatorText;

			ADelegator->FValue = 0;

			bool AFirstCharProcessed = false;
			bool AIsNegative = false;

			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( ADelegator->NotEmptyText() && ADelegator->IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator3" );
//					Serial.println( ADelegator->FValue );
					if( AIsNegative )
						ADelegator->FValue = -ADelegator->FValue;

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( ADelegator->FValue );

					return; // Exit without double checking separator
				}

//				Serial.println( "TEST2" );
				char AChar = *AText;
//				Serial.println( AChar );
//				Serial.println( "-----" );
				if( AChar == '-' )
				{
//					Serial.println( "is '-'" );
					if( AFirstCharProcessed )
						break;

					AIsNegative = true;
					AFirstCharProcessed = true;
//					Serial.println( "AIsNegative" );
				}

				else if( AChar == '+' )
				{
					if( AFirstCharProcessed )
						break;

					AFirstCharProcessed = true;
				}

				else
				{
					if( ! isdigit( AChar ))
						break;

					ADelegator->FValue *= 10;
					ADelegator->FValue += ( AChar - '0' );
				}

				++ AText; // Do not increment before checking for isdigit!
			}

			if( AIsNegative )
				ADelegator->FValue = -ADelegator->FValue;

			ACanSend = ADelegator->IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

	public:
		inline void CreateStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "CreateStorage1" );
			AItem.FValue = new int32_t;
		}

		inline void DestroyStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "DestroyStorage1" );
			delete (int32_t *)AItem.FValue;
		}

		void ProcessDynamic( Mitov::TSplitTextStringItem &AItem, char *&AText, bool &ACanSend )
		{
//			Serial.println( "ProcessDynamic1" );
			int32_t &AValue = *(int32_t *)AItem.FValue;
			AValue = 0;

			bool AFirstCharProcessed = false;
			bool AIsNegative = false;

			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( AItem.NotEmptyText() && AItem.IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator1" );
//					Serial.println( AValue );

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( AValue );

					return; // Exit without double checking separator
				}

//				Serial.println( "TEST2" );
				char AChar = *AText;
//				Serial.println( AChar );
//				Serial.println( "-----" );
				if( AChar == '-' )
				{
//					Serial.println( "is '-'" );
					if( AFirstCharProcessed )
						break;

					AIsNegative = true;
					AFirstCharProcessed = true;
//					Serial.println( "AIsNegative" );
				}

				else if( AChar == '+' )
				{
					if( AFirstCharProcessed )
						break;

					AFirstCharProcessed = true;
				}

				else
				{
					if( ! isdigit( AChar ))
						break;

					AValue *= 10;
					AValue += ( AChar - '0' );
				}

				++ AText; // Do not increment before checking for isdigit!
			}

			ACanSend = AItem.IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( AValue );

		}

		void SendDataDynamic( Mitov::TSplitTextStringItem &AItem, bool &ACanSend )
		{
//			Serial.println( "SendDataDynamic" );
//			Serial.println( ACanSend );
//			Serial.println( *( uint32_t *)AItem.FValue );
			if( ACanSend )
				T_OutputPin::SetPinValue( *( int32_t *)AItem.FValue );
		}

	public:
		template <typename T_DELEGATOR> inline void SendData( T_DELEGATOR *ADelegator, bool ACanSend )
		{
			if( ACanSend )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_OutputPin
	> class SplitTextElementAnalog :
		public T_OutputPin
	{
	public:
		_V_PIN_( OutputPin )

	public:
		template <typename T_DELEGATOR> inline void Process( T_DELEGATOR *ADelegator, char *&AText, bool &ACanSend )
		{
//			const char *ASeparatorPtr = ASeparatorText;

			Mitov::String AFloatText;

			ADelegator->FValue = 0.0f;
			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( ADelegator->NotEmptyText() && ADelegator->IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator4" );
//					Serial.println( ADelegator->FValue );
					char *AEndPos;
					ADelegator->FValue = strtod( AFloatText.c_str(), &AEndPos );
					if( *AEndPos )
						ACanSend = false;

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( ADelegator->FValue );

					return; // Exit without double checking separator
				}

				AFloatText += *AText ++;
			}

			ADelegator->FValue = strtod( AFloatText.c_str(), nullptr );

			ACanSend = ADelegator->IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

		inline void CreateStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "CreateStorage1" );
			AItem.FValue = new float;
		}

		inline void DestroyStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "DestroyStorage1" );
			delete (float *)AItem.FValue;
		}

		void ProcessDynamic( Mitov::TSplitTextStringItem &AItem, char *&AText, bool &ACanSend )
		{
//			Serial.println( "ProcessDynamic1" );
			float &AValue = *(float *)AItem.FValue;
			AValue = 0.0f;

			Mitov::String AFloatText;
			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( AItem.NotEmptyText() && AItem.IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
//					Serial.println( "IsSeparator4" );
//					Serial.println( ADelegator->FValue );
					char *AEndPos;
					AValue = strtod( AFloatText.c_str(), &AEndPos );
					if( *AEndPos )
						ACanSend = false;


					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( AValue );

					return; // Exit without double checking separator
				}

				AFloatText += *AText ++;
			}

			AValue = strtod( AFloatText.c_str(), nullptr );

			ACanSend = AItem.IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( AValue );

		}

		void SendDataDynamic( Mitov::TSplitTextStringItem &AItem, bool &ACanSend )
		{
//			Serial.println( "SendDataDynamic" );
//			Serial.println( ACanSend );
//			Serial.println( *( uint32_t *)AItem.FValue );
			if( ACanSend )
				T_OutputPin::SetPinValue( *( float *)AItem.FValue );
		}

	public:
		template <typename T_DELEGATOR> inline void SendData( T_DELEGATOR *ADelegator, bool ACanSend )
		{
			if( ACanSend )
				T_OutputPin::SetPinValue( ADelegator->FValue );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_FalseValue,
		typename T_OutputPin,
		typename T_TrueValue
	> class SplitTextElementDigital :
		public T_FalseValue,
		public T_OutputPin,
		public T_TrueValue
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( FalseValue )
		_V_PROP_( TrueValue )

	public:
		template <typename T_DELEGATOR> inline void Process( T_DELEGATOR *ADelegator, char *&AText, bool &ACanSend )
		{
//			const char *ASeparatorPtr = ASeparatorText;

			ADelegator->FValue() = false;

			Mitov::String ABoolText;

			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( ADelegator->NotEmptyText() && ADelegator->IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
					if( ABoolText == TrueValue() )
						ADelegator->FValue() = true;

					else if( ABoolText != FalseValue() )
						ACanSend = false;

//					Serial.println( "IsSeparator4" );
//					Serial.println( ADelegator->FValue );
//					char *AEndPos;
//					ADelegator->FValue = strtod( AFloatText.c_str(), &AEndPos );
//					if( *AEndPos )
//						ACanSend = false;

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( ADelegator->FValue() );

					return; // Exit without double checking separator
				}

				ABoolText += *AText ++;
			}

//			ADelegator->FValue = strtod( AFloatText.c_str(), nullptr );

//			Serial.println( "TEST5" );

			if( ABoolText == TrueValue() )
				ADelegator->FValue() = true;

			else if( ABoolText != FalseValue() )
			{
				ACanSend = false;
				return;
			}

			ACanSend = ADelegator->IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( ADelegator->FValue() );

		}

		inline void CreateStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "CreateStorage1" );
			AItem.FValue = new bool;
		}

		inline void DestroyStorage( Mitov::TSplitTextStringItem &AItem )
		{
//			Serial.println( "DestroyStorage1" );
			delete (bool *)AItem.FValue;
		}

		void ProcessDynamic( Mitov::TSplitTextStringItem &AItem, char *&AText, bool &ACanSend )
		{
//			Serial.println( "ProcessDynamic1" );
			bool &AValue = *(bool *)AItem.FValue;
			AValue = false;

			Mitov::String ABoolText;

			while( *AText )
			{
//				Serial.println( "TEST1" );
				char *ATextCheck = AText;
				if( AItem.NotEmptyText() && AItem.IsSeparator( ATextCheck ) )
				{
					AText = ATextCheck;
					if( ABoolText == TrueValue() )
						AValue = true;

					else if( ABoolText != FalseValue() )
						ACanSend = false;

//					Serial.println( "IsSeparator4" );
//					Serial.println( ADelegator->FValue );
//					char *AEndPos;
//					ADelegator->FValue = strtod( AFloatText.c_str(), &AEndPos );
//					if( *AEndPos )
//						ACanSend = false;

					if( C_OWNER.AllowPartial() )
						T_OutputPin::SetPinValue( AValue );

					return; // Exit without double checking separator
				}

				ABoolText += *AText ++;
			}

//			ADelegator->FValue = strtod( AFloatText.c_str(), nullptr );

//			Serial.println( "TEST5" );

			if( ABoolText == TrueValue() )
				AValue = true;

			else if( ABoolText != FalseValue() )
			{
				ACanSend = false;
				return;
			}

			ACanSend = AItem.IsSeparator( AText );

			if( C_OWNER.AllowPartial() )
				T_OutputPin::SetPinValue( AValue );

		}

		void SendDataDynamic( Mitov::TSplitTextStringItem &AItem, bool &ACanSend )
		{
//			Serial.println( "SendDataDynamic" );
//			Serial.println( ACanSend );
//			Serial.println( *( uint32_t *)AItem.FValue );
			if( ACanSend )
				T_OutputPin::SetPinValue( *( bool *)AItem.FValue );
		}

	public:
		template <typename T_DELEGATOR> inline void SendData( T_DELEGATOR *ADelegator, bool ACanSend )
		{
			if( ACanSend )
				T_OutputPin::SetPinValue( ADelegator->FValue().GetValue() );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_FValue,
		typename T_Text,
		typename T_ITEM, T_ITEM &C_ITEM
	> class TSplitTextStringItemDigitalIndexed :
		public T_FValue,
		public T_Text
	{
	public:
		_V_PROP_( FValue )

	public:
		inline void Process( char *&AText, bool &ACanSend )
		{
			if( ! ACanSend )
				return;

//			Serial.println( "Process2" );

			C_ITEM.Process( this, AText, ACanSend );
		}

	public:
		inline bool NotEmptyText() { return *T_Text::GetValue() != '\0'; }

		inline bool IsSeparator( char *&AText )
		{
			const char * APtr = T_Text::GetValue();
//			Serial.println( "IsSeparator" );
//			Serial.println( APtr );
			while( *APtr )
			{
				if( ! *AText )
					break;

//				Serial.print( *AText );
//				Serial.print( " - " );
//				Serial.println( *APtr );

				if( *APtr ++ != *AText ++ )
					break;
			}

			return( ! *APtr );
		}

	public:
		inline void SendData( bool ACanSend )
		{
			C_ITEM.SendData( this, ACanSend );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_DATA,
		typename T_Text,
		typename T_ITEM, T_ITEM &C_ITEM
	> class TSplitTextStringItemTypedIndexed :
		public T_Text
	{
	public:
		T_DATA	FValue;

	public:
		inline void Process( char *&AText, bool &ACanSend )
		{
			if( ! ACanSend )
				return;

//			Serial.println( "Process2" );

			C_ITEM.Process( this, AText, ACanSend );
		}

	public:
		inline bool NotEmptyText() { return *T_Text::GetValue() != '\0'; }

		inline bool IsSeparator( char *&AText )
		{
			const char * APtr = T_Text::GetValue();
//			Serial.println( "IsSeparator" );
//			Serial.println( APtr );
			while( *APtr )
			{
				if( ! *AText )
					break;

//				Serial.print( *AText );
//				Serial.print( " - " );
//				Serial.println( *APtr );

				if( *APtr ++ != *AText ++ )
					break;
			}

			return( ! *APtr );
		}

	public:
		inline void SendData( bool ACanSend )
		{
			C_ITEM.SendData( this, ACanSend );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Text
	> class TSplitTextStringItemText :
		public T_Text
	{
	public:
		inline void Process( char *&AText, bool &ACanSend )
		{
			if( ! ACanSend )
				return;

//			Serial.println( "Process1" );

			const char * APtr = T_Text::GetValue();
			while( *APtr )
			{
				if( ! *AText )
					break;

				if( *APtr++ != *AText++ )
					break;
			}

			ACanSend = ! *APtr;
//			Serial.println( ACanSend );
		}

	public:
		inline void SendData( bool ACanSend ) {}

	};
//---------------------------------------------------------------------------
	template <
		typename T_AllowPartial,
		uint16_t C_COUNT,
		typename T_Elements_Process,
		typename T_Elements_SendData,
		typename T_Enabled,
		typename T_IngnoreEnd
	> class SplitText_Fixed :
		public T_AllowPartial,
		public T_Enabled,
		public T_IngnoreEnd
	{
	public:
		_V_PROP_( AllowPartial )
		_V_PROP_( Enabled )
		_V_PROP_( IngnoreEnd )

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			char * AText = (char *)_Data;
			bool ACanSend = true;
			T_Elements_Process::Call( AText, ACanSend );
			if( ! IngnoreEnd().GetValue() )
				if( *AText )
					ACanSend = false; // There is remaining text!

			if( ! AllowPartial().GetValue() )
				T_Elements_SendData::Call( ACanSend );

		}

	public:
		inline void SystemStart() {}

	};
//---------------------------------------------------------------------------
	template <
		typename T_AllowPartial,
		uint16_t C_COUNT,
		typename T_Elements_CreateStorage,
		typename T_Elements_DestroyStorage,
		typename T_Elements_Process,
		typename T_Elements_SendData,
		typename T_IngnoreEnd,
		typename T_Text
	> class SplitText :
		public T_AllowPartial,
		public T_IngnoreEnd,
		public T_Text
	{
	public:
		_V_PROP_( AllowPartial )
		_V_PROP_( IngnoreEnd )
		_V_PROP_( Text )

	protected:
		Mitov::SimpleList<TSplitTextStringItem>	FProcessElements;

	protected:
		void AddReadyElement( Mitov::String ATextItem, int16_t AIndex )
		{
			TSplitTextStringItem AItem;
			AItem.Text = ATextItem;

//			Serial.print( "AddElement: " ); Serial.print( AItem.Text ); Serial.print( " - " ); Serial.println( AIndex );

			if( AIndex < C_COUNT )
				AItem.Index = AIndex;

			else
				AItem.Index = -1;

			T_Elements_CreateStorage::Call( AItem );

			FProcessElements.push_back( AItem );
		}

	public:
		inline void InitElements( const String &AOldValue, const String &Value )
		{
		}

		void InitElements()
		{
			TSplitTextStringItem *AElements = FProcessElements.AsPointer();
			for( int i = 0; i < FProcessElements.size(); ++i )
				T_Elements_DestroyStorage::Call( AElements[ i ] );

			FProcessElements.clear();
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
////								AddReadyElement( ATextItem, AIndexText.toInt() );
								ATextItem = "";
								AIndexText = "";
							}

						}

						else
						{
							if( AIndexText.length() == 0 )
								AIndexText = AAutoIndex ++;

////							if( AIndexText.length() == 0 )
////								AddReadyElement( ATextItem, AAutoIndex ++ );

////							else
////								AddReadyElement( ATextItem, AIndexText.toInt() );

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
						if( AIndexText.length() != 0 )
							AddReadyElement( ATextItem, AIndexText.toInt() );

						else
							AddReadyElement( ATextItem, -1 );

						AInEscape = true;
						AIndexText = "";
					}

					else
						ATextItem += AChar;

				}

			}

			if( AInEscape )
			{
				if( AIndexText.length() == 0 )
					AddReadyElement( "", AAutoIndex );

				else
					AddReadyElement( "", AIndexText.toInt() );
			}

			else if( ATextItem.length() )
			{
				if( AIndexText.length() != 0 )
					AddReadyElement( ATextItem, AIndexText.toInt() );

				else
					AddReadyElement( ATextItem, -1 );

////				TStringItem	*AItem = new TStringItem;

////				AItem->Text = ATextItem;
////				AItem->Index = -1;

////				FReadyElements.push_back( AItem );
			}

//			Serial.println( "DEBUG>>" );
//			for( Mitov::SimpleList<TStringItem *>::iterator Iter = FReadyElements.begin(); Iter != FReadyElements.end(); ++Iter )
//				Serial.println( ( *Iter )->Text );

//			Serial.println( "<<DEBUG" );
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			char * AText = (char *)_Data;

			if( C_COUNT > 0 )
			{
				bool ACanSend = true;

				TSplitTextStringItem *AElements = FProcessElements.AsPointer();
				uint32_t ASize = FProcessElements.size();
				for( int i = 0; i < ASize; ++i )
				{
					if( AElements[ i ].Index < 0 )
						AElements[ i ].ProcessTextOnly( AText, ACanSend );

					else
						T_Elements_Process::Call( AElements[ i ], AText, ACanSend );

				}

				if( ! IngnoreEnd().GetValue() )
					if( *AText )
						ACanSend = false; // There is remaining text!

				if( ! AllowPartial().GetValue() )
					for( int i = 0; i < ASize; ++i )
					{
//						Serial.println( AElements[ i ].Index );
						if( AElements[ i ].Index >= 0 )
							T_Elements_SendData::Call( AElements[ i ], ACanSend );

					}

			}
/*
			bool ACanSend = true;
			T_Elements_Process::Call( AText, ACanSend );
			if( *AText )
				ACanSend = false; // There is remaining text!

			T_Elements_SendData::Call( ACanSend );
*/
		}

	public:
		inline void SystemStart()
		{
			InitElements();
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


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_Input,
		typename T_Output
	> class TArduinoInterpolateAnalogElement :
		public T_Input,
		public T_Output
	{
	public:
		_V_PROP_( Input )
		_V_PROP_( Output )

	public:
		inline void GetValue( float & AInput, float & AOutput )
		{
			AInput = Input();
			AOutput = Output();

        }

	};
//---------------------------------------------------------------------------
	template <
		uint32_t C_COUNT,
		uint32_t COUNT_INCLUDED_Elements,
		typename T_Elements,
		typename T_Elements_GetValue,
		typename T_Enabled,
		typename T_OutputPin
	> class TArduinoInterpolateAnalog :
	    public T_Elements,
	    public T_Elements_GetValue,
	    public T_Enabled,
	    public T_OutputPin
    {
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )

	public:
        inline void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled().GetValue() || ( C_COUNT < 2 ))
            {
                T_OutputPin::PinNotify( _Data );
                return;
			}

			float AValue = *(float *)_Data;

//			AValue = 1.319999;
//			AValue = 2.279999;
//			AValue = 4.24684143;

            float ATestValue1;

			if( ! COUNT_INCLUDED_Elements )
			{
				// Optimized for sorted constant array!
				uint8_t AIndexPart = ( C_COUNT - 1 ) / 2;

				uint8_t AIndex = 0;

				while( AIndexPart )
				{
					if( AValue > T_Elements::GetValue( ( AIndex + AIndexPart ) * 2 ) )
						AIndex += AIndexPart;

					AIndexPart >>= 1;
				}

				for(;;)
				{
					if( AIndex < ( C_COUNT - 2 ) )
						if( AValue >= T_Elements::GetValue( ( AIndex + 1 ) * 2 ) )
						{
							++ AIndex;
							continue;
						}

					break;
				}

				AValue = Func::MapRange<float>( AValue, T_Elements::GetValue( AIndex * 2 ), T_Elements::GetValue( ( AIndex + 1 ) * 2 ), T_Elements::GetValue( AIndex * 2 + 1 ), T_Elements::GetValue( ( AIndex + 1 ) * 2 + 1 ) );
				T_OutputPin::SetPinValue( AValue );
				return;

//				Serial.print( T_Elements::GetValue( AIndex * 2 ), 8 );
//				Serial.print( " - " );
//				Serial.println( T_Elements::GetValue( ( AIndex + 1 ) * 2 ), 8 );

//				ATestValue1 = Func::MapRange<float>( AValue, T_Elements::GetValue( AIndex * 2 ), T_Elements::GetValue( ( AIndex + 1 ) * 2 ), T_Elements::GetValue( AIndex * 2 + 1 ), T_Elements::GetValue( ( AIndex + 1 ) * 2 + 1 ) );
			}

			float AMinInput; // = T_Elements::GetValue( 0 );
			float AMinOutput; // = T_Elements::GetValue( 1 );
			float AMaxInput; // = T_Elements::GetValue( 2 );
			float AMaxOutput; // = T_Elements::GetValue( 3 );
			bool AMinAssigned = false;
			bool AMaxAssigned = false;

			for( int i = 0; i < C_COUNT; ++ i )
			{
				float AInput = T_Elements::GetValue( i * 2 );
				float AOutput = T_Elements::GetValue( i * 2 + 1 );
				T_Elements_GetValue::Call( i, AInput, AOutput );

				if( ! AMinAssigned )
				{
					AMinInput = AInput;
					AMinOutput = AOutput;
					AMinAssigned = true;
				}

				else if( ! AMaxAssigned )
				{
					if( AMinInput > AInput )
					{
						AMaxInput = AMinInput;
						AMaxOutput = AMinOutput;
						AMinInput = AInput;
						AMinOutput = AOutput;
					}

					else
					{
						AMaxInput = AInput;
						AMaxOutput = AOutput;
					}

					AMaxAssigned = true;
				}

				else if( AInput < AValue )
				{
					if( ( AInput > AMinInput ) || ( ( AMinInput > AValue ) && ( AInput < AMinInput ) ) )
					{
						if( AInput < AMinInput )
						{
							AMaxInput = AMinInput;
							AMaxOutput = AMinOutput;
							AMinInput = AInput;
							AMinOutput = AOutput;
						}

						else if( AInput > AMaxInput )
						{
							AMinInput = AMaxInput;
							AMinOutput = AMaxOutput;
							AMaxInput = AInput;
							AMaxOutput = AOutput;
						}

						else
						{
							AMinInput = AInput;
							AMinOutput = AOutput;
						}
					}
				}

				else
				{
					if( ( AInput < AMaxInput ) || ( ( AMaxInput < AValue ) && ( AInput > AMaxInput ) ) )
					{
						if( AInput > AMaxInput )
						{
							AMinInput = AMaxInput;
							AMinOutput = AMaxOutput;
							AMaxInput = AInput;
							AMaxOutput = AOutput;
						}

						else if( AInput < AMinInput )
						{
							AMaxInput = AMinInput;
							AMaxOutput = AMinOutput;
							AMinInput = AInput;
							AMinOutput = AOutput;
						}

						else
						{
							AMaxInput = AInput;
							AMaxOutput = AOutput;
                        }
					}
				}
			}

/*
			if( AMinInput > AMaxInput )
			{
				Serial.print( "ERROR!!!" );
			}
*/
			if( AMinInput == AMaxInput )
			{
//				Serial.print( "ERROR1!!!" );
				T_OutputPin::PinNotify( _Data );
				return;
			}

//			Serial.print( AMinInput, 8 );
//			Serial.print( " - " );
//			Serial.println( AMaxInput, 8 );

			AValue = Func::MapRange<float>( AValue, AMinInput, AMaxInput, AMinOutput, AMaxOutput );
/*
			if( ATestValue1 != AValue )
			{
				Serial.print( "ERROR! - " );
				Serial.print( *(float *)_Data, 8 );
				Serial.print( " " );
				Serial.print( ATestValue1, 8 );
				Serial.print( " " );
				Serial.println( AValue, 8 );
//				++ATestValue1;
			}
*/
			T_OutputPin::SetPinValue( AValue );
		}

	};
//---------------------------------------------------------------------------
} // Mitov
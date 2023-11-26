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
		typename T_ClockInputPin_o_IsConnected,
		typename T_Count,
		uint32_t C_Count,
		typename T_FIndex,
		typename T_FStepCounter,
		typename T_Normalize,
		typename T_OutputPin,
		typename T_Step
	> class SlidingWindowAnalogArray :
		public T_ClockInputPin_o_IsConnected,
		public T_Count,
		public T_FIndex,
		public T_FStepCounter,
		public T_Normalize,
		public T_OutputPin,
		public T_Step
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Count )
		_V_PROP_( Step )
		_V_PROP_( Normalize )

	public:
		_V_PROP_( ClockInputPin_o_IsConnected )

	protected:
		_V_PROP_( FIndex )
		_V_PROP_( FStepCounter )

	protected:
		float   FQueue[ C_Count ];

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FIndex() >= Count() )
				FIndex() = 0;

//			Serial.println( *(float *)_Data );

			FQueue[ FIndex() ] = *(float *)_Data;
			FIndex() = FIndex() + 1;

//			TArray<float> ABuffer( Count(), FQueue );
//			Serial.println( ABuffer.ToString() );
//			return;

			if( ClockInputPin_o_IsConnected() )
				return;

			uint16_t AStep = ( Step() ) ? Step() : Count();
			FStepCounter() = FStepCounter() + 1;
			if( FStepCounter() >= AStep )
			{
				FStepCounter() = 0;
				ClockInputPin_o_Receive( nullptr );
			}
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			float *AData = new float[ Count() ];

			float *APtr = AData;
			int AIndex = FIndex();

			if( Normalize() )
			{
				float AMin = FQueue[ 0 ]; //MitovMin
				float AMax = AMin; //MitovMax

				for( int i = 1; i < Count(); ++i )
				{
					AMin = MitovMin( AMin, FQueue[ i ] );
					AMax = MitovMax( AMax, FQueue[ i ] );
				}

				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = (( FQueue[ AIndex ++ ] - AMin ) / ( AMax - AMin ));
				}
			}

			else
			{
				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = FQueue[ AIndex ++ ];
				}
			}

//			Serial.println( "TEST" );

			TArray<float> ABuffer( Count(), AData );
			T_OutputPin::SetPinValue( ABuffer );

			delete [] AData;
		}

	public:
		inline void SystemInit()
		{
			memset( FQueue, 0, sizeof( float ) * Count() );
		}

	public:
        inline SlidingWindowAnalogArray()
        {
		    FIndex() = 0;
		    FStepCounter() = 0;
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_ClockInputPin_o_IsConnected,
		typename T_Count,
		typename T_FStepCounter,
		typename T_Normalize,
		typename T_OutputPin,
		typename T_Step
	> class SlidingWindowAnalogArray_BindingCount :
		public T_ClockInputPin_o_IsConnected,
		public T_Count,
		public T_FStepCounter,
		public T_Normalize,
		public T_OutputPin,
		public T_Step
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Count )
		_V_PROP_( Step )
		_V_PROP_( Normalize )

	public:
		_V_PROP_( ClockInputPin_o_IsConnected )

	protected:
		_V_PROP_( FStepCounter )

	protected:
		uint32_t	FIndex = 0;
		float		*FQueue = nullptr;

	public:
		void SetCount( uint16_t AValue )
		{
			if( Count() == AValue )
				return;

			float	*AOldBuffer = FQueue;
			FQueue = new float[ AValue ];

			if( AValue > Count() )
				memset( FQueue + Count(), 0, ( AValue - Count() ) * sizeof( float ) );

			memcpy( FQueue, AOldBuffer, MitovMin( Count(), AValue ) * sizeof( float ));

			Count() = AValue;
			delete [] AOldBuffer;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FIndex >= Count() )
				FIndex = 0;

//			Serial.println( *(float *)_Data );

			FQueue[ FIndex ++ ] = *(float *)_Data;

//			TArray<float> ABuffer( Count(), FQueue );
//			Serial.println( ABuffer.ToString() );
//			return;

			if( ClockInputPin_o_IsConnected() )
				return;

			uint16_t AStep = ( Step() ) ? Step() : Count();
			FStepCounter() = FStepCounter() + 1;
			if( FStepCounter() >= AStep )
			{
				FStepCounter() = 0;
				ClockInputPin_o_Receive( nullptr );
			}
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			float *AData = new float[ Count() ];

			float *APtr = AData;
			int AIndex = FIndex;

			if( Normalize() )
			{
				float AMin = FQueue[ 0 ]; //MitovMin
				float AMax = AMin; //MitovMax

				for( int i = 1; i < Count(); ++i )
				{
					AMin = MitovMin( AMin, FQueue[ i ] );
					AMax = MitovMax( AMax, FQueue[ i ] );
				}

				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = (( FQueue[ AIndex ++ ] - AMin ) / ( AMax - AMin ));
				}
			}

			else
			{
				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = FQueue[ AIndex ++ ];
				}
			}

//			Serial.println( "TEST" );

			TArray<float> ABuffer( Count(), AData );
			T_OutputPin::SetPinValue( ABuffer );

			delete [] AData;
		}

	public:
		inline void SystemInit()
		{
			if( FQueue )
				delete [] FQueue;

			FQueue = new float[ Count() ];
			memset( FQueue, 0, sizeof( float ) * Count() );
		}

	public:
        inline SlidingWindowAnalogArray_BindingCount()
        {
		    FStepCounter() = 0;
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_ClockInputPin_o_IsConnected,
		typename T_Count,
		uint32_t C_Count,
		typename T_FIndex,
		typename T_Normalize,
		typename T_OutputPin,
		typename T_Step
	> class SlidingWindowAnalogArray_BindingStep :
		public T_ClockInputPin_o_IsConnected,
		public T_Count,
		public T_FIndex,
		public T_Normalize,
		public T_OutputPin,
		public T_Step
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Count )
		_V_PROP_( Step )
		_V_PROP_( Normalize )

	public:
		_V_PROP_( ClockInputPin_o_IsConnected )

	protected:
		_V_PROP_( FIndex )

	protected:
		uint32_t	FStepCounter = 0;
		float       FQueue[ C_Count ];

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FIndex() >= Count() )
				FIndex() = 0;

//			Serial.println( *(float *)_Data );

			FQueue[ FIndex() ] = *(float *)_Data;
			FIndex() = FIndex() + 1;

//			TArray<float> ABuffer( Count(), FQueue );
//			Serial.println( ABuffer.ToString() );
//			return;

			if( ClockInputPin_o_IsConnected() )
				return;

			uint16_t AStep = ( Step() ) ? Step() : Count();
			if( ++ FStepCounter >= AStep )
			{
				FStepCounter = 0;
				ClockInputPin_o_Receive( nullptr );
			}
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			float *AData = new float[ Count() ];

			float *APtr = AData;
			int AIndex = FIndex();

			if( Normalize() )
			{
				float AMin = FQueue[ 0 ]; //MitovMin
				float AMax = AMin; //MitovMax

				for( int i = 1; i < Count(); ++i )
				{
					AMin = MitovMin( AMin, FQueue[ i ] );
					AMax = MitovMax( AMax, FQueue[ i ] );
				}

				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = (( FQueue[ AIndex ++ ] - AMin ) / ( AMax - AMin ));
				}
			}

			else
			{
				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = FQueue[ AIndex ++ ];
				}
			}

//			Serial.println( "TEST" );

			TArray<float> ABuffer( Count(), AData );
			T_OutputPin::SetPinValue( ABuffer );

			delete [] AData;
		}

	public:
		inline void SystemInit()
		{
			memset( FQueue, 0, sizeof( float ) * Count() );
		}

	public:
        inline SlidingWindowAnalogArray_BindingStep()
        {
		    FIndex() = 0;
        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_ClockInputPin_o_IsConnected,
		typename T_Count,
		typename T_Normalize,
		typename T_OutputPin,
		typename T_Step
	> class SlidingWindowAnalogArray_BindingCount_BindingStep :
		public T_ClockInputPin_o_IsConnected,
		public T_Count,
		public T_Normalize,
		public T_OutputPin,
		public T_Step
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Count )
		_V_PROP_( Step )
		_V_PROP_( Normalize )

	public:
		_V_PROP_( ClockInputPin_o_IsConnected )

	protected:
		uint32_t	FIndex = 0; // Full range for the binding!
		uint32_t	FStepCounter = 0; // Full range for the binding!
		float		*FQueue = nullptr; // Dynamic Queue for the binding!

	public:
		void SetCount( uint16_t AValue )
		{
			if( Count() == AValue )
				return;

			float	*AOldBuffer = FQueue;
			FQueue = new float[ AValue ];

			if( AValue > Count() )
				memset( FQueue + Count(), 0, ( AValue - Count() ) * sizeof( float ) );

			memcpy( FQueue, AOldBuffer, MitovMin( Count(), AValue ) * sizeof( float ));

			Count() = AValue;
			delete [] AOldBuffer;
		}

	public:
		void InputPin_o_Receive( void *_Data )
		{
			if( FIndex >= Count() )
				FIndex = 0;

//			Serial.println( *(float *)_Data );

			FQueue[ FIndex ++ ] = *(float *)_Data;

//			TArray<float> ABuffer( Count(), FQueue );
//			Serial.println( ABuffer.ToString() );
//			return;

			if( ClockInputPin_o_IsConnected() )
				return;

			uint16_t AStep = ( Step() ) ? Step() : Count();
			if( ++ FStepCounter >= AStep )
			{
				FStepCounter = 0;
				ClockInputPin_o_Receive( nullptr );
			}
		}

		void ClockInputPin_o_Receive( void *_Data )
		{
			float *AData = new float[ Count() ];

			float *APtr = AData;
			int AIndex = FIndex;

			if( Normalize() )
			{
				float AMin = FQueue[ 0 ]; //MitovMin
				float AMax = AMin; //MitovMax

				for( int i = 1; i < Count(); ++i )
				{
					AMin = MitovMin( AMin, FQueue[ i ] );
					AMax = MitovMax( AMax, FQueue[ i ] );
				}

				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = (( FQueue[ AIndex ++ ] - AMin ) / ( AMax - AMin ));
				}
			}

			else
			{
				for( uint16_t i = 0; i < Count(); ++i )
				{
					if( AIndex >= Count() )
						AIndex = 0;

					*APtr++ = FQueue[ AIndex ++ ];
				}
			}

//			Serial.println( "TEST" );

			TArray<float> ABuffer( Count(), AData );
			T_OutputPin::SetPinValue( ABuffer );

			delete [] AData;
		}

	public:
		inline void SystemInit()
		{
			if( FQueue )
				delete [] FQueue;

			FQueue = new float[ Count() ];
			memset( FQueue, 0, sizeof( float ) * Count() );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

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

// #define __GENERAL_VISION_NEURONS__DEBUG__

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		uint8_t C_CONTEXT,
		typename T_IsKNN,
		typename T_UseInfinityDistance
	> class GeneralVisionNeuronsClassifier :
		public T_IsKNN,
		public T_UseInfinityDistance
	{
	public:
		_V_PROP_( IsKNN )
		_V_PROP_( UseInfinityDistance )

	public:
		TValueArray<float>	FFeaturesValue;

	public:
		static inline void ResetChain()
		{
			C_OWNER.ResetChain();
		}

		static inline int16_t Read_NetworkStatusRegister()
		{
			return C_OWNER.Read_NetworkStatusRegister();
		}

		static inline void Write_NetworkStatusRegister( int16_t AValue )
		{
			C_OWNER.Write_NetworkStatusRegister( AValue );
		}

		static inline int16_t Read_NeuronContextRegister()
		{
			return C_OWNER.Read_NeuronContextRegister();
		}

		static inline void Write_NeuronContextRegister( int16_t AValue )
		{
			C_OWNER.Write_NeuronContextRegister( AValue );
		}

		static inline int16_t Read_Component()
		{
			return C_OWNER.Read_Component();
		}

		static inline void Write_Component( int16_t AValue )
		{
			C_OWNER.Write_Component( AValue );
		}

		static inline void Write_LastComponent( int16_t AValue )
		{
			C_OWNER.Write_LastComponent( AValue );
		}

		static inline int16_t Read_ActiveInfluenceField()
		{
			return C_OWNER.Read_ActiveInfluenceField();
		}

		static inline void Write_ActiveInfluenceField( int16_t AValue )
		{
			C_OWNER.Write_ActiveInfluenceField( AValue );
		}

		static inline int16_t Read_MinInfluenceField()
		{
			return C_OWNER.Read_MinInfluenceField();
		}

		static inline void Write_MinInfluenceField( int16_t AValue )
		{
			C_OWNER.Write_MinInfluenceField( AValue );
		}

		static inline void Write_MaxInfluenceField( int16_t AValue )
		{
			C_OWNER.Write_MaxInfluenceField( AValue );
		}

		static inline int16_t Read_CountCommittedNeurons()
		{
			return C_OWNER.Read_CountCommittedNeurons();
		}

		static inline int16_t Read_Category()
		{
			return C_OWNER.Read_Category();
		}

		static inline void Write_Category( int16_t AValue )
		{
			C_OWNER.Write_Category( AValue );
		}

		static inline int16_t Read_NeuronID()
		{
			return C_OWNER.Read_NeuronID();
		}

		static inline void Write_GlobalControlRegister( int16_t AValue )
		{
			C_OWNER.Write_GlobalControlRegister( AValue );
		}

		static inline int16_t Read_Distance()
		{
			return C_OWNER.Read_Distance();
		}
		
		static constexpr inline uint32_t GetNumberFeatures()
		{
			return C_OWNER.GetNumberFeatures();
		}

/*
		static inline void ReadNeurons( float *AFeaturesData )
		{
			C_OWNER.ReadNeurons( AFeaturesData );
		}
*/
	public:
		inline constexpr uint32_t GetContext()
		{
			return C_CONTEXT;
		}

	public:
		void FillFeatures()
		{
			int ACount = MitovMin<uint32_t>( FFeaturesValue.Size, C_OWNER.GetNumberFeatures() );
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.print( "Count Features: " ); Serial.println( ACount );
#endif

			for( int i = 0; i < ACount; ++i )
			{
				float AValue = MitovConstrain( FFeaturesValue.Data[ i ], 0.0f, 1.0f ) * 255 + 0.5;
				if( i == ACount - 1 )
					C_OWNER.Write_LastComponent( AValue );

				else
					C_OWNER.Write_Component( AValue );
			}
		}

		void Forget()
		{
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.println( "Forget" );
#endif
			int ANCount = C_OWNER.Read_CountCommittedNeurons();
			int ATempNSR = C_OWNER.Read_NetworkStatusRegister();
			C_OWNER.Write_NetworkStatusRegister( 0x0010 );
			C_OWNER.ResetChain();
			int ACountOtherNeurons = 0;
			for( int i=0; i < ANCount; i++ )
			{
				int Temp = C_OWNER.Read_NeuronContextRegister();
				if( Temp && 0x7F != C_CONTEXT )
					++ACountOtherNeurons;

				Temp = C_OWNER.Read_Category(); // Move to the next neuron
			}

			if( ! ACountOtherNeurons )
				C_OWNER.Forget();

			else
			{
				int16_t *ANeuronData = new int16_t[ ACountOtherNeurons * ( 4 + ( C_OWNER.GetNumberFeatures() / 2 )) ];

				C_OWNER.ResetChain();
				int16_t *ACurrentPtr = ANeuronData;
				for( int i=0; i < ANCount; ++i )
				{
					int ATemp = C_OWNER.Read_NeuronContextRegister();
					if( ATemp && 0x7F == C_CONTEXT )
						ATemp = C_OWNER.Read_Category(); // Move to the next neuron

					else
					{
						*ACurrentPtr++ = ATemp; // Save context
						uint8_t *AKnowledgePtr = (uint8_t *)ACurrentPtr;
						ACurrentPtr += 64;
						for( int j = 0; j < C_OWNER.GetNumberFeatures(); ++j )
							*AKnowledgePtr++ = C_OWNER.Read_Component();

						*ACurrentPtr++ = C_OWNER.Read_ActiveInfluenceField();
						*ACurrentPtr++ = C_OWNER.Read_MinInfluenceField();
						*ACurrentPtr++ = C_OWNER.Read_Category(); // Move to the next neuron
					}
				}

//				C_OWNER.Write_NetworkStatusRegister( ATempNSR );
				C_OWNER.Forget();
//				C_OWNER.Write_NetworkStatusRegister( 0x0010 );
				C_OWNER.ResetChain();

				ACurrentPtr = ANeuronData;
				for( int i=0; i < ANCount; ++i )
				{
					C_OWNER.Write_NeuronContextRegister( *ACurrentPtr ++ );
					uint8_t *AKnowledgePtr = (uint8_t *)ACurrentPtr;
					ACurrentPtr += 64;
					for( int j = 0; j < C_OWNER.GetNumberFeatures(); ++j )
						C_OWNER.Write_Component( *AKnowledgePtr ++ );

					C_OWNER.Write_ActiveInfluenceField( *ACurrentPtr++ );
					C_OWNER.Write_MinInfluenceField( *ACurrentPtr++ );
					C_OWNER.Write_Category( *ACurrentPtr++ ); // Move to the next neuron
				}

				delete [] ANeuronData;
			}

			C_OWNER.Write_NetworkStatusRegister( ATempNSR ); // set the NN back to its calling status
		}

	public:
		inline void FeaturesInputPin_o_Receive( void *_Data )
		{
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.print( "FeaturesInputPin_o_Receive: " );
			Serial.println( ((TArray<float>*)_Data )->Size );
#endif
			FFeaturesValue = *(TArray<float>*)_Data;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER
	> class GeneralVisionNeuronForgetOperation
	{
	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			C_OWNER.Forget();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_CategoryOutputPin,
		typename T_CompletedOutputPin,
		typename T_FeaturesOutputPin,
		typename T_InfluenceFieldOutputPin,
		typename T_MinInfluenceFieldOutputPin,
		typename T_NeuronCompletedOutputPin
	> class GeneralVisionNeuronReadKnowledgeOperation :
		public T_CategoryOutputPin,
		public T_CompletedOutputPin,
		public T_FeaturesOutputPin,
		public T_InfluenceFieldOutputPin,
		public T_MinInfluenceFieldOutputPin,
		public T_NeuronCompletedOutputPin
	{
	public:
		_V_PIN_( FeaturesOutputPin )
		_V_PIN_( CategoryOutputPin )
		_V_PIN_( InfluenceFieldOutputPin )
		_V_PIN_( MinInfluenceFieldOutputPin )
		_V_PIN_( NeuronCompletedOutputPin )
		_V_PIN_( CompletedOutputPin )

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			int ANCount = C_OWNER.Read_CountCommittedNeurons();
			int ATempNSR = C_OWNER.Read_NetworkStatusRegister();

			C_OWNER.Write_NetworkStatusRegister( 0x0010 );
			C_OWNER.ResetChain();

			float *AFeaturesData = new float[ C_OWNER.GetNumberFeatures() ];

			for( int i=0; i < ANCount; i++ )
			{
				int ATemp = C_OWNER.Read_NeuronContextRegister();
				if( ATemp && 0x7F == C_OWNER.GetContext() )
					ATemp = C_OWNER.Read_Category(); // Move to the next neuron

				else
				{
//					C_OWNER.ReadNeurons( AFeaturesData );

					float *ADataPtr = AFeaturesData;
					for( int j = 0; j < C_OWNER.GetNumberFeatures(); ++j )
						*ADataPtr++ = float( C_OWNER.Read_Component() ) / 255;

					T_FeaturesOutputPin::SetPinValue( TArray<float>( C_OWNER.GetNumberFeatures(), AFeaturesData ));
					T_InfluenceFieldOutputPin::SetPinValue( C_OWNER.Read_ActiveInfluenceField() );
					T_MinInfluenceFieldOutputPin::SetPinValue( C_OWNER.Read_MinInfluenceField() );
					T_CategoryOutputPin::SetPinValue( C_OWNER.Read_Category() );
					T_NeuronCompletedOutputPin::ClockPin();
				}
			}

			delete [] AFeaturesData;
			C_OWNER.Write_NetworkStatusRegister( ATempNSR ); // set the NN back to its calling status

			T_CompletedOutputPin::ClockPin();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Category,
		typename T_Features,
        typename T_Features_ApplyValues,
        typename T_Features_GetValue,
		typename T_InfluenceField,
		typename T_MinInfluenceField
//        typename T_Values
	> class GeneralVisionNeuronAddNeuronOperation :
		public T_Category,
		public T_Features,
		public T_InfluenceField,
		public T_MinInfluenceField
	{
	public:
		_V_PROP_( Category )
        _V_PROP_( Features )
		_V_PROP_( MinInfluenceField )
		_V_PROP_( InfluenceField )

//	public:
//		uint32_t _MaxIndex;
//		float *_Values;

//	public:
//		float &operator[]( uint32_t AIndex ) { return T_Features::GetValue( AIndex ); }

	protected:
		void LoadFeatures()
		{
			for( uint32_t i = 0; i < MitovMin<uint32_t>( T_Features::GetCount(), C_OWNER.GetNumberFeatures() ); ++i )
            {
                float AValue = T_Features::GetValue( i );
                T_Features_GetValue::Call( i, AValue );
				C_OWNER.Write_Component( AValue * 255 + 0.5 );
            }
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			int ATempNSR = C_OWNER.Read_NetworkStatusRegister();
			C_OWNER.Write_NetworkStatusRegister( 0x0010 );

			C_OWNER.Write_NeuronContextRegister( C_OWNER.GetContext() );
			LoadFeatures();

			C_OWNER.Write_ActiveInfluenceField( InfluenceField() );
			C_OWNER.Write_MinInfluenceField( MinInfluenceField() );
			C_OWNER.Write_Category( Category() ); // Move to the next neuron

			C_OWNER.Write_NetworkStatusRegister( ATempNSR ); // set the NN back to its calling status
		}

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Neurons_ProcessNeurons
	> class GeneralVisionNeuronLoadKnowledgeOperation
	{
	public:
		static inline void Write_Component( uint8_t AValue )
		{
			C_OWNER.Write_Component( AValue );
		}

		static inline void Write_ActiveInfluenceField( int16_t AValue )
		{
			C_OWNER.Write_ActiveInfluenceField( AValue );
		}

		static inline void Write_MinInfluenceField( int16_t AValue )
		{
			C_OWNER.Write_MinInfluenceField( AValue );
		}

		static inline void Write_Category( int16_t AValue )
		{
			C_OWNER.Write_Category( AValue );
		}

		static constexpr inline uint32_t GetNumberFeatures()
		{
			return C_OWNER.GetNumberFeatures();
		}

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			int ATempNSR = C_OWNER.Read_NetworkStatusRegister();
			C_OWNER.Write_NetworkStatusRegister( 0x0010 );

			C_OWNER.Write_NeuronContextRegister( C_OWNER.GetContext() );

			T_Neurons_ProcessNeurons::Call();
/*
			for( int i = 0; i < FNeurons.size(); ++i )
			{
				FNeurons[ i ]->LoadFeatures();
				C_OWNER.Write_ActiveInfluenceField( FNeurons[ i ]->InfluenceField );
				C_OWNER.Write_MinInfluenceField( FNeurons[ i ]->MinInfluenceField );
				C_OWNER.Write_Category( FNeurons[ i ]->Category ); // Move to the next neuron
			}
*/

			C_OWNER.Write_NetworkStatusRegister( ATempNSR ); // set the NN back to its calling status
		}

	public:
		inline void SystemStart()
		{
			ClockInputPin_o_Receive( nullptr );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Category,
		typename T_Features,
        typename T_Features_ApplyValues,
        typename T_Features_GetValue,
		typename T_InfluenceField,
		typename T_MinInfluenceField
	> class GeneralVisionNeuronItem :
		public T_Category,
		public T_Features,
		public T_InfluenceField,
		public T_MinInfluenceField
	{
	public:
		_V_PROP_( Category )
        _V_PROP_( Features )
		_V_PROP_( MinInfluenceField )
		_V_PROP_( InfluenceField )

//	public:
//		uint32_t _MaxIndex;
//		float *_Values;

	public:
		void SetIndexedValue( uint32_t AIndex )
        {
        }

	public:
//		void LoadFeatures()
		void ProcessNeurons()
		{
			for( uint32_t i = 0; i < MitovMin<uint32_t>( T_Features::GetCount(), C_OWNER.GetNumberFeatures() ); ++ i )
            {
                float AValue = T_Features::GetValue( i );
                T_Features_GetValue::Call( i, AValue );
				C_OWNER.Write_Component( AValue * 255 + 0.5 );
            }

			C_OWNER.Write_ActiveInfluenceField( InfluenceField() );
			C_OWNER.Write_MinInfluenceField( MinInfluenceField() );
			C_OWNER.Write_Category( Category() ); // Move to the next neuron
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Category,
		typename T_MaxInfluenceField,
		typename T_MinInfluenceField
	> class GeneralVisionCurieTrainNeurons :
		public T_Category,
		public T_MaxInfluenceField,
		public T_MinInfluenceField
	{
	public:
		_V_PROP_( Category )
		_V_PROP_( MinInfluenceField )
		_V_PROP_( MaxInfluenceField )

//	protected:
//		int32_t	FValue = 0;

/*
	public:
		void CategoryInputPin_o_Receive( void *_Data )
		{
			FValue = 
		}
*/
	public:
		void TrainInputPin_o_Receive( void *_Data )
		{
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.println( "TrainInputPin_o_Receive" );
#endif
/*
			int ACount = 0;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				ACount += FOwner.FFeatures[ i ]->GetCount();

			uint8_t *ABuffer = new uint8_t[ ACount ];
			uint8_t *APtr = ABuffer;
			for( int i = 0; i < FOwner.FFeatures.size(); ++i )
				FOwner.FFeatures[ i ]->GetFeatures( APtr );

			Arduino101CurieNeurons::FNeurons.Learn( ABuffer, ACount, constrain( CategoryInputPin.Value, -32768, 32767 ));

			delete []ABuffer;
*/
			if( C_OWNER.IsKNN() )
				C_OWNER.Write_NetworkStatusRegister( C_OWNER.Read_NetworkStatusRegister() | 0x20 );

			else
				C_OWNER.Write_NetworkStatusRegister( C_OWNER.Read_NetworkStatusRegister() & ~0x20 );

//			*CurieNeuronRegisters::NM_GCR = C_OWNER.GetContext();
			if( C_OWNER.UseInfinityDistance() )
				C_OWNER.Write_GlobalControlRegister( ( C_OWNER.GetContext() & 0x7F ) | 0x80 );

			else
				C_OWNER.Write_GlobalControlRegister( C_OWNER.GetContext() & 0x7F );

			C_OWNER.Write_MinInfluenceField( MinInfluenceField() );
			C_OWNER.Write_MaxInfluenceField( MaxInfluenceField() );

			C_OWNER.FillFeatures();

//			Serial.println( "TRAIN" );
			C_OWNER.Write_Category( Category() );

#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.println( "OUT1" );
#endif
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_CategoryOutputPin,
		typename T_CountNeuronsOutputPin,
		typename T_DegeneratedOutputPin,
		typename T_DisagreenmentOutputPin,
		typename T_DistanceOutputPin,
		typename T_NeuronIndexOutputPin,
		typename T_RecognizedOutputPin
	> class GeneralVisionCurieRecognizeNeurons :
		public T_CategoryOutputPin,
		public T_CountNeuronsOutputPin,
		public T_DegeneratedOutputPin,
		public T_DisagreenmentOutputPin,
		public T_DistanceOutputPin,
		public T_NeuronIndexOutputPin,
		public T_RecognizedOutputPin
	{
	public:
		_V_PIN_( CategoryOutputPin )
		_V_PIN_( DistanceOutputPin )
		_V_PIN_( NeuronIndexOutputPin )
		_V_PIN_( DegeneratedOutputPin )
		_V_PIN_( RecognizedOutputPin )
		_V_PIN_( DisagreenmentOutputPin )
		_V_PIN_( CountNeuronsOutputPin )

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.println( "ClockInputPin_o_Receive" );
#endif
/*
			int ACount = 0;
			for( int i = 0; i < C_OWNER.FFeatures.size(); ++i )
				ACount += C_OWNER.FFeatures[ i ]->GetCount();

			uint8_t *ABuffer = new uint8_t[ ACount ];
			uint8_t *APtr = ABuffer;
			for( int i = 0; i < C_OWNER.FFeatures.size(); ++i )
				C_OWNER.FFeatures[ i ]->GetFeatures( APtr );
*/
			C_OWNER.Write_GlobalControlRegister( C_OWNER.GetContext() );

#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.println( "FillFeatures" );
#endif
			C_OWNER.FillFeatures();

			int AResult = C_OWNER.Read_NetworkStatusRegister();

#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.print( "Read_NetworkStatusRegister: " );
			Serial.println( AResult );
#endif

//			Arduino101CurieNeurons::FNeurons.Learn( ABuffer, ACount, constrain( CategoryInputPin.Value, -32768, 32767 ));
/*
			int ADistance;
			int ACategory;
			int ANid;
			int AResult = Arduino101CurieNeurons::FNeurons.Classify( ABuffer, ACount, &ADistance, &ACategory, &ANid );
*/
			switch( AResult )
			{
				case 4:
//					CategoryOutputPin.SendValue<int32_t>( ACategory );
					T_DisagreenmentOutputPin::SetPinValueHigh();
					T_RecognizedOutputPin::SetPinValueHigh();
					break;

				case 8:
//					CategoryOutputPin.SendValue<int32_t>( ACategory );
					T_DisagreenmentOutputPin::SetPinValueLow();
					T_RecognizedOutputPin::SetPinValueHigh();
					break;

				default:
					T_DisagreenmentOutputPin::SetPinValueLow();
					T_RecognizedOutputPin::SetPinValueLow();
			}

			uint32_t ANeuronCount = 0;

			for(;;)
			{
				uint16_t ADistance = C_OWNER.Read_Distance();
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
				Serial.print( "ADistance: " ); Serial.println( ADistance );
#endif
				if( ADistance == 0xFFFF )
					break;

				int ACategory = C_OWNER.Read_Category();
				bool ADegenerated = ( ( ACategory & 0x8000 ) != 0 );
				ACategory &= 0x7FFF;

				uint32_t ANeuronId = C_OWNER.Read_NeuronID();

				++ ANeuronCount;
				T_DegeneratedOutputPin::SetPinValue( ADegenerated );
				T_NeuronIndexOutputPin::SetPinValue( ANeuronId );
				T_DistanceOutputPin::SetPinValue( ADistance );
				T_CategoryOutputPin::SetPinValue( ACategory );
			}

			T_CountNeuronsOutputPin::SetPinValue( ANeuronCount );
//			delete []ABuffer;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_AllocatedOutputPin,
		typename T_AvailableOutputPin
	> class GeneralVisionNeuronsCount :
		public T_AllocatedOutputPin,
		public T_AvailableOutputPin
	{
	public:
		_V_PIN_( AllocatedOutputPin )
		_V_PIN_( AvailableOutputPin )

	};
//---------------------------------------------------------------------------
	template <
		typename T_0_IMPLEMENTATION
//		typename T_AllocatedNeuronsOutputPin
	> class ArduinoGeneralVisionNeuronsModule :
		public T_0_IMPLEMENTATION
//		public T_AllocatedNeuronsOutputPin
	{
//	public:
//		_V_PIN_( AllocatedNeuronsOutputPin )

	public:
		inline void SystemInit()
		{
            T_0_IMPLEMENTATION::SystemInit();
//			delay(100);

//			return;
//			Digital.Write( 5, false );

//			int TempNSR = Read_NetworkStatusRegister();
			T_0_IMPLEMENTATION::Forget();
			T_0_IMPLEMENTATION::Write_NetworkStatusRegister( 0x0010 );
			for( uint32_t i = 0; i < T_0_IMPLEMENTATION::GetNumberFeatures(); i++ ) 
				T_0_IMPLEMENTATION::TestComponent();

			T_0_IMPLEMENTATION::Write_NetworkStatusRegister( 0 );
//			Write_NetworkStatusRegister( TempNSR );
		}


/*
		inline void SystemStart()
		{
		}
*/
	public:
		inline void ForgetInputPin_o_Receive( void *_Data )
		{
			T_0_IMPLEMENTATION::Forget();
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
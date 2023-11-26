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
#include <Mitov_BasicGenerator.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
	    uint8_t COUNT_OutputPins,
		typename T_Enabled,
		typename T_OutputPins_GetPinIsConnected,
		typename T_OutputPins_SetPinValue
    > class AudioToAnalogArray :
		public T_Enabled
    {
    public:
		_V_PROP_( Enabled )

    public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled() )
				return;

//            Serial.println( "TEST 1" );
			TAudioBuffer &ABuffer = *(TAudioBuffer *)_Data;

			uint8_t AByteSize = ABuffer.GetBytesPerSample();
			uint8_t AChannelIndex = 0;
			uint8_t *ADataStart = ABuffer.Data;
			uint32_t ASize = ABuffer.Size;
            uint32_t ASubStep = AByteSize * ( ABuffer.GetChannelsCount() - 1 );
			for( int i = 0; i < sizeof( ABuffer.ChannelsMap ) * 8; ++ i )
			{
				if( AChannelIndex >= COUNT_OutputPins )
					break;

				if( ABuffer.ChannelsMap & ( uint32_t( 1 ) << i ) )
				{
					bool AIsConnected = false;
					T_OutputPins_GetPinIsConnected::Call( i, AIsConnected );
					uint8_t *AData = ADataStart;
					ADataStart += AByteSize;
					if( AIsConnected )
					{
						float *AOutData = new float[ ASize ];
						float *AOutDataPtr = AOutData;
						for( int j = 0; j < ASize; ++ j )
						{
							*AOutDataPtr ++ = TAudioBuffer::ReadNormalizedSample( AData, AByteSize, ABuffer.BitsPerSample );
//							Serial.println( *( AOutDataPtr - 1 ) );
							AData += ASubStep;
                        }

//			Serial.println( "TEST 2" );
//			Serial.println( ASize );
//			delay( 1000 );

//						Serial.println( "+++" );
//						Serial.println( *AOutData );

/*
						if( ATest1 != *AOutData )
						{
							++ AOutDataPtr;

						}
*/
						TArray<float> ABuffer( ASize, AOutData );
						T_OutputPins_SetPinValue::Call( i, ABuffer );

//			Serial.println( "TEST 3" );
//			delay( 1000 );
						delete [] AOutData;
					}

					++ AChannelIndex;
				}
			}
		}

    };
//---------------------------------------------------------------------------
	template <
	    typename T_BitsPerSample,
	    uint8_t C_COUNT_CONNECTED_InputPins,
	    uint8_t C_COUNT_InputPins,
		typename T_Enabled,
		typename T_InputPins_GetPinIsConnected,
		typename T_MaxSamples,
		typename T_MinSamples,
		typename T_OutputPin,
		typename T_OverfillOutputPin,
		typename T_SampleRate
    > class AnalogArrayToAudio :
	    public T_BitsPerSample,
		public T_Enabled,
		public T_MaxSamples,
		public T_MinSamples,
		public T_OutputPin,
		public T_OverfillOutputPin,
		public T_SampleRate
	{
    public:
        _V_PIN_( OutputPin )
        _V_PIN_( OverfillOutputPin )

    public:
		_V_PROP_( BitsPerSample )
		_V_PROP_( Enabled )
		_V_PROP_( MaxSamples )
		_V_PROP_( MinSamples )
		_V_PROP_( SampleRate )

	protected:
	    SimpleList<uint8_t, uint32_t>  FBuffers[ C_COUNT_CONNECTED_InputPins ];

    public:
        void UpdateBitsPerSample()
        {
            for( int i = 0; i < C_COUNT_CONNECTED_InputPins; ++ i )
                FBuffers[ i ].clear();

		}

	public:
		inline void InputPins_o_Receive( int AIndex, void *_Data )
		{
			if( ! Enabled() )
				return;

			uint8_t ADisconnectedPins = 0;
			uint8_t AChannelsMap = 0;
			for( int i = 0; i < C_COUNT_InputPins; ++ i )
			{
				bool AIsConnected = false;
				T_InputPins_GetPinIsConnected::Call( i, AIsConnected );
				if( AIsConnected )
					AChannelsMap |= ( uint32_t( 1 ) << i );

				else if( i <= AIndex )
					++ ADisconnectedPins;

			}

/*
			for( int i = 0; i <= AIndex; ++ i )
			{
				bool AIsConnected = false;
				T_InputPins_GetPinIsConnected::Call( i, AIsConnected );
				if( ! AIsConnected )
					AChannelsMap |= ( uint32_t( 1 ) << i );

				else
					++ ADisconnectedPins;

			}
*/
			AIndex -= ADisconnectedPins;

			uint8_t ABytesPerSample = ( BitsPerSample() + 7 ) / 8;

			TArray<float> &AData = *(TArray<float>*)_Data;

			uint32_t ACopySize = AData.Size;

			SimpleList<uint8_t, uint32_t> &ABuffer = FBuffers[ AIndex ];

			uint32_t AOldSize = ABuffer.size();
			uint32_t ASampleSize = AOldSize / ABytesPerSample;

			if( ASampleSize + ACopySize > MaxSamples() )
			{
				T_OverfillOutputPin::ClockPin();
				ACopySize = MaxSamples() - ASampleSize;
			}

			ABuffer.AddCount( ACopySize * ABytesPerSample );
			uint8_t *APtr = ABuffer.AsPointer();
			APtr += AOldSize;

			uint64_t AMultiplier = ( uint32_t( 1 ) << BitsPerSample().GetValue() ) - 1;
//   			Serial.println( "MMM" );
//			Serial.println( AMultiplier, HEX );
//			Serial.println( BitsPerSample().GetValue() );

			for( int i = 0; i < ACopySize; ++ i )
			{
				int64_t AValue = ( MitovConstrain<float>( AData.Data[ i ], 0.0, 1.0 )) * AMultiplier - ( AMultiplier / 2 + 1 );

//				Serial.println( "---" );
//				Serial.println( AData.Data[ i ] );
//				Serial.println( AValue );

//				Serial.println( i );
//				auto AOldPointer = APtr;
				TAudioBuffer::WriteRawSample( AValue, APtr, ABytesPerSample );

//				auto ATest1 = TAudioBuffer::ReadRawSample( AOldPointer, ABytesPerSample );
//				Serial.println( ATest1 );

//				ATest1 = TAudioBuffer::ReadNormalizedSample( AOldPointer, ABytesPerSample, BitsPerSample() );
//				AValue = AValue + 1;
//				AValue = AValue - 1;
			}

			// Try to send
			for(;;)
			{
				uint32_t AOutSize = MaxSamples();
				for( int i = 0; i < C_COUNT_CONNECTED_InputPins; ++ i )
				{
					uint32_t ABufferSize = FBuffers[ i ].size() / ABytesPerSample;
					if( ABufferSize < MinSamples() )
						return; // Not enough samples

					if( AOutSize > ABufferSize )
						AOutSize = ABufferSize;

				}

				if( C_COUNT_CONNECTED_InputPins == 1 )
				{
					TAudioBuffer AOutBuffer( AChannelsMap, BitsPerSample(), SampleRate(), AOutSize, ABuffer.AsPointer() );
					T_OutputPin::SetPinValue( AOutBuffer );
					ABuffer.pop_front( AOutSize * ABytesPerSample );
					return;
				}

				TStoredAudioBuffer AOutBuffer( AChannelsMap, BitsPerSample(), SampleRate(), AOutSize );
				uint8_t *AStartPtr = AOutBuffer.Data;
				uint32_t AStep = ABytesPerSample * C_COUNT_CONNECTED_InputPins;

				for( int i = 0; i < C_COUNT_CONNECTED_InputPins; ++ i )
				{
					uint8_t *AInPtr = FBuffers[ i ].AsPointer();
					APtr = AStartPtr;
					AStartPtr += ABytesPerSample;

					for( int j = 0; j < AOutSize; ++ j )
					{
/*
						if( i == 1 )
						{
							AInPtr[ 0 ] = 0;
							AInPtr[ 1 ] = 1;
						}
*/
						memcpy( (char *)APtr, (char *)AInPtr, ABytesPerSample );
						APtr += AStep;
						AInPtr += ABytesPerSample;
					}

					FBuffers[ i ].pop_front( AOutSize * ABytesPerSample );
				}

				T_OutputPin::SetPinValue( AOutBuffer );
			}
		}

    };
//---------------------------------------------------------------------------
    template <
        typename T_BitsPerSampleOutputPin,
        typename T_ChannelsOutputPin,
		typename T_DataOutputPin,
		typename T_SampleRateOutputPin,
        typename T_Enabled
    > class ExtractAudioInfo :
        public T_BitsPerSampleOutputPin,
        public T_ChannelsOutputPin,
        public T_DataOutputPin,
		public T_SampleRateOutputPin,
		public T_Enabled
    {
    public:
        _V_PIN_( BitsPerSampleOutputPin )
        _V_PIN_( ChannelsOutputPin )
		_V_PIN_( DataOutputPin )
		_V_PIN_( SampleRateOutputPin )

    public:
		_V_PROP_( Enabled )

    public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled() )
				return;

			TAudioBuffer &AValue = *(TAudioBuffer*)_Data;

//            Serial.println( AValue.GetSize() );

			T_BitsPerSampleOutputPin::SetPinValue( AValue.BitsPerSample );
			T_ChannelsOutputPin::SetPinValue( AValue.ChannelsMap );
			T_SampleRateOutputPin::SetPinValue( AValue.SampleRate );

			if( T_DataOutputPin::GetPinIsConnected() )
				T_DataOutputPin::SetPinValue( Mitov::TDataBlock( AValue.GetSize(), AValue.Data ) );

		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
        typename T_IncludeFormat,
        typename T_OutputPin
    > class AudioToBinary :
		public T_Enabled,
        public T_IncludeFormat,
        public T_OutputPin
    {
	public:
        _V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )
        _V_PROP_( IncludeFormat )

	public:
		void InputPin_o_Receive( void *_Data )
        {
            if( ! Enabled() )
                return;

			TAudioBuffer &ABuffer = *(TAudioBuffer*)_Data;

			uint32_t ASize = ABuffer.GetSize();
            if( ! IncludeFormat().GetValue() )
                T_OutputPin::SetPinValue( Mitov::TDataBlock( ASize, ABuffer.Data ) );

            else
            {
				uint8_t AOffset = Func::GetVariableSize( ABuffer.ChannelsMap );
				AOffset += Func::GetVariableSize( ABuffer.BitsPerSample );

				uint8_t *ABufferData = new uint8_t[ ASize + AOffset ];

				uint32_t ADataIndex = 0;

        		Func::GetAddVariableSizeValue( ABufferData, ADataIndex, ABuffer.ChannelsMap );
                ABufferData[ ADataIndex ++ ] = ABuffer.BitsPerSample;

				memcpy( ABufferData + AOffset, ABuffer.Data, ASize );
				ASize += AOffset;

				T_OutputPin::SetPinValue( Mitov::TDataBlock( ASize, ABufferData ) );

				delete [] ABufferData;
            }

        }

    };
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		uint8_t C_INDEX
	> class TArduinoBinaryToAudioFormatChannel :
		public T_Enabled
	{
	public:
		_V_PROP_( Enabled )

	public:
		inline void GetChannelsMap( uint32_t &AValue )
		{
			if( Enabled() )
				AValue |= ( 1 << C_INDEX );

        }

	};
//---------------------------------------------------------------------------
	template <
		typename T_BitsPerSample,
		typename T_Channels_GetChannelsMap,
		typename T_ExtractFromData,
		typename T_SampleRate
	> class TArduinoBinaryToAudioFormat :
		public T_BitsPerSample,
		public T_ExtractFromData,
		public T_SampleRate
	{
	public:
		_V_PROP_( BitsPerSample )
		_V_PROP_( ExtractFromData )
		_V_PROP_( SampleRate )

	public:
		inline uint32_t GetChannelsMap()
		{
			uint32_t AValue = 0;
			T_Channels_GetChannelsMap::Call( AValue );

			return AValue;
        }

    };
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
        typename T_Format,
        typename T_OutputPin
    > class BinaryToAudio :
		public T_Enabled,
        public T_Format,
        public T_OutputPin
    {
	public:
        _V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )
        _V_PROP_( Format )

	public:
		void InputPin_o_Receive( void *_Data )
        {
			if( ! Enabled() )
                return;

			TArray<uint8_t> &ABuffer = *(TArray<uint8_t>*)_Data;
/*
            for( int i = 0; i < ABuffer.Size; ++ i )
            {
                Serial.print( ABuffer.Data[ i ] );
                Serial.print( "," );
            }
*/
			uint32_t AChannelsMap;
			uint8_t ABitsPerSample;
//			uint32_t ADataSize;
			uint8_t *ABufferData = nullptr;
			uint8_t *AData;
			uint32_t ADataSize;

			if( Format().ExtractFromData() )
			{
				if( ABuffer.Size < 3 )
					return; // Not enogh data

//				AChannelsMap = ABuffer.Data[ 0 ];

				uint32_t ADataIndex = 0;
				AChannelsMap = Func::GetVariableSizeValue( ABuffer.Data, ABuffer.Size, ADataIndex );

				ABitsPerSample = ABuffer.Data[ 1 ];
				++ ADataIndex;
//				ABitsPerSample = Func::GetVariableSizeValue( ABuffer.Data, ABuffer.Size, ADataIndex );
				uint32_t ABytesPerSample = TAudioBuffer::GetBytesPerSample( ABitsPerSample );
				uint32_t AChannelsCount = TAudioBuffer::GetChannelsCount( AChannelsMap );
//				ADataSize = Func::GetVariableSizeValue( ABuffer.Data, ABuffer.Size, ADataIndex );
				if( ABitsPerSample == 0 || AChannelsMap == 0 )
					return; // Incorrect buffer format/size

				uint32_t ASize = TAudioBuffer::GetSize( AChannelsMap, ABitsPerSample, ( ABuffer.Size - ADataIndex ) / ( ABytesPerSample * AChannelsCount ) );
				ADataSize = ASize /  ( ABytesPerSample * AChannelsCount );
				if( ASize + ADataIndex <= ABuffer.Size )
					AData = ABuffer.Data + ADataIndex;

				else
				{
					ABufferData = new uint8_t[ ASize ];
					memcpy( ABufferData, ABuffer.Data + ADataIndex, ABuffer.Size - ADataIndex );
					memset( ABufferData + ABuffer.Size - ADataIndex, 0, ASize - ( ABuffer.Size - ADataIndex ));
					AData = ABufferData;
				}

			}

            else
            {
//                Serial.println( "STEP1" );
				AChannelsMap = Format().GetChannelsMap();
				ABitsPerSample = Format().BitsPerSample();
				uint32_t ABytesPerSample = TAudioBuffer::GetBytesPerSample( ABitsPerSample );
				uint32_t AChannelsCount = TAudioBuffer::GetChannelsCount( AChannelsMap );
//                Serial.println( ASize );
//                Serial.println( ABuffer.Size );
				uint8_t AChannelsCorrection = ABuffer.Size % ( AChannelsCount * ABytesPerSample );
				if( ! AChannelsCorrection )
				{
					AData = ABuffer.Data;
//					ASize = TAudioBuffer::GetSize( AChannelsMap, ABitsPerSample, ABuffer.Size / ( ABytesPerSample * AChannelsCount ) );
					ADataSize = ABuffer.Size / ( ABytesPerSample * AChannelsCount );
				}

				else
				{
					AChannelsCorrection = ( AChannelsCount * ABytesPerSample ) - AChannelsCorrection;
					uint32_t ANewSize = ABuffer.Size + ( AChannelsCount * ABytesPerSample ) + AChannelsCorrection;
					ABufferData = new uint8_t[ ANewSize ];
					AData = ABufferData;
					memcpy( ABufferData, ABuffer.Data, ABuffer.Size );
					memset( ABufferData + ABuffer.Size, 0, AChannelsCorrection );
//					ASize = TAudioBuffer::GetSize( AChannelsMap, ABitsPerSample, ANewSize / ( ABytesPerSample * AChannelsCount ) );
					ADataSize = ANewSize / ( ABytesPerSample * AChannelsCount );
				}
			}

			T_OutputPin::SetPinValue( TAudioBuffer( AChannelsMap, ABitsPerSample, Format().SampleRate(), ADataSize, AData ) );

			if( ABufferData )
				delete [] ABufferData;

        }

    };
//---------------------------------------------------------------------------
    namespace TArduinoAudioGeneratorSignalType
    {
        enum TArduinoAudioGeneratorSignalType
        {
            Tone,
            Triangle,
            Square,
            SawToothUp,
            SawToothDown
        };
    }
//---------------------------------------------------------------------------
    template <
        typename T_BitsPerSample,
        typename T_BufferSize,
        typename T_Channels_GetChannelsMap,
        typename T_SampleRate
    > class TArduinoAudioFormat :
        public T_BitsPerSample,
        public T_BufferSize,
        public T_Channels_GetChannelsMap,
        public T_SampleRate
    {
	public:
		_V_PROP_( BitsPerSample )
		_V_PROP_( BufferSize )
		_V_PROP_( SampleRate )

	public:
		uint8_t GetChannelMap()
		{
			uint32_t AChannelsMap = 0;
			T_Channels_GetChannelsMap::Call( AChannelsMap );
            return AChannelsMap;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Amplitude,
		typename T_Asymmetry,
		typename T_Enabled,
		typename T_Format,
		typename T_Frequency,
		typename T_OutputPin,
		typename T_Phase,
		typename T_SignalType,
		typename T_WhenDisabled
	> class AudioGenerator :
		public T_Amplitude,
		public T_Asymmetry,
		public T_Enabled,
		public T_Format,
		public T_Frequency,
		public T_OutputPin,
		public T_Phase,
		public T_SignalType,
		public T_WhenDisabled
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Amplitude )
		_V_PROP_( Asymmetry )
		_V_PROP_( Enabled )
		_V_PROP_( Format )
		_V_PROP_( Frequency )
		_V_PROP_( Phase )
		_V_PROP_( SignalType )
		_V_PROP_( WhenDisabled )

	protected:
		float	FPhase;

	public:
		inline void UpdateEnabled()
		{
			if( Enabled() )
			{
				WhenDisabled().FCycleCompleted() = false;
				if( WhenDisabled().Reset() )
					FPhase = 0.0;

			}

			else if( WhenDisabled().Reset() )
				ResetOutput();

		}

	protected:
		void ResetOutput()
		{
			WhenDisabled().FCycleCompleted() = true;
			FPhase = 0;
		}

	public:
		inline void PeriodicCall()
		{
			if( ! Enabled() )
				if( WhenDisabled().FCycleCompleted().GetValue() )
					return;

			uint8_t AChannelsMap = Format().GetChannelMap();

			uint8_t ACountChannels = 0;
			for( int i = 0; i < 8; ++ i )
				if( AChannelsMap & ( 1 << i ))
					++ ACountChannels;

			uint8_t ABytes = ( Format().BitsPerSample() + 7 ) / 8;

			uint8_t *ABuffer = new uint8_t[ ACountChannels * ABytes * Format().BufferSize() ];

			uint32_t AMaxValue = 0x7FFFFFFFul >> ( 32 - Format().BitsPerSample() );
			float   AAmplitude = Amplitude().GetValue() * AMaxValue;
//			float   AOffset = 0xFFFFFFFFul / 2 + 0.5;

			uint8_t *APtr = ABuffer;

			float APeriod = 1000000 / Frequency();
//			float APeriod = Format().SampleRate().GetValue() / Frequency();
			float ATime = 1000000 / Format().SampleRate().GetValue();
			ATime /= APeriod;
//			float APhase = FPhase + ATime;

			uint32_t AOutputBufferSize;
			if( ( ! Enabled().GetValue() ) && WhenDisabled().CompleteCycle() )
                AOutputBufferSize = 0;

			else
			 	AOutputBufferSize = Format().BufferSize();

			switch( SignalType() )
			{
				case Mitov::TArduinoAudioGeneratorSignalType::Tone :
				{
//					float APhaseStep = ( Frequency() / Format().SampleRate().GetValue() ) * 2 * MITOV_PI;
					for( int i = 0; i < Format().BufferSize(); ++ i )
					{
						int32_t AValue = ::Mitov::Func::Round( sin( ( V_FMOD( FPhase + Phase(), 1.0f ) ) * 2 * PI ) * AAmplitude );
						for( int j = 0; j < ACountChannels; ++ j )
							TAudioBuffer::WriteRawSample( AValue, APtr, ABytes );

						auto APhase = V_FMOD( FPhase + ATime, 1.0f );
						if( ( ! Enabled().GetValue() ) && WhenDisabled().CompleteCycle() )
						{
							++ AOutputBufferSize;
							if(( FPhase == 0.0 ) || ( APhase < FPhase ))
							{
	//							Serial.println( "STEP1" );
								ResetOutput();
								break;
							}
						}

						FPhase = APhase;
					}

					break;
				}

				case Mitov::TArduinoAudioGeneratorSignalType::Triangle :
				{
//					float AFrequency = Frequency() / Format().SampleRate().GetValue();
					const float AAssymetryPount = 0.5 + Asymmetry() / 2;
//					float APhase = ( FPhase + Phase() ) / ( 2 * MITOV_PI );
					for( int i = 0; i < Format().BufferSize(); ++ i )
					{
						int32_t AValue;
						if( FPhase < AAssymetryPount )
							AValue = ::Mitov::Func::Round( AAmplitude * 2 * ( FPhase / AAssymetryPount ));

						else
							AValue = ::Mitov::Func::Round( AAmplitude * 2 * ( 1 - (( FPhase - AAssymetryPount ) / ( 1 - AAssymetryPount ))));

						AValue -= AAmplitude + 1;

						for( int j = 0; j < ACountChannels; ++ j )
							TAudioBuffer::WriteRawSample( AValue, APtr, ABytes );

						auto APhase = V_FMOD( FPhase + ATime, 1.0f );
						if( ( ! Enabled().GetValue() ) && WhenDisabled().CompleteCycle() )
						{
							++ AOutputBufferSize;
							if(( FPhase == 0.0 ) || ( APhase < FPhase ))
							{
	//							Serial.println( "STEP1" );
								ResetOutput();
								break;
							}
						}

						FPhase = APhase;
					}

//					FPhase = APhase * 2 * MITOV_PI;
					break;
				}

				case Mitov::TArduinoAudioGeneratorSignalType::Square :
				{
					const float AAssymetryPount = 0.5 + Asymmetry() / 2;
					for( int i = 0; i < Format().BufferSize(); ++ i )
					{
						int32_t AValue;
						if( V_FMOD( ( FPhase + Phase() ), 1.0f ) < AAssymetryPount )
							AValue = - AAmplitude;

						else
							AValue = AAmplitude;

						for( int j = 0; j < ACountChannels; ++ j )
							TAudioBuffer::WriteRawSample( AValue, APtr, ABytes );

						auto APhase = V_FMOD( FPhase + ATime, 1.0f );
						if( ( ! Enabled().GetValue() ) && WhenDisabled().CompleteCycle() )
						{
							++ AOutputBufferSize;
							if(( FPhase == 0.0 ) || ( APhase < FPhase ))
							{
	//							Serial.println( "STEP1" );
								ResetOutput();
								break;
							}
						}

						FPhase = APhase;
					}

					break;
				}

			}

			TAudioBuffer AOutBuffer( AChannelsMap, Format().BitsPerSample(), Format().SampleRate(), AOutputBufferSize, ABuffer );
			T_OutputPin::SetPinValue( AOutBuffer );

            delete [] ABuffer;
        }

	public:
		void ClockInputPin_o_Receive( void *_Data )
		{
			PeriodicCall();
		}

		inline void ResetInputPin_o_Receive( void *_Data )
		{
            FPhase = 0.0f;
        }

	public:
		inline void SystemStart()
		{
			FPhase = 0.0f;
		}

    };
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Value
	> class ChangeAudioFormatOptionalValue :
		public T_Enabled,
		public T_Value
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( Value )

	};
//---------------------------------------------------------------------------
	template <
		typename T_Channels_GetChannelsMap,
		typename T_Enabled
	> class ChangeAudioFormatChannels :
		public T_Enabled
	{
	public:
		_V_PROP_( Enabled )

	public:
		inline uint32_t GetChannelsMap()
		{
			uint32_t AValue = 0;
			T_Channels_GetChannelsMap::Call( AValue );

			return AValue;
        }
    };
//---------------------------------------------------------------------------
	template <
		typename T_BitsPerSample,
//		typename T_BufferSize,
		typename T_Channels,
		typename T_Enabled,
		typename T_OutputPin,
		typename T_SampleRate
	> class ChangeAudioFormat :
		public T_BitsPerSample,
//		public T_BufferSize,
		public T_Channels,
		public T_Enabled,
		public T_OutputPin,
		public T_SampleRate
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( BitsPerSample )
//		_V_PROP_( BufferSize )
		_V_PROP_( Channels )
		_V_PROP_( Enabled )
		_V_PROP_( SampleRate )


	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled() )
			{
	            T_OutputPin::PinNotify( _Data );
				return;
            }

			TAudioBuffer &AInBuffer = *(TAudioBuffer *)_Data;

			uint32_t AChannelsMap = Channels().GetChannelsMap();
			uint8_t AOutChannelsCount = TAudioBuffer::GetChannelsCount( AChannelsMap );

			if( ( ! SampleRate().Enabled().GetValue() ) || ( SampleRate().Value().GetValue() == AInBuffer.SampleRate ) )
			{
				uint8_t AInChannelsCount = AInBuffer.GetChannelsCount();
				if( ( ! BitsPerSample().Enabled().GetValue() ) || ( BitsPerSample().Value().GetValue() == AInBuffer.BitsPerSample ) )
  //					if( ( ! BufferSize().Enabled().GetValue() ) || ( BufferSize().Value().GetValue() == AInBuffer.Size ) )
				{
					if( ( ! Channels().Enabled().GetValue() ) || ( AChannelsMap == AInBuffer.ChannelsMap ) )
					{
						T_OutputPin::PinNotify( _Data );
						return;
					}

					if( AInChannelsCount == AOutChannelsCount )
					{
						TAudioBuffer AOutBuffer( AChannelsMap, AInBuffer.BitsPerSample, AInBuffer.SampleRate, AInBuffer.Size, AInBuffer.Data );
						T_OutputPin::SetPinValue( AOutBuffer );
						return;
					}
				}

				TStoredAudioBuffer AOutBuffer( AChannelsMap, ( BitsPerSample().Enabled().GetValue() ) ? BitsPerSample().Value().GetValue() : AInBuffer.SampleRate, AInBuffer.SampleRate, AInBuffer.Size );

				uint8_t *ABaseInPtr = AInBuffer.Data;
				uint8_t *AaseOutPtr = AOutBuffer.Data;

				uint32_t AInStep = AInBuffer.GetSampleStep();
				uint32_t AOutStep = AOutBuffer.GetSampleStep();

				uint8_t  AInBytesPerSample = AInBuffer.GetBytesPerSample();
				uint8_t  AOutBytesPerSample = AOutBuffer.GetBytesPerSample();

				uint8_t AZeroSize = ( AOutChannelsCount - AInChannelsCount ) * AOutBytesPerSample;
//						Serial.println( AZeroSize );

				for( int i = 0; i < AInBuffer.Size; ++ i )
				{
					TAudioBuffer::CopySample( ABaseInPtr, AaseOutPtr, AInStep, AOutStep, AInChannelsCount, AOutChannelsCount, AInBytesPerSample, AOutBytesPerSample, AInBuffer.BitsPerSample, AOutBuffer.BitsPerSample, AZeroSize );

					ABaseInPtr += AInStep;
					AaseOutPtr += AOutStep;
				}

				T_OutputPin::SetPinValue( AOutBuffer );
				return;
			}

			else
			{
				uint32_t AOutSampleRate = SampleRate().Value().GetValue();
				float ARatio = float( AOutSampleRate ) / AInBuffer.SampleRate;
				float AReverseRatio = float( AInBuffer.SampleRate ) / AOutSampleRate;

                uint32_t AOutBufferSize = AInBuffer.Size * ARatio + 0.5;

				TStoredAudioBuffer AOutBuffer(
					( Channels().Enabled().GetValue() ) ? AChannelsMap : AInBuffer.ChannelsMap,
					( BitsPerSample().Enabled().GetValue() ) ? BitsPerSample().Value().GetValue() : AInBuffer.BitsPerSample,
					AOutSampleRate, AOutBufferSize );

				uint8_t *ABaseInPtr = AInBuffer.Data;
				uint8_t *ABaseOutPtr = AOutBuffer.Data;

				uint32_t AInStep = AInBuffer.GetSampleStep();
				uint32_t AOutStep = AOutBuffer.GetSampleStep();

				uint8_t  AInBytesPerSample = AInBuffer.GetBytesPerSample();
				uint8_t  AOutBytesPerSample = AOutBuffer.GetBytesPerSample();

				uint8_t AInChannelsCount = AInBuffer.GetChannelsCount();

				uint32_t AZeroSize = ( AOutChannelsCount - AInChannelsCount ) * AOutBytesPerSample;

				for( int i = 0; i < AOutBufferSize; ++ i )
				{
					TAudioBuffer::CopySample( ABaseInPtr + MitovMin<uint32_t>( AInBuffer.Size - 1, i * AReverseRatio ) * AInStep, ABaseOutPtr, AInStep, AOutStep, AInChannelsCount, AOutChannelsCount, AInBytesPerSample, AOutBytesPerSample, AInBuffer.BitsPerSample, AOutBuffer.BitsPerSample, AZeroSize );
					ABaseOutPtr += AOutStep;
				}

				T_OutputPin::SetPinValue( AOutBuffer );
			}
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
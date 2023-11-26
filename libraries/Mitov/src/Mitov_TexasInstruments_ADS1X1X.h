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
	namespace ADS1X1XConst
	{
		const uint8_t REG_POINTER_CONVERT = 0x00;
		const uint8_t REG_POINTER_CONFIG = 0x01;
		const uint8_t REG_POINTER_LOWTHRESH = 0x02;
		const uint8_t REG_POINTER_HITHRESH = 0x03;
	}
//---------------------------------------------------------------------------
	template <
		typename T_ActiveHigh,
		typename T_Enabled,
		typename T_Latching,
		typename T_QueueSize,
		typename T_ThresholdHigh,
		typename T_ThresholdLow,
		typename T_UseAsReady,
		typename T_WindowMode
	> class TArduinoTexasInstrumentsADS1X1XComparator :
		public T_ActiveHigh,
		public T_Enabled,
		public T_Latching,
		public T_QueueSize,
		public T_ThresholdHigh,
		public T_ThresholdLow,
		public T_UseAsReady,
		public T_WindowMode
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( WindowMode )
		_V_PROP_( Latching )
		_V_PROP_( ActiveHigh )
		_V_PROP_( UseAsReady )
		_V_PROP_( QueueSize )
		_V_PROP_( ThresholdHigh )
		_V_PROP_( ThresholdLow )

	};
//---------------------------------------------------------------------------
	template<
        typename T_OWNER, T_OWNER &C_OWNER,
        typename T_Differential,
        typename T_Enabled,
		typename T_FClocked,
		uint8_t C_INDEX,
		bool C_Is12bit,
		typename T_OutputPin,
		typename T_Range,
		typename T_SampleRate
    > class TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel :
        public T_Differential,
        public T_Enabled,
		public T_FClocked,
		public T_OutputPin,
		public T_Range,
		public T_SampleRate
    {
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Differential )
		_V_PROP_( Enabled )
		_V_PROP_( Range )
		_V_PROP_( SampleRate )

	protected:
		_V_PROP_( FClocked )

	public:
        void UpdateConfig()
        {
            C_OWNER.UpdateChannelConfig( C_INDEX );
        }

	public:
		inline void GetIndex( uint8_t &AInIndex, uint8_t & AOutIndex )
        {
            if( ! Enabled().GetValue() )
		        return;

		    if( ! FClocked().GetValue() )
		        return;

            if( ! AInIndex )
    		    AOutIndex = C_INDEX;

			-- AInIndex;
		}

		inline void SetValue( float AValue )
		{
   	        FClocked() = false;
			T_OutputPin::SetPinValue( AValue );
		}

		inline void GetCountActiveChannels( uint8_t &ACount )
		{
		    if( Enabled() )
		        if( FClocked() )
		            ++ ACount;

		}

		inline void GetConfig( uint16_t &AValue )
		{
		    if( C_Is12bit )
		    {
//		          000 : 128 SPS
//                001 : 250 SPS
//                010 : 490 SPS
//                011 : 920 SPS
//                100 : 1600 SPS (default)
//                101 : 2400 SPS
//                1110 111 : 3300 SPS


                if( SampleRate().GetValue() > 920 ) // 1600, 2400, 3300
                {
                    if( SampleRate().GetValue() > 2400 )
        			        AValue |= ( 0b110 << 5 ); // 3300

        			else
        			{
        			    if( SampleRate().GetValue() > 1600 )
        			        AValue |= ( 0b101 << 5 ); // 2400

        			    else
        			        AValue |= ( 0b100 << 5 ); // 1600

                    }
                }

                else // 128, 250, 490, 920
                {

                    if( SampleRate().GetValue() > 250 ) // 490, 920
                    {
                        if( SampleRate().GetValue() > 490 )
                            AValue |= ( 0b011 << 5 ); // 920

                        else
                            AValue |= ( 0b010 << 5 ); // 490

                    }

                    else // 128, 250
                    {
                        if( SampleRate().GetValue() > 128 )
                            AValue |= ( 0b001 << 5 ); // 250

//                        else
//                            AValue |= ( 0b000 << 5 ); // 128

                    }

                }

		    }

		    else // 16 Bit
            {
    			if( SampleRate().GetValue() > 64 ) // 128, 250, 475, 860
    			{
    			    if( SampleRate().GetValue() > 250 ) // 475, 860
    			    {
    			        if( SampleRate().GetValue() > 475 )
        			        AValue |= ( 0b111 << 5 ); // 860

        			    else
        			        AValue |= ( 0b110 << 5 ); // 475

    			    }

    			    else // 128, 250
    			    {
    			        if( SampleRate().GetValue() > 128 )
                            AValue |= ( 0b101 << 5 ); // 250

                        else
                            AValue |= ( 0b100 << 5 ); // 128

    			    }
    			}

    			else
                {
    			    if( SampleRate().GetValue() > 16 ) // 32, 64
    			    {
    			        if( SampleRate().GetValue() > 32 )
                            AValue |= ( 0b011 << 5 ); // 64

                        else
                            AValue |= ( 0b010 << 5 ); // 32

    			    }

    			    else // 8, 16
    			    {
    			        if( SampleRate().GetValue() > 8 )
                            AValue |= ( 0b001 << 5 ); // 16

    //                    else
    //                        AValue |= ( 0b000 << 5 ); // 8

    			    }
                }
            }

//            Serial.println( Range().GetValue() );
			if( Range().GetValue() > 1.024f )
			{
			    if( Range().GetValue() <= 4.096f )
			    {
			        if( Range().GetValue() <= 2.048f )
			            AValue |= ( 0b010 << 9 ); // 2.048

			        else
			            AValue |= ( 0b001 << 9 ); // 4.096

			    }

//			    else
//                  AValue |= ( 0b000 << 9 ); // 6.144
			}

			else
            {
                if( Range().GetValue() <= 0.512f )
                {
                    if( Range().GetValue() <= 0.256f )
        	            AValue |= ( 0b101 << 9 ); // 0.256

                    else
        	            AValue |= ( 0b100 << 9 ); // 0.512

                }

                else
    	            AValue |= ( 0b011 << 9 ); // 1.024

            }

		}

	public:
	    void ClockInputPin_o_Receive( void *_Data )
	    {
	        FClocked() = true;
	    }

	public:
	    inline TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel()
	    {
	        FClocked() = false;
	    }

    };
//---------------------------------------------------------------------------
	template<
        typename T_OWNER, T_OWNER &C_OWNER,
        typename T_Differential,
        typename T_Enabled,
		typename T_FClocked,
		uint8_t C_INDEX,
		bool C_Is12bit,
		typename T_OutputPin,
		typename T_Range,
		typename T_SampleRate
    > class TArduinoTexasInstrumentsMultiChannelADS1X15_Channel :
        public TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel <
                T_OWNER, C_OWNER,
                T_Differential,
                T_Enabled,
        		T_FClocked,
        		C_INDEX,
        		C_Is12bit,
        		T_OutputPin,
        		T_Range,
        		T_SampleRate
            >
    {
        typedef TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel <
                T_OWNER, C_OWNER,
                T_Differential,
                T_Enabled,
        		T_FClocked,
        		C_INDEX,
        		C_Is12bit,
        		T_OutputPin,
        		T_Range,
        		T_SampleRate
            > inherited;

    public:
		inline void GetConfig( uint16_t &AValue )
		{
		    inherited::GetConfig( AValue );

            if( inherited::Differential().GetValue() )
				AValue |= ( uint16_t( C_INDEX + 1 ) << 12 );

			else
			    AValue |= ( uint16_t( 1 ) << 14 ) | ( uint16_t( C_INDEX ) << 12 );

		}

    };
//---------------------------------------------------------------------------
	template<
        typename T_OWNER, T_OWNER &C_OWNER,
        typename T_Differential,
        typename T_Enabled,
		typename T_FClocked,
		uint8_t C_INDEX,
		bool C_Is12bit,
		typename T_OutputPin,
		typename T_PairWithChannel3,
		typename T_Range,
		typename T_SampleRate
    > class TArduinoTexasInstrumentsMultiChannelADS1X15_Channel_0 :
        public TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel <
                T_OWNER, C_OWNER,
                T_Differential,
                T_Enabled,
        		T_FClocked,
        		C_INDEX,
        		C_Is12bit,
        		T_OutputPin,
        		T_Range,
        		T_SampleRate
            >,
		public T_PairWithChannel3
    {
        typedef TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel <
                T_OWNER, C_OWNER,
                T_Differential,
                T_Enabled,
        		T_FClocked,
        		C_INDEX,
        		C_Is12bit,
        		T_OutputPin,
        		T_Range,
        		T_SampleRate
            > inherited;

    public:
        _V_PROP_( PairWithChannel3 )

    public:
		inline void GetConfig( uint16_t &AValue )
		{
		    inherited::GetConfig( AValue );

            if( inherited::Differential().GetValue() )
            {
                if( PairWithChannel3() )
				    AValue |= ( uint16_t( C_INDEX + 1 ) << 12 );

				else
				    AValue |= ( uint16_t( C_INDEX ) << 12 );

		    }

			else
			    AValue |= ( uint16_t( 1 ) << 14 ) | ( uint16_t( C_INDEX ) << 12 );

		}

    };
//---------------------------------------------------------------------------
	template<
        typename T_OWNER, T_OWNER &C_OWNER,
        typename T_Differential,
        typename T_Enabled,
		typename T_FClocked,
		uint8_t C_INDEX,
		bool C_Is12bit,
		typename T_OutputPin,
		typename T_PairWithChannel,
		typename T_Range,
		typename T_SampleRate
    > class TArduinoTexasInstrumentsMultiChannelADS1X15_Channel_3 :
        public TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel <
                T_OWNER, C_OWNER,
                T_Differential,
                T_Enabled,
        		T_FClocked,
        		C_INDEX,
                C_Is12bit,
        		T_OutputPin,
        		T_Range,
        		T_SampleRate
            >,
		public T_PairWithChannel
    {
        typedef TArduinoTexasInstrumentsMultiChannelADS1X15_Basic_Channel <
                T_OWNER, C_OWNER,
                T_Differential,
                T_Enabled,
        		T_FClocked,
        		C_INDEX,
                C_Is12bit,
        		T_OutputPin,
        		T_Range,
        		T_SampleRate
            > inherited;

    public:
        _V_PROP_( PairWithChannel )

    public:
		inline void GetConfig( uint16_t &AValue )
		{
		    inherited::GetConfig( AValue );

            if( inherited::Differential().GetValue() )
			    AValue |= ( uint16_t( PairWithChannel().GetValue() + 1 ) << 12 );

			else
			    AValue |= ( uint16_t( 1 ) << 14 ) | ( uint16_t( C_INDEX ) << 12 );

		}

    };
//---------------------------------------------------------------------------
	template<
	    typename T_Enabled,
	    typename T_Index
	> class TArduinoTexasInstrumentsADS1X15ChannelIndex :
	    public T_Enabled,
	    public T_Index
	{
	public:
		_V_PROP_( Enabled )
	    _V_PROP_( Index )

	};
//---------------------------------------------------------------------------
	template<
		typename T_I2C, T_I2C &C_I2C,
		typename T_Address,
//		uint8_t C_COUNT_Channels,
		typename T_ChannelIndex,
        typename T_Channels_GetConfig,
		typename T_Channels_GetCountActiveChannels,
		typename T_Channels_GetIndex,
		typename T_Channels_SetValue,
		typename T_Comparator,
//		typename T_Differential,
		typename T_Enabled,
		typename T_FClocked,
		typename T_FIndex,
//		typename T_Range,
//		typename T_SampleRate,
		typename T_SingleShot
	> class TexasInstrumentsMultiChannelADS1X15 :
		public T_Address,
		public T_ChannelIndex,
		public T_Comparator,
//		public T_Differential,
		public T_Enabled,
		public T_FClocked,
		public T_FIndex,
//		public T_Range,
//		public T_SampleRate,
		public T_SingleShot
	{
	public:
		_V_PROP_( Address )
	    _V_PROP_( ChannelIndex )
		_V_PROP_( Comparator )
		_V_PROP_( Enabled )
//		_V_PROP_( Range )
//		_V_PROP_( Differential )
		_V_PROP_( SingleShot )
//		_V_PROP_( SampleRate )

	public:
		_V_PROP_( FIndex )
		_V_PROP_( FClocked )

	public:
		void UpdateConfig()
		{
			if( Comparator().UseAsReady() )
			{
				writeRegister( ADS1X1XConst::REG_POINTER_LOWTHRESH, 0x8000 );
				writeRegister( ADS1X1XConst::REG_POINTER_HITHRESH, 0x7FFF );
			}

			else
			{
				uint16_t AHigh = Comparator().ThresholdHigh() * 0x7FFF + 0.5;
				uint16_t ALow = Comparator().ThresholdLow() * 0x7FFF + 0.5;

				if( ALow > AHigh )
				{
					writeRegister( ADS1X1XConst::REG_POINTER_LOWTHRESH, AHigh );
					writeRegister( ADS1X1XConst::REG_POINTER_HITHRESH, ALow );
				}

				else
				{
					writeRegister( ADS1X1XConst::REG_POINTER_LOWTHRESH, ALow );
					writeRegister( ADS1X1XConst::REG_POINTER_HITHRESH, AHigh );
				}
			}

			StartRead( false );
		}

		void UpdateChannelConfig( uint8_t AIndex )
		{
		    if( ! ChannelIndex().Enabled())
		        return;

            if( SingleShot() )
		        return;

		    if( AIndex != ChannelIndex().Index().GetValue() )
		        return;

            StartRead();
		}

	protected:
		void writeRegister( uint8_t AReg, uint16_t AValue )
		{
			C_I2C.beginTransmission( uint8_t( Address() ));
			C_I2C.write( uint8_t( AReg ));
			C_I2C.write( uint8_t( AValue >> 8 ));
			C_I2C.write( uint8_t( AValue & 0xFF ));
			C_I2C.endTransmission();
		}

		int16_t readRegister( uint8_t AReg )
		{
			C_I2C.beginTransmission( uint8_t( Address() ));
			C_I2C.write( AReg );
			C_I2C.endTransmission();
			C_I2C.requestFrom( uint8_t( Address() ), uint8_t( 2 ));
			return ((int16_t( int8_t( C_I2C.read() )) << 8) | C_I2C.read());
		}

	protected:
		void StartRead( bool AStartRead )
		{
		    uint8_t	AChannelIndex;
		    if( ChannelIndex().Enabled())
		        AChannelIndex = ChannelIndex().Index();

		    else
		    {
			    AChannelIndex = 255;
    			uint8_t AInIndex = FIndex().GetValue(); // Needs to be captured by reference!
			    T_Channels_GetIndex::Call( AInIndex, AChannelIndex );

			    if( AChannelIndex == 255 )
			        return;

			}

	        uint16_t AValue =   (( AStartRead ) ? ( uint16_t( 1 ) << 15 ) : 0 ) |
								( Comparator().Latching() ? ( uint16_t( 1 ) << 2 ) : 0 ) |
								( Comparator().ActiveHigh() ? ( uint16_t( 1 ) << 3 ) : 0 ) |
								( Comparator().WindowMode() ? ( uint16_t( 1 ) << 4 ) : 0 ) |
//								( uint16_t( ASampleRate ) << 5 ) |
								( ( SingleShot() || ( ! ChannelIndex().Enabled().GetValue() )) ? uint16_t( 1 ) << 8 : 0 );
//								( uint16_t( AChannelIndex & 0b11 ) << 12 );
//								( Differential() ? 0 : ( uint16_t( 1 ) << 14 ) );

//            Serial.println( AChannelIndex );
            T_Channels_GetConfig::Call( AChannelIndex, AValue );

//            Serial.println( AValue, BIN );

            FClocked() = true;
		    writeRegister( ADS1X1XConst::REG_POINTER_CONFIG, AValue );
		}

	public:
		inline void SystemStart()
		{
			FIndex() = 0;
			UpdateConfig();
		    if( Enabled().GetValue() )
			    StartRead( true );

		}

		inline void SystemLoopBegin()
		{
		    if( ! Enabled().GetValue() )
		        return;

		    uint16_t AStatus = readRegister( ADS1X1XConst::REG_POINTER_CONFIG );
//			Serial.println( AStatus, HEX );
            if( SingleShot() || ( ! ChannelIndex().Enabled().GetValue() ) )
    			if(( AStatus & 0x8000 ) == 0 )
    				return;

            uint8_t ACountActive = 0;
	        T_Channels_GetCountActiveChannels::Call( ACountActive );
  	        if( ! ACountActive )
   	            return;

            if( FClocked() || ( ! SingleShot().GetValue() ))
            {
                FClocked() = false;
			    int16_t	AValue = readRegister( ADS1X1XConst::REG_POINTER_CONVERT ); // >> 4;
//			    Serial.println( AValue );

    			float AFloatValue = float( AValue ) / 0x7FFF;

//            uint8_t AChannelIndex = C_OWNER.Analog_GetIndexCall( FIndex().GetValue() );
                uint8_t	AChannelIndex;
	    	    if( ChannelIndex().Enabled())
		            AChannelIndex = ChannelIndex().Index();

    		    else
	    	    {
    			    AChannelIndex = 255;
    			    uint8_t AInIndex = FIndex().GetValue(); // Needs to be captured by reference!
    			    T_Channels_GetIndex::Call( AInIndex, AChannelIndex );

    			    if( AChannelIndex == 255 )
    			        return;

    			}
//              Serial.println( AChannelIndex );

//			C_OWNER.Analog_SetValue( AChannelIndex + 2, AFloatValue );
//            Serial.println( AChannelIndex );
    			T_Channels_SetValue::Call( AChannelIndex, AFloatValue );

        	    FIndex() = FIndex() + 1;

        	    if( FIndex() >= ACountActive ) //C_OWNER.GetAnalogChannelsCount() )
    	            FIndex() = 0;

    		}

            if( SingleShot() || ( ! ChannelIndex().Enabled().GetValue() ) )
			    StartRead( true );

		}

	public:
	    inline TexasInstrumentsMultiChannelADS1X15()
	    {
	        FClocked() = false;
	    }

	};
//---------------------------------------------------------------------------
    template <
		typename T_I2C, T_I2C &C_I2C,
		typename T_Address,
		typename T_Enabled,
		typename T_FClocked,
		typename T_FReadClocked,
		bool C_Is12bit,
		typename T_OutputPin,
		typename T_Range,
		typename T_SampleRate,
		typename T_SingleShot
    > class TexasInstrumentsADS1X13 :
		public T_Address,
		public T_Enabled,
		public T_FClocked,
		public T_FReadClocked,
		public T_OutputPin,
		public T_Range,
		public T_SampleRate,
		public T_SingleShot
    {
	public:
		_V_PROP_( Address )
		_V_PROP_( Enabled )
		_V_PROP_( OutputPin )
		_V_PROP_( Range )
		_V_PROP_( SampleRate )
		_V_PROP_( SingleShot )

    protected:
		_V_PROP_( FClocked )
		_V_PROP_( FReadClocked )

	protected:
		void writeRegister( uint8_t AReg, uint16_t AValue )
		{
			C_I2C.beginTransmission( uint8_t( Address() ));
			C_I2C.write( uint8_t( AReg ));
			C_I2C.write( uint8_t( AValue >> 8 ));
			C_I2C.write( uint8_t( AValue & 0xFF ));
			C_I2C.endTransmission();
		}

		int16_t readRegister( uint8_t AReg )
		{
			C_I2C.beginTransmission( uint8_t( Address() ));
			C_I2C.write( AReg );
			C_I2C.endTransmission();
			C_I2C.requestFrom( uint8_t( Address() ), uint8_t( 2 ));
			return ((int16_t( int8_t( C_I2C.read() )) << 8) | C_I2C.read());
		}

	protected:
		void StartRead()
		{
	        uint16_t AValue =   ( uint16_t( 1 ) << 15 ) |
////								( Comparator().Latching() ? ( uint16_t( 1 ) << 2 ) : 0 ) |
////								( Comparator().ActiveHigh() ? ( uint16_t( 1 ) << 3 ) : 0 ) |
////								( Comparator().WindowMode() ? ( uint16_t( 1 ) << 4 ) : 0 ) |
//								( uint16_t( ASampleRate ) << 5 ) |
								(( SingleShot() ) ? uint16_t( 1 ) << 8 : 0 );
//								( uint16_t( AChannelIndex & 0b11 ) << 12 );
//								( Differential() ? 0 : ( uint16_t( 1 ) << 14 ) );

//            Serial.println( AChannelIndex );
////            T_Channels_GetConfig::Call( AChannelIndex, AValue );

//            Serial.println( AValue, BIN );
		    if( C_Is12bit )
		    {
//		          000 : 128 SPS
//                001 : 250 SPS
//                010 : 490 SPS
//                011 : 920 SPS
//                100 : 1600 SPS (default)
//                101 : 2400 SPS
//                1110 111 : 3300 SPS


                if( SampleRate().GetValue() > 920 ) // 1600, 2400, 3300
                {
                    if( SampleRate().GetValue() > 2400 )
        			        AValue |= ( 0b110 << 5 ); // 3300

        			else
        			{
        			    if( SampleRate().GetValue() > 1600 )
        			        AValue |= ( 0b101 << 5 ); // 2400

        			    else
        			        AValue |= ( 0b100 << 5 ); // 1600

                    }
                }

                else // 128, 250, 490, 920
                {

                    if( SampleRate().GetValue() > 250 ) // 490, 920
                    {
                        if( SampleRate().GetValue() > 490 )
                            AValue |= ( 0b011 << 5 ); // 920

                        else
                            AValue |= ( 0b010 << 5 ); // 490

                    }

                    else // 128, 250
                    {
                        if( SampleRate().GetValue() > 128 )
                            AValue |= ( 0b001 << 5 ); // 250

//                        else
//                            AValue |= ( 0b000 << 5 ); // 128

                    }

                }

		    }

		    else // 16 Bit
            {
    			if( SampleRate().GetValue() > 64 ) // 128, 250, 475, 860
    			{
    			    if( SampleRate().GetValue() > 250 ) // 475, 860
    			    {
    			        if( SampleRate().GetValue() > 475 )
        			        AValue |= ( 0b111 << 5 ); // 860

        			    else
        			        AValue |= ( 0b110 << 5 ); // 475

    			    }

    			    else // 128, 250
    			    {
    			        if( SampleRate().GetValue() > 128 )
                            AValue |= ( 0b101 << 5 ); // 250

                        else
                            AValue |= ( 0b100 << 5 ); // 128

    			    }
    			}

    			else
                {
    			    if( SampleRate().GetValue() > 16 ) // 32, 64
    			    {
    			        if( SampleRate().GetValue() > 32 )
                            AValue |= ( 0b011 << 5 ); // 64

                        else
                            AValue |= ( 0b010 << 5 ); // 32

    			    }

    			    else // 8, 16
    			    {
    			        if( SampleRate().GetValue() > 8 )
                            AValue |= ( 0b001 << 5 ); // 16

    //                    else
    //                        AValue |= ( 0b000 << 5 ); // 8

    			    }
                }
            }

//            Serial.println( Range().GetValue() );
			if( Range().GetValue() > 1.024f )
			{
			    if( Range().GetValue() <= 4.096f )
			    {
			        if( Range().GetValue() <= 2.048f )
			            AValue |= ( 0b010 << 9 ); // 2.048

			        else
			            AValue |= ( 0b001 << 9 ); // 4.096

			    }

//			    else
//                  AValue |= ( 0b000 << 9 ); // 6.144
			}

			else
            {
                if( Range().GetValue() <= 0.512f )
                {
                    if( Range().GetValue() <= 0.256f )
        	            AValue |= ( 0b101 << 9 ); // 0.256

                    else
        	            AValue |= ( 0b100 << 9 ); // 0.512

                }

                else
    	            AValue |= ( 0b011 << 9 ); // 1.024

            }


            FReadClocked() = true;

//            Serial.println( AValue, BIN );
		    writeRegister( ADS1X1XConst::REG_POINTER_CONFIG, AValue );
		}

	public:
	    void ClockInputPin_o_Receive( void *_Data )
	    {
	        FClocked() = true;
	    }

	public:
		inline void SystemStart()
		{
//			FIndex() = 0;
//            UpdateConfig();
//delay( 3000 );
		    if( Enabled().GetValue() )
			    StartRead();

		}

		inline void SystemLoopBegin()
		{
		    if( ! Enabled().GetValue() )
		        return;

//            Serial.println( "TEST1" );
		    uint16_t AStatus = readRegister( ADS1X1XConst::REG_POINTER_CONFIG );
//			Serial.println( AStatus, HEX );
            if( SingleShot().GetValue() )
			    if(( AStatus & 0x8000 ) == 0 )
				    return;

//            uint8_t ACountActive = 0;
//	        T_Channels_GetCountActiveChannels::Call( ACountActive );
//  	        if( ! ACountActive )
//   	            return;

//            Serial.println( "TEST1" );
            if( FClocked() && ( FReadClocked() || ( ! SingleShot().GetValue() )))
            {
                FReadClocked() = false;
                FClocked() = false;
			    int16_t	AValue = readRegister( ADS1X1XConst::REG_POINTER_CONVERT ); // >> 4;
//			    Serial.println( AValue );

    			float AFloatValue = float( AValue ) / 0x7FFF;

    			T_OutputPin::SetPinValue( AFloatValue );
    		}

            if( SingleShot().GetValue() )
			    StartRead();

		}

	public:
	    inline TexasInstrumentsADS1X13()
	    {
	        FClocked() = false;
	        FReadClocked() = false;
	    }

    };
//---------------------------------------------------------------------------
    template <
		typename T_I2C, T_I2C &C_I2C,
		typename T_Address,
		typename T_Comparator,
		typename T_Enabled,
		typename T_FClocked,
		typename T_FReadClocked,
		bool C_Is12bit,
		typename T_OutputPin,
		typename T_Range,
		typename T_SampleRate,
		typename T_SingleShot
    > class TexasInstrumentsADS1X14 :
		public T_Address,
		public T_Comparator,
		public T_Enabled,
		public T_FClocked,
		public T_FReadClocked,
		public T_OutputPin,
		public T_Range,
		public T_SampleRate,
		public T_SingleShot
    {
	public:
		_V_PROP_( Address )
		_V_PROP_( Comparator )
		_V_PROP_( Enabled )
		_V_PROP_( OutputPin )
		_V_PROP_( Range )
		_V_PROP_( SampleRate )
		_V_PROP_( SingleShot )

    protected:
		_V_PROP_( FClocked )
		_V_PROP_( FReadClocked )

	public:
		void UpdateConfig()
		{
			if( Comparator().UseAsReady() )
			{
				writeRegister( ADS1X1XConst::REG_POINTER_LOWTHRESH, 0x8000 );
				writeRegister( ADS1X1XConst::REG_POINTER_HITHRESH, 0x7FFF );
			}

			else
			{
				uint16_t AHigh = Comparator().ThresholdHigh() * 0x7FFF + 0.5;
				uint16_t ALow = Comparator().ThresholdLow() * 0x7FFF + 0.5;

				if( ALow > AHigh )
				{
					writeRegister( ADS1X1XConst::REG_POINTER_LOWTHRESH, AHigh );
					writeRegister( ADS1X1XConst::REG_POINTER_HITHRESH, ALow );
				}

				else
				{
					writeRegister( ADS1X1XConst::REG_POINTER_LOWTHRESH, ALow );
					writeRegister( ADS1X1XConst::REG_POINTER_HITHRESH, AHigh );
				}
			}

			StartRead( false );
		}

	protected:
		void writeRegister( uint8_t AReg, uint16_t AValue )
		{
			C_I2C.beginTransmission( uint8_t( Address() ));
			C_I2C.write( uint8_t( AReg ));
			C_I2C.write( uint8_t( AValue >> 8 ));
			C_I2C.write( uint8_t( AValue & 0xFF ));
			C_I2C.endTransmission();
		}

		int16_t readRegister( uint8_t AReg )
		{
			C_I2C.beginTransmission( uint8_t( Address() ));
			C_I2C.write( AReg );
			C_I2C.endTransmission();
			C_I2C.requestFrom( uint8_t( Address() ), uint8_t( 2 ));
			return ((int16_t( int8_t( C_I2C.read() )) << 8) | C_I2C.read());
		}

	protected:
		void StartRead( bool AStartRead )
		{
	        uint16_t AValue =   (( AStartRead ) ? ( uint16_t( 1 ) << 15 ) : 0 ) |
								( Comparator().Latching() ? ( uint16_t( 1 ) << 2 ) : 0 ) |
    							( Comparator().ActiveHigh() ? ( uint16_t( 1 ) << 3 ) : 0 ) |
								( Comparator().WindowMode() ? ( uint16_t( 1 ) << 4 ) : 0 ) |
//								( uint16_t( ASampleRate ) << 5 ) |
								(( SingleShot() ) ? uint16_t( 1 ) << 8 : 0 );
//								( uint16_t( AChannelIndex & 0b11 ) << 12 );
//								( Differential() ? 0 : ( uint16_t( 1 ) << 14 ) );

//            Serial.println( AChannelIndex );
////            T_Channels_GetConfig::Call( AChannelIndex, AValue );

//            Serial.println( AValue, BIN );
		    if( C_Is12bit )
		    {
//		          000 : 128 SPS
//                001 : 250 SPS
//                010 : 490 SPS
//                011 : 920 SPS
//                100 : 1600 SPS (default)
//                101 : 2400 SPS
//                1110 111 : 3300 SPS


                if( SampleRate().GetValue() > 920 ) // 1600, 2400, 3300
                {
                    if( SampleRate().GetValue() > 2400 )
        			        AValue |= ( 0b110 << 5 ); // 3300

        			else
        			{
        			    if( SampleRate().GetValue() > 1600 )
        			        AValue |= ( 0b101 << 5 ); // 2400

        			    else
        			        AValue |= ( 0b100 << 5 ); // 1600

                    }
                }

                else // 128, 250, 490, 920
                {

                    if( SampleRate().GetValue() > 250 ) // 490, 920
                    {
                        if( SampleRate().GetValue() > 490 )
                            AValue |= ( 0b011 << 5 ); // 920

                        else
                            AValue |= ( 0b010 << 5 ); // 490

                    }

                    else // 128, 250
                    {
                        if( SampleRate().GetValue() > 128 )
                            AValue |= ( 0b001 << 5 ); // 250

//                        else
//                            AValue |= ( 0b000 << 5 ); // 128

                    }

                }

		    }

		    else // 16 Bit
            {
    			if( SampleRate().GetValue() > 64 ) // 128, 250, 475, 860
    			{
    			    if( SampleRate().GetValue() > 250 ) // 475, 860
    			    {
    			        if( SampleRate().GetValue() > 475 )
        			        AValue |= ( 0b111 << 5 ); // 860

        			    else
        			        AValue |= ( 0b110 << 5 ); // 475

    			    }

    			    else // 128, 250
    			    {
    			        if( SampleRate().GetValue() > 128 )
                            AValue |= ( 0b101 << 5 ); // 250

                        else
                            AValue |= ( 0b100 << 5 ); // 128

    			    }
    			}

    			else
                {
    			    if( SampleRate().GetValue() > 16 ) // 32, 64
    			    {
    			        if( SampleRate().GetValue() > 32 )
                            AValue |= ( 0b011 << 5 ); // 64

                        else
                            AValue |= ( 0b010 << 5 ); // 32

    			    }

    			    else // 8, 16
    			    {
    			        if( SampleRate().GetValue() > 8 )
                            AValue |= ( 0b001 << 5 ); // 16

    //                    else
    //                        AValue |= ( 0b000 << 5 ); // 8

    			    }
                }
            }

//            Serial.println( Range().GetValue() );
			if( Range().GetValue() > 1.024f )
			{
			    if( Range().GetValue() <= 4.096f )
			    {
			        if( Range().GetValue() <= 2.048f )
			            AValue |= ( 0b010 << 9 ); // 2.048

			        else
			            AValue |= ( 0b001 << 9 ); // 4.096

			    }

//			    else
//                  AValue |= ( 0b000 << 9 ); // 6.144
			}

			else
            {
                if( Range().GetValue() <= 0.512f )
                {
                    if( Range().GetValue() <= 0.256f )
        	            AValue |= ( 0b101 << 9 ); // 0.256

                    else
        	            AValue |= ( 0b100 << 9 ); // 0.512

                }

                else
    	            AValue |= ( 0b011 << 9 ); // 1.024

            }


            FReadClocked() = true;

//            Serial.println( AValue, BIN );
		    writeRegister( ADS1X1XConst::REG_POINTER_CONFIG, AValue );
		}

	public:
	    void ClockInputPin_o_Receive( void *_Data )
	    {
	        FClocked() = true;
	    }

	public:
		inline void SystemStart()
		{
//			FIndex() = 0;
            UpdateConfig();
//delay( 3000 );
		    if( Enabled().GetValue() )
			    StartRead( true );

		}

		inline void SystemLoopBegin()
		{
		    if( ! Enabled().GetValue() )
		        return;

//            Serial.println( "TEST1" );
		    uint16_t AStatus = readRegister( ADS1X1XConst::REG_POINTER_CONFIG );
//			Serial.println( AStatus, HEX );
            if( SingleShot().GetValue() )
			    if(( AStatus & 0x8000 ) == 0 )
				    return;

//            uint8_t ACountActive = 0;
//	        T_Channels_GetCountActiveChannels::Call( ACountActive );
//  	        if( ! ACountActive )
//   	            return;

//            Serial.println( "TEST1" );
            if( FClocked() && ( FReadClocked() || ( ! SingleShot().GetValue() )))
            {
                FReadClocked() = false;
                FClocked() = false;
			    int16_t	AValue = readRegister( ADS1X1XConst::REG_POINTER_CONVERT ); // >> 4;
//			    Serial.println( AValue );

    			float AFloatValue = float( AValue ) / 0x7FFF;

    			T_OutputPin::SetPinValue( AFloatValue );
    		}

            if( SingleShot().GetValue() )
			    StartRead( true );

		}

	public:
	    inline TexasInstrumentsADS1X14()
	    {
	        FClocked() = false;
	        FReadClocked() = false;
	    }

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

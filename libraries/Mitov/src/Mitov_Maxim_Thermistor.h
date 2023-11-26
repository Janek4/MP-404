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
	    typename T_FManualTesting,
	    typename T_RefferenceHighOutputPin,
	    typename T_RefferenceLowOutputPin,
	    typename T_SensorLowOutputPin,
	    typename T_UnderOverVoltageOutputPin
    > class TArduinoThermistorMAX31865Errors :
	    public T_FManualTesting,
	    public T_RefferenceHighOutputPin,
	    public T_RefferenceLowOutputPin,
	    public T_SensorLowOutputPin,
	    public T_UnderOverVoltageOutputPin
    {
	public:
		_V_PIN_( RefferenceHighOutputPin )
		_V_PIN_( RefferenceLowOutputPin )
		_V_PIN_( SensorLowOutputPin )
		_V_PIN_( UnderOverVoltageOutputPin )

	public:
	    _V_PIN_( FManualTesting )

	public:
	    inline bool GetIsConnected()
	    {
	        return ( T_RefferenceHighOutputPin::GetPinIsConnected() || T_RefferenceLowOutputPin::GetPinIsConnected() || T_SensorLowOutputPin::GetPinIsConnected() || T_UnderOverVoltageOutputPin::GetPinIsConnected() );
	    }

    };
//---------------------------------------------------------------------------
    namespace TArduinoThermistorMAX31865ProbeType
    {
        enum TArduinoThermistorMAX31865ProbeType
        {
            PT100,
            PT1000,
            Custom
        };
    }
//---------------------------------------------------------------------------
	template <
	    typename T_ReferenceResistor,
	    typename T_ZeroDegreeResisstance
    > class TArduinoThermistorMAX31865SensorCustom :
	    public T_ReferenceResistor,
	    public T_ZeroDegreeResisstance
    {
	public:
		_V_PROP_( ReferenceResistor )
		_V_PROP_( ZeroDegreeResisstance )

    };
//---------------------------------------------------------------------------
	template <
	    typename T_CountWires,
	    typename T_Custom,
	    typename T_MeasurementTime,
        typename T_StartingTime,
        typename T_Type
    > class TArduinoThermistorMAX31865Sensor :
	    public T_CountWires,
	    public T_Custom,
	    public T_MeasurementTime,
        public T_StartingTime,
        public T_Type
    {
	public:
		_V_PROP_( CountWires )
		_V_PROP_( Custom )
		_V_PROP_( MeasurementTime )
        _V_PROP_( StartingTime )
		_V_PROP_( Type )

	public:
		float GetReferenceResistor()
		{
		    switch( Type().GetValue() )
		    {
		        case TArduinoThermistorMAX31865ProbeType::PT100 : return 430.0;
		        case TArduinoThermistorMAX31865ProbeType::PT1000 : return 4300.0;
		        default: return Custom().ReferenceResistor().GetValue();
		    }
		}

		float GetZeroDegreeResisstance()
		{
		    switch( Type().GetValue() )
		    {
		        case TArduinoThermistorMAX31865ProbeType::PT100 : return 100.0;
		        case TArduinoThermistorMAX31865ProbeType::PT1000 : return 1000.0;
		        default: return Custom().ZeroDegreeResisstance().GetValue();
		    }
		}

    };
//---------------------------------------------------------------------------
	template <
	    typename T_Value_Max,
	    typename T_MaxOutputPin,
	    typename T_Value_Min,
	    typename T_MinOutputPin
    > class TArduinoThermistorMAX31865Threshold :
	    public T_Value_Max,
	    public T_MaxOutputPin,
	    public T_Value_Min,
	    public T_MinOutputPin
    {
	public:
		_V_PIN_( MaxOutputPin )
		_V_PIN_( MinOutputPin )

	public:
		_V_PROP_( Value_Max )
		_V_PROP_( Value_Min )

    public:
        inline uint16_t GetMin16()
        {
            return ( Value_Min() * 0xFFFF ) + 0.5;
        }

        inline uint16_t GetMax16()
        {
            return ( Value_Max() * 0xFFFF ) + 0.5;
        }

	public:
	    inline bool GetIsConnected()
	    {
	        return ( T_MaxOutputPin::GetPinIsConnected() || T_MinOutputPin::GetPinIsConnected() );
	    }

    };
//---------------------------------------------------------------------------
	template <
		typename T_SPI, T_SPI &C_SPI,
		typename T_ChipSelectOutputPin,
		typename T_ClockInputPin_o_IsConnected,
		typename T_Enabled,
		typename T_Errors,
		typename T_FClocked,
		typename T_FEnabled,
		typename T_FMeasuring,
		typename T_FStarting,
		typename T_Filter50Hz,
		typename T_InFahrenheit,
		typename T_OutputPin,
		typename T_SPISpeed,
		typename T_Sensor,
		typename T_SingleShot,
        typename T_Threshold
	> class ThermistorMAX31865 :
		public T_ChipSelectOutputPin,
		public T_ClockInputPin_o_IsConnected,
		public T_Enabled,
		public T_Errors,
		public T_FClocked,
		public T_FEnabled,
		public T_FMeasuring,
		public T_FStarting,
		public T_Filter50Hz,
		public T_InFahrenheit,
		public T_OutputPin,
		public T_SPISpeed,
		public T_Sensor,
		public T_SingleShot,
        public T_Threshold
	{
	public:
		_V_PIN_( OutputPin )
		_V_PIN_( ChipSelectOutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( Errors )
		_V_PROP_( Filter50Hz )
		_V_PROP_( InFahrenheit )
		_V_PROP_( SPISpeed )
		_V_PROP_( Sensor )
		_V_PROP_( SingleShot )
		_V_PROP_( Threshold )

	public: // Live Binding Setters
	    inline void UpdateEnabled()
	    {
	        UpdateConfig( false, 0 );

            if( Enabled().GetValue() && ( ! FEnabled().GetValue() ))
            {
                FStarting() = true;
                FStartTime = millis();
            }

            FEnabled() = Enabled().GetValue();
	    }

	    inline void UpdateSettings()
	    {
            UpdateConfig( false, 0 );
	    }

	    inline void UpdateThreshold_Min()
	    {
	        writeRegister16( LFAULTMSB_REG, Threshold().GetMin16() );
	    }

	    inline void UpdateThreshold_Max()
	    {
	        writeRegister16( HFAULTMSB_REG, Threshold().GetMax16() );
	    }

	protected:
        _V_PROP_( ClockInputPin_o_IsConnected )
        _V_PROP_( FClocked )
		_V_PROP_( FEnabled )
		_V_PROP_( FMeasuring )
		_V_PROP_( FStarting )

	protected:
		unsigned long	FStartTime;

	protected:
        const uint8_t CONFIG_REG = 0x00;
        const uint8_t RTDMSB_REG = 0x01;
        const uint8_t RTDLSB_REG = 0x02;
        const uint8_t HFAULTMSB_REG = 0x03;
        const uint8_t HFAULTLSB_REG = 0x04;
        const uint8_t LFAULTMSB_REG = 0x05;
        const uint8_t LFAULTLSB_REG = 0x06;
        const uint8_t FAULTSTAT_REG = 0x07;

        const float RTD_A = 3.9083e-3;
        const float RTD_B = -5.775e-7;


	protected:
	    void UpdateConfig( bool AStartSingleShot, uint8_t ATestFlags )
	    {
	        uint8_t AValue =    ( ( SingleShot().GetValue() && ClockInputPin_o_IsConnected().GetValue()  ) ? 0 : 0b01000000 ) |
	                            (( AStartSingleShot ) ? 0b00100000 : 0 ) |
	                            (( Sensor().CountWires() == 3 ) ? 0b00010000 : 0 ) |
	                            0b00000010 | // Clear Fault Startus
	                            ATestFlags |
	                            (( Filter50Hz() ) ? 0b00000001 : 0 );

	        if( ClockInputPin_o_IsConnected().GetValue() )
	            AValue |= 0b01000000;

            if( Enabled().GetValue() )
            {
                if( ! ClockInputPin_o_IsConnected().GetValue() )
                    AValue |= 0b10000000;

                else if(( SingleShot().GetValue() && AStartSingleShot ) || ( ! SingleShot().GetValue() ) )
                    AValue |= 0b10000000;

            }

//            delay( 1000 );
//            Serial.println();
//            Serial.println( AValue, BIN );

            writeRegister8( CONFIG_REG, AValue );
	    }

	protected:
        void writeRegister8( uint8_t addr, uint8_t data )
        {
            addr |= 0x80; // make sure top bit is set

			T_ChipSelectOutputPin::SetPinValueLow();

			C_SPI.beginTransaction(SPISettings( SPISpeed(), MSBFIRST, SPI_MODE1 ));
    	    C_SPI.transfer( addr );
    	    C_SPI.transfer( data );
			C_SPI.endTransaction();

			T_ChipSelectOutputPin::SetPinValueHigh();
        }

        void writeRegister16( uint8_t addr, uint16_t data )
        {
            addr |= 0x80; // make sure top bit is set

			T_ChipSelectOutputPin::SetPinValueLow();

			C_SPI.beginTransaction(SPISettings( SPISpeed(), MSBFIRST, SPI_MODE1 ));
    	    C_SPI.transfer( addr );
    	    C_SPI.transfer( uint8_t( data >> 8 ));
    	    C_SPI.transfer( uint8_t( data ));
			C_SPI.endTransaction();

			T_ChipSelectOutputPin::SetPinValueHigh();
        }

        uint8_t readRegister8(uint8_t addr)
        {
			T_ChipSelectOutputPin::SetPinValueLow();
			C_SPI.beginTransaction(SPISettings( SPISpeed(), MSBFIRST, SPI_MODE1 ));
    	    C_SPI.transfer( addr );

    	    uint8_t ret = C_SPI.transfer( 0 );

			C_SPI.endTransaction();

			T_ChipSelectOutputPin::SetPinValueHigh();

            return ret;
        }

        uint16_t readRegister16( uint8_t addr )
        {
			T_ChipSelectOutputPin::SetPinValueLow();
			C_SPI.beginTransaction(SPISettings( SPISpeed(), MSBFIRST, SPI_MODE1 ));
    	    C_SPI.transfer( addr );

    	    uint16_t ret = uint16_t( C_SPI.transfer( 0 ) ) << 8;
            ret |= C_SPI.transfer( 0 );

			C_SPI.endTransaction();

			T_ChipSelectOutputPin::SetPinValueHigh();

            return ret;
        }


	protected:
		void ReadSensor()
		{
			if( ! Enabled() )
				return;

			if( FStarting() )
			{
			    if( millis() - FStartTime < Sensor().StartingTime() )
                    return;

                FStarting() = false;

    			if( FClocked() )
    			    if( SingleShot().GetValue() && ClockInputPin_o_IsConnected().GetValue() )
    			    {
    			        UpdateConfig( true, 0 );
    			        FStartTime = millis();
    			        FMeasuring() = true;
    			        return;
    			    }

			}

			if( FMeasuring() )
			{
//			    Serial.println( "FMeasuring" );
			    if( millis() - FStartTime < Sensor().MeasurementTime() )
                    return;

                FMeasuring() = false;
			}

			else if( SingleShot().GetValue() && ClockInputPin_o_IsConnected().GetValue() )
                return;

            if( T_OutputPin::GetPinIsConnected() )
            {
                uint16_t AValue = readRegister16( RTDMSB_REG );
                if( ( AValue & 1 ) == 0 )
                {
                    AValue >>= 1;
//                    Serial.println( AValue );

                    float Rt = AValue;
                    Rt /= 32768;
                    Rt *= Sensor().GetReferenceResistor();

                    float Z1 = - RTD_A;
                    float Z2 = RTD_A * RTD_A - ( 4 * RTD_B );
                    float Z3 = ( 4 * RTD_B ) / Sensor().GetZeroDegreeResisstance();
                    float Z4 = 2 * RTD_B;

                    float ATemperature = Z2 + (Z3 * Rt);
                    ATemperature = (sqrt(ATemperature) + Z1) / Z4;

                    if( ATemperature < 0 )
                    {
                        Rt /= Sensor().GetZeroDegreeResisstance();
                        Rt *= 100; // normalize to 100 ohm

                        float rpoly = Rt;

                        ATemperature = -242.02;
                        ATemperature += 2.2228 * rpoly;
                        rpoly *= Rt; // square
                        ATemperature += 2.5859e-3 * rpoly;
                        rpoly *= Rt; // ^3
                        ATemperature -= 4.8260e-6 * rpoly;
                        rpoly *= Rt; // ^4
                        ATemperature -= 2.8183e-8 * rpoly;
                        rpoly *= Rt; // ^5
                        ATemperature += 1.5243e-10 * rpoly;
                    }

        			T_OutputPin::SetPinValue( ATemperature );
        		}
    	    }

/*
			float ATemperature;
			if ( AValue & 0b100 )
			{
				ATemperature = 0;
				T_NotConnectedOutputPin::SetPinValue( true );
			}

			else
			{
				T_NotConnectedOutputPin::SetPinValue( false );
				AValue >>= 3;
				if( InFahrenheit() )
					ATemperature = AValue * 0.25 * 9.0 / 5.0 + 32;

				else
					ATemperature = AValue * 0.25;

			}

			T_OutputPin::SetPinValue( ATemperature );
*/
			if( FClocked() )
			    if( SingleShot().GetValue() && ClockInputPin_o_IsConnected().GetValue() )
			    {
			        UpdateConfig( true, 0 );
			        FStartTime = millis();
			        FMeasuring() = true;
			    }

		}

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
		    if( SingleShot().GetValue() )
		    {
    		    if( ! FMeasuring() )
  			    {
			        UpdateConfig( true, 0 );
			        FStartTime = millis();
			        FMeasuring() = true;
			        return;
                }

    		    FClocked() = true;
            }

            else
                ReadSensor();
		}

		inline void AutomaticTest_InputPin_o_Receive( void *_Data )
		{
		    UpdateConfig( false, 0b10000100 );
		}

		inline void ManualTest_InputPin_o_Receive( void *_Data )
		{
		    bool AValue = *(bool *) _Data;

		    if( FManualTesting() == AValue )
		        return;

		    FManualTesting() = AValue;

		    if( AValue )
		        UpdateConfig( false, 0b10001000 );

		    else
		        UpdateConfig( false, 0b10001100 );

		}

	public:
		inline void SystemLoopBegin()
		{
		    if( ( SingleShot().GetValue() && ClockInputPin_o_IsConnected().GetValue() ) || ( ! ClockInputPin_o_IsConnected().GetValue() ) )
			    ReadSensor();

			if( Errors().GetIsConnected() || Threshold().GetIsConnected() )
			{
                uint8_t AStatus = readRegister8( FAULTSTAT_REG );

                Threshold().MaxOutputPin().SetPinValue( ( AStatus & 0b10000000 ) != 0 );
                Threshold().MinOutputPin().SetPinValue( ( AStatus & 0b01000000 ) != 0 );

                Errors().RefferenceLowOutputPin().SetPinValue( ( AStatus & 0b00010000 ) != 0 );
                Errors().RefferenceHighOutputPin().SetPinValue( ( AStatus & 0b00100000 ) != 0 );


                Errors().SensorLowOutputPin().SetPinValue( ( AStatus & 0b00001000 ) != 0 );

                Errors().UnderOverVoltageOutputPin().SetPinValue( ( AStatus & 0b00000100 ) != 0 );

                if( AStatus != 0 )
                    UpdateConfig( false, 0b00000010 ); // Clear Faults

			}
		}

		inline void SystemStart()
		{
			T_ChipSelectOutputPin::SetPinValueHigh();

			T_ChipSelectOutputPin::SetPinValueLow();

			C_SPI.beginTransaction(SPISettings( SPISpeed(), MSBFIRST, SPI_MODE1 ));
    	    C_SPI.transfer( HFAULTMSB_REG | 0x80 );

            uint16_t AValue = Threshold().GetMax16();
    	    C_SPI.transfer( uint8_t( AValue >> 8 ));
    	    C_SPI.transfer( uint8_t( AValue ));

            AValue = Threshold().GetMin16();
    	    C_SPI.transfer( uint8_t( AValue >> 8 ));
    	    C_SPI.transfer( uint8_t( AValue ));

			C_SPI.endTransaction();

			T_ChipSelectOutputPin::SetPinValueHigh();

		    UpdateConfig( false, 0 );
		    FStarting() = true;
		}

	public:
		ThermistorMAX31865()
		{
			FClocked() = false;
    		FEnabled() = false;
		    FStarting() = false;
		    Errors().FManualTesting() = false;
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
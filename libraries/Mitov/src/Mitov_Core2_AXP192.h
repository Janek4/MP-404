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
#include <Wire.h>
#include <Mitov_XPowers_AXP192.h>

namespace Mitov
{
//---------------------------------------------------------------------------
/*
	template <
	    typename T_Display,
	    typename T_Processor
    > class TArduinoESP32_M5Stack_Core2_PowerModule_Voltages :
	    public T_Display,
	    public T_Processor
    {
	public:
		_V_PROP_( Display )
		_V_PROP_( Processor )

    };
*/
//---------------------------------------------------------------------------
    namespace Core2_AXP192_Func
    {
        uint8_t Read8bit( uint8_t AReguster )
        {
            Wire1.beginTransmission( 0x34 );
            Wire1.write( AReguster );
            Wire1.endTransmission();
            Wire1.requestFrom( 0x34, 1 );
            return Wire1.read();
        }

        void Write1Byte( uint8_t AReguster, uint8_t AValue )
        {
			Wire1.beginTransmission( 0x34 );
			Wire1.write( AReguster );
			Wire1.write( AValue );
			Wire1.endTransmission();
        }

    }
//---------------------------------------------------------------------------
	template <
	    typename T_FalseValue,
	    typename T_InitialValue,
	    typename T_TrueValue
    > class Arduino_Core2_LED :
	    public T_FalseValue,
	    public T_InitialValue,
	    public T_TrueValue
    {
	public:
		_V_PROP_( FalseValue )
		_V_PROP_( InitialValue )
		_V_PROP_( TrueValue )

    protected:
        void SetValue( float AValue )
        {
            if( AValue == 0 )
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) | 0b00000010 );

            else if( AValue == 1 )
            {
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_92_GPIO1_FunctionControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_92_GPIO1_FunctionControl ) | 0x0F ) & 0b11111000 ); // Disanle PWM

                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) & 0b11111101 );
            }


            else
            {
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_92_GPIO1_FunctionControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_92_GPIO1_FunctionControl ) | 0x0F ) & 0b11111010 ); // Enable PWM

                uint8_t AByteValue = ( ( 1.0 - AValue ) * 255 ) + 0.5;
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_9A_PWM1_DutyCycle_2, AByteValue );
//                Write1Byte(0x91, ( Read8bit(0x91) & 0x0F) | ( AValue & 0xF0 ) );

                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) & 0b11111101 );
            }

        }

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
		    if( *(bool *)_Data )
            {
                if( TrueValue() != 1.0 )
                    SetValue( TrueValue().GetValue() );

                else
                {
                    Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_92_GPIO1_FunctionControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_92_GPIO1_FunctionControl ) | 0x0F ) & 0b11111000 ); // Disanle PWM

                    Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) & 0b11111101 );
                }
            }

            else
            {
                if( FalseValue() != 0.0 )
                    SetValue( FalseValue().GetValue() );

                else
                    Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) | 0b00000010 );

            }
		}

		inline void AnalogInputPin_o_Receive( void *_Data )
		{
		    SetValue( *(float *)_Data );
		}

	public:
	    inline void SystemInit()
	    {
            Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_98_PWM1_OutputFrequency, 19 );
            Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_99_PWM1_DutyCycle_1, 0xFF );

	        SetValue( InitialValue().GetValue() );
	    }

    };
//---------------------------------------------------------------------------
	template <
	    typename T_FalseValue,
	    typename T_InitialValue,
	    typename T_TrueValue
    > class Arduino_Core2_Vibrator :
	    public T_FalseValue,
	    public T_InitialValue,
	    public T_TrueValue
    {
	public:
		_V_PROP_( FalseValue )
		_V_PROP_( InitialValue )
		_V_PROP_( TrueValue )

    protected:
/*
        void SetLDOVoltage( uint8_t number, float voltage )
        {
//            uint16_t AmlVoltage = (voltage > 3.3) ? 15 : ( ( voltage * 1000 ) / 100) - 18;
            uint16_t AmlVoltage = ( ( voltage * 1000 ) / 100 ) - 18;
            switch (number)
            {
            //uint8_t reg, data;
                case 2:
                    Write1Byte(0x28, (Read8bit(0x28) & 0X0F) | ( AmlVoltage << 4));
                    break;

                case 3:
                    Write1Byte(0x28, (Read8bit(0x28) & 0XF0) | AmlVoltage );
                    break;
            }
        }

//        M5.Axp.SetLDOEnable(3, true);

        void SetLDOEnable( uint8_t number, bool state )
        {
            uint8_t mark = 0x01;
            if ((number < 2) || (number > 3))
                return;

            mark <<= number;
            if( state )
                Write1Byte(0x12, (Read8bit(0x12) | mark));

            else
                Write1Byte(0x12, (Read8bit(0x12) & (~mark)));

        }
*/
        void SetValue( float AValue )
        {
            if( AValue == 0 )
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_12_PowerOutputControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_12_PowerOutputControl ) & 0b11110111 ));

            else
            {
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_12_PowerOutputControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_12_PowerOutputControl ) | 0b00001000 ));
                uint8_t AByteValue = ( AValue * 0x10 ) - 1 + 0.5;
                if( AByteValue == 0 )
                    Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_12_PowerOutputControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_12_PowerOutputControl ) & 0b11110111 ));

                else
                    Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_28_LDO2_3_OutputVoltage, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_28_LDO2_3_OutputVoltage ) & 0xF0 ) | AByteValue - 1 );

            }
/*
            if( AValue == 0 )
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) | 0b00000010 );

            else if( AValue == 1 )
            {
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_92_GPIO1_FunctionControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_92_GPIO1_FunctionControl ) | 0x0F ) & 0b11111000 ); // Disanle PWM

                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) & 0b11111101 );
            }


            else
            {
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_92_GPIO1_FunctionControl, ( Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_92_GPIO1_FunctionControl ) | 0x0F ) & 0b11111010 ); // Enable PWM

                uint8_t AByteValue = ( ( 1.0 - AValue ) * 255 ) + 0.5;
                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_9A_PWM1_DutyCycle_2, AByteValue );
//                Write1Byte(0x91, ( Read8bit(0x91) & 0x0F) | ( AValue & 0xF0 ) );

                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) & 0b11111101 );
            }
*/
        }

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
		    if( *(bool *)_Data )
                SetValue( TrueValue().GetValue() );

            else
                SetValue( FalseValue().GetValue() );

		}

		inline void AnalogInputPin_o_Receive( void *_Data )
		{
		    SetValue( *(float *)_Data );
		}

	public:
	    inline void SystemInit()
	    {
//	        Serial.begin( 9600 );
/*
            Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_98_PWM1_OutputFrequency, 19 );
            Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_99_PWM1_DutyCycle_1, 0xFF );
*/
//            SetLDOVoltage( 3, 2.0 );  //Vibrator power voltage preset
	        SetValue( InitialValue().GetValue() );
	    }

    };
//---------------------------------------------------------------------------
	template <
		typename T_Battery,
//		typename T_LED,
        typename T_ProcessorVoltage,
		typename T_Thermometer
	> class Arduino_Core2_AXP192 :
		public T_Battery,
//		public T_LED,
        public T_ProcessorVoltage,
		public T_Thermometer
	{
    protected:
        enum mbus_mode_t
        {
            kMBusModeOutput = 0,  // powered by USB or Battery
            kMBusModeInput = 1    // powered by outside input
        };

        enum CHGCurrent
        {
            kCHG_100mA = 0,
            kCHG_190mA,
            kCHG_280mA,
            kCHG_360mA,
            kCHG_450mA,
            kCHG_550mA,
            kCHG_630mA,
            kCHG_700mA,
            kCHG_780mA,
            kCHG_880mA,
            kCHG_960mA,
            kCHG_1000mA,
            kCHG_1080mA,
            kCHG_1160mA,
            kCHG_1240mA,
            kCHG_1320mA,
        };

	public:
		_V_PROP_( Battery )
//		_V_PROP_( LED )
		_V_PROP_( ProcessorVoltage )
		_V_PROP_( Thermometer )

    protected:
        inline static uint8_t Read8bit( uint8_t AReguster )
        {
            return Core2_AXP192_Func::Read8bit( AReguster );
        }

        inline static void Write1Byte( uint8_t AReguster, uint8_t AValue )
        {
            Core2_AXP192_Func::Write1Byte( AReguster, AValue );
        }

    protected:
        void SetLDOVoltage( uint8_t number, float voltage )
        {
//            uint16_t AmlVoltage = (voltage > 3.3) ? 15 : ( ( voltage * 1000 ) / 100) - 18;
            uint16_t AmlVoltage = ( ( voltage * 1000 ) / 100 ) - 18;
            switch (number)
            {
            //uint8_t reg, data;
                case 2:
                    Write1Byte(0x28, (Read8bit(0x28) & 0X0F) | ( AmlVoltage << 4));
                    break;

                case 3:
                    Write1Byte(0x28, (Read8bit(0x28) & 0XF0) | AmlVoltage );
                    break;
            }
        }

        void SetLDOEnable( uint8_t number, bool state )
        {
            uint8_t mark = 0x01;
            if ((number < 2) || (number > 3))
                return;

            mark <<= number;
            if( state )
                Write1Byte(0x12, (Read8bit(0x12) | mark));

            else
                Write1Byte(0x12, (Read8bit(0x12) & (~mark)));

        }

/*
        void SetDCDC3( bool State )
        {
            uint8_t buf = Read8bit( 0x12 );

            if (State == true)
                buf = (1 << 1) | buf;

            else
                buf = ~(1 << 1) & buf;

            Write1Byte( 0x12, buf );
        }
*/
        /// @param number 0=DCDC1 / 1=DCDC2 / 2=DCDC3
        void SetDCVoltage( uint8_t number, float voltage )
        {
            uint8_t addr;
            if( number > 2 )
                return;

            voltage *= 1000;
            uint16_t AmlVoltage = (voltage < 700) ? 0 : (voltage - 700) / 25;
            switch (number)
            {
                case 0:
                    addr = 0x26;
                    break;

                case 1:
                    addr = 0x23;
                    break;

                case 2:
                    addr = 0x27;
                    break;
            }

            // Serial.printf("result:%hhu\n", (Read8bit(addr) & 0X80) | (AmlVoltage & 0X7F));
            // Serial.printf("result:%d\n", (Read8bit(addr) & 0X80) | (AmlVoltage & 0X7F));
            // Serial.printf("result:%x\n", (Read8bit(addr) & 0X80) | (AmlVoltage & 0X7F));
            Write1Byte( addr, ( Read8bit( addr ) & 0X80 ) | ( AmlVoltage & 0X7F ) );
        }


/*
        void SetLed(uint8_t state)
        {
            uint8_t data = Read8bit( 0x94 );

            if (state)
                data = data & 0XFD;

            else
                data |= 0X02;

            Write1Byte( 0x94, data);
        }
*/
        void SetCHGCurrent(uint8_t state)
        {
            uint8_t data = Read8bit(0x33);
            data &= 0xf0;
            data |= ( state & 0x0f );
            Write1Byte(0x33, data);
        }

/*
        void SetLCDRSet(bool state) // LCD Reset
        {
            uint8_t reg_addr = 0x96;
            uint8_t gpio_bit = 0x02;
            uint8_t data = Read8bit(reg_addr);

            if (state)
                data |= gpio_bit;

            else
                data &= ~gpio_bit;

            Write1Byte( reg_addr, data );
        }
*/

        // Select source for BUS_5V
        // 0 : use internal boost
        // 1 : powered externally
        void SetBusPowerMode(uint8_t state)
        {
            uint8_t data;
            if (state == 0)
            {
                // Set GPIO to 3.3V (LDO OUTPUT mode)
                data = Read8bit(0x91);
                Write1Byte(0x91, (data & 0x0F) | 0xF0);
                // Set GPIO0 to LDO OUTPUT, pullup N_VBUSEN to disable VBUS supply from BUS_5V
                data = Read8bit(0x90);
                Write1Byte(0x90, (data & 0xF8) | 0x02);
                // Set EXTEN to enable 5v boost
                data = Read8bit(0x10);
                Write1Byte(0x10, data | 0x04);
            }

            else
            {
                // Set EXTEN to disable 5v boost
                data = Read8bit(0x10);
                Write1Byte(0x10, data & ~0x04);
                // Set GPIO0 to float, using enternal pulldown resistor to enable VBUS supply from BUS_5V
                data = Read8bit(0x90);
                Write1Byte(0x90, (data & 0xF8) | 0x07);
            }
        }

	public:
		inline void SystemInit()
		{
/*
//			Write1Byte( 0x10, 0xff );  //OLED_VPP Enable
//			Write1Byte( 0x28, 0xcc ); //Enable LDO2&LDO3, LED&TFT 3.0V

            Write1Byte( 0x82, 0xff ); //Enable all the ADCs
            Write1Byte( 0x33, 0xC0 ); //Enable Charging, 100mA, 4.2V, End at current less than 10%
            Write1Byte( 0xB8, 0x80 ); //Enable Colume Counter

//            Write1Byte( 0x12, 0x4d ); //Enable DC-DC1, OLED_VDD, 5B V_EXT

            Write1Byte( 0x36, 0b00001100 ); // pek setting //shut down setting with 4s,auto shut donw while  btn press longer then shut donw setting
            Write1Byte( 0x90, 0x02 ); //gpio0 setting


			Write1Byte( 0x90, 0x02 ); //gpio0 setting

			Write1Byte( 0x30, 0xe0 ); //VBUS limit

			Write1Byte( 0x39, 0xFC );

            Write1Byte( 0x35, 0xA2 );
*/

            //AXP192 30H
            Write1Byte( 0x30, (Read8bit( 0x30 ) & 0x04 ) | 0X02 );
            //  Serial.printf("axp: vbus limit off\n");

            //AXP192 GPIO1:OD OUTPUT
//            Write1Byte( REG_92_GPIO1_FunctionControl, Read8bit( REG_92_GPIO1_FunctionControl ) & 0xf8 );
//            Write1Byte( REG_92_GPIO1_FunctionControl, ( Read8bit( REG_92_GPIO1_FunctionControl ) | 0x0F ) & 0b11111010 );
            //  Serial.printf("axp: gpio1 init\n");

            //AXP192 GPIO2:OD OUTPUT
            Write1Byte( 0x93, Read8bit( 0x93 ) & 0xf8 );
//            Serial.printf("axp: gpio2 init\n");

            //AXP192 RTC CHG
            Write1Byte( 0x35, ( Read8bit( 0x35 ) & 0x1c ) | 0xa2 );
//            Serial.printf("axp: rtc battery charging enabled\n");

//            SetESPVoltage( 3.35 );
            SetDCVoltage( 0, ProcessorVoltage().GetValue() );
//            Serial.printf("axp: esp32 power voltage was set to 3.35v\n");

//            SetLcdVoltage( 2.8 );
//// Display BKL           SetDCVoltage( 2, Voltages().Display().GetValue() );
//            Serial.printf("axp: lcd backlight voltage was set to 2.80v\n");

            SetLDOVoltage( 2, 3.3 );  //Periph power voltage preset (LCD_logic, SD card)
//            Serial.printf("axp: lcd logic and sdcard voltage preset to 3.3v\n");

//            SetLDOVoltage( 3, 2.0 );  //Vibrator power voltage preset
//            Serial.printf("axp: vibrator voltage preset to 2v\n");

            SetLDOEnable( 2, true );
//// Display BKL           SetDCDC3( true );  // LCD backlight

            // PWM1 Frequency
//            Write1Byte( 0x98, 19 );
//            Write1Byte( 0x99, 0xFF );

//            UpdateLED();
//                Core2_AXP192_Func::Write1Byte( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status, Core2_AXP192_Func::Read8bit( XPowers_AXP192::REG_94_GPIO_2_0_SignalControl_Status ) | 0b00000010 );

//            SetLed( LED().GetValue() );

            SetCHGCurrent( kCHG_100mA );
            //SetAxpPriphPower(1);
            //Serial.printf("axp: lcd_logic and sdcard power enabled\n\n");

            //pinMode(39, INPUT_PULLUP);

            //AXP192 GPIO4
            Write1Byte( 0X95, ( Read8bit( 0x95 ) & 0x72 ) | 0X84 );

            Write1Byte( 0X36, 0X4C );

            Write1Byte( 0x82, 0xff );

////            SetLCDRSet( 0 );
////            delay( 100 );
////            SetLCDRSet( 1 );
////            delay( 100 );
            // I2C_WriteByteDataAt(0X15,0XFE,0XFF);

            // axp: check v-bus status
            if( Read8bit( 0x00 ) & 0x08 )
            {
                Write1Byte( 0x30, Read8bit( 0x30 ) | 0x80 );
                // if v-bus can use, disable M-Bus 5V output to input
                SetBusPowerMode( kMBusModeInput );
            }

            else
            {
                // if not, enable M-Bus 5V output
                SetBusPowerMode( kMBusModeOutput );
            }

		}

		inline void SystemStartThermometer()
		{
			if( ! Thermometer().ClockInputPin_o_IsConnected().GetValue() )
				Thermometer().ReadThermometer( false );

		}

		inline void SystemLoopBeginThermometer()
		{
			if( ! Thermometer().ClockInputPin_o_IsConnected().GetValue() )
				Thermometer().ReadThermometer( true );

		}

		inline void SystemStartBatteryVoltage()
		{
			if( ! Battery().ClockInputPin_o_IsConnected().GetValue() )
				Battery().ReadBatteryVoltage( false );

		}

		inline void SystemLoopBeginBatteryVoltage()
		{
			if( ! Battery().ClockInputPin_o_IsConnected().GetValue() )
				Battery().ReadBatteryVoltage( true );

		}

		inline void SystemStartBatteryPower()
		{
			if( ! Battery().ClockInputPin_o_IsConnected().GetValue() )
				Battery().ReadBatteryPower( false );

		}

		inline void SystemLoopBeginBatteryPower()
		{
			if( ! Battery().ClockInputPin_o_IsConnected().GetValue() )
				Battery().ReadBatteryPower( true );

		}

/*
		inline void SystemStartCoulombsStored()
		{
			if( ! Coulombs().ClockInputPin_o_IsConnected().GetValue() )
				Coulombs().ReadCoulombsStored( false );

		}

		inline void SystemLoopBeginCoulombsStored()
		{
			if( ! Coulombs().ClockInputPin_o_IsConnected().GetValue() )
				Coulombs().ReadCoulombsStored( true );

		}
*/
	};
//---------------------------------------------------------------------------
}


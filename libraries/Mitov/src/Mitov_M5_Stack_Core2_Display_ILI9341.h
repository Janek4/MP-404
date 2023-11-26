////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Mitov_M5_Stack_Display_ILI9341.h>
#include <Mitov_Core2_AXP192.h>
#include <Mitov_FocalTech_FT6336U.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
    class M5Stack_Core2_Display_ResetPinImplementation
    {
    public:
        void SetPinValueHigh()
        {
            Core2_AXP192_Func::Write1Byte( 0x96, Core2_AXP192_Func::Read8bit( 0x96 ) | 0b00000010 );
        }

        void SetPinValueLow()
        {
            Core2_AXP192_Func::Write1Byte( 0x96, Core2_AXP192_Func::Read8bit( 0x96 ) & 0b11111101 );
        }

    };
//---------------------------------------------------------------------------
	template <
	    typename T_Enabled,
	    typename T_InactivityPeriod,
	    typename T_ReportPeriod
    > class TArduinoM5StackCore2DisplayModuleTouchMonitorMode :
	    public T_Enabled,
	    public T_InactivityPeriod,
	    public T_ReportPeriod
    {
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InactivityPeriod )
		_V_PROP_( ReportPeriod )

    };
//---------------------------------------------------------------------------
	template <
	    typename T_OWNER, T_OWNER &C_OWNER,
	    typename T_ButtonsOutputPins_A,
	    typename T_ButtonsOutputPins_B,
	    typename T_ButtonsOutputPins_C,
        typename T_Enabled,
        typename T_FID_Button,
        typename T_FFlags,
        typename T_FLastCount,
        typename T_MonitorMode,
        typename T_OutputPin,
        typename T_ReportPeriod
    > class TArduinoM5StackCore2DisplayModuleTouch :
	    public T_ButtonsOutputPins_A,
	    public T_ButtonsOutputPins_B,
	    public T_ButtonsOutputPins_C,
        public T_Enabled,
        public T_FID_Button,
        public T_FFlags,
        public T_FLastCount,
        public T_MonitorMode,
        public T_OutputPin,
        public T_ReportPeriod
    {
	public:
	    _V_PIN_( ButtonsOutputPins_A )
	    _V_PIN_( ButtonsOutputPins_B )
	    _V_PIN_( ButtonsOutputPins_C )
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( MonitorMode )
		_V_PROP_( Enabled )
        _V_PROP_( ReportPeriod )

	protected:
        _V_PROP_( FID_Button )
	    _V_PROP_( FFlags )
	    _V_PROP_( FLastCount )

	public:
        void Update_ReportPeriod_Active()
        {
            Write( FocalTech_FT6336U::REG_PERIODACTIVE, ReportPeriod().GetValue() );
        }

        void Update_ReportPeriod_Monitor()
        {
            Write( FocalTech_FT6336U::REG_PERIODMONITOR, MonitorMode().ReportPeriod().GetValue() );
        }

        void Update_EnableMonitor()
        {
            Write( FocalTech_FT6336U::REG_CTRL, ( MonitorMode().Enabled().GetValue() ) ? 1 : 0 );
        }

        void Update_InactivityPeriod()
        {
            Write( FocalTech_FT6336U::REG_TIMEENTERMONITOR, MonitorMode().InactivityPeriod() );
        }

    protected:
        static const uint8_t TOUCH_ADDR = 0x38;
        static const uint8_t TOUCH_INTERRUPT_PIN = 39;

    protected:
        TArduino2DTouchPoint FLastPoints[ 2 ];

    protected:
        uint8_t Read( uint8_t reg )
        {
            Wire1.beginTransmission( TOUCH_ADDR );
            Wire1.write(reg);
            Wire1.endTransmission();
            Wire1.requestFrom( TOUCH_ADDR, uint8_t(1) );
            return Wire1.read();
        }

        void Write( uint8_t reg, uint8_t value )
        {
            Wire1.beginTransmission( TOUCH_ADDR );
            Wire1.write(reg);
            Wire1.write( value );
            Wire1.endTransmission();
        }

        // Reading size bytes into data
        void BeginRead(uint8_t reg, uint8_t size )
        {
            Wire1.beginTransmission( TOUCH_ADDR );
            Wire1.write(reg);
            Wire1.endTransmission();
            Wire1.requestFrom(TOUCH_ADDR, size);
        }

        uint16_t Read16()
        {
            uint16_t AResult = uint16_t( Wire1.read() ) << 8;
            AResult |= Wire1.read();

            return AResult;
        }

        void ReportResult( TArduino2DTouchPoint *APoints, uint8_t ACount )
        {
            TArduino2DTouchPoint ALocalPoints[ 2 ];
            memcpy( ALocalPoints, APoints, sizeof( ALocalPoints ) );
            APoints = ALocalPoints;
//            Serial.println( ACount );
            for( int AIndex = 0; AIndex < ACount; ++ AIndex )
            {
//                if( APoints[ AIndex ].ID )
//                Serial.println( FID_Button(), BIN );
//                Serial.println( APoints[ AIndex ].ID );
                uint8_t AButtonIndex = ( FID_Button() >> ( APoints[ AIndex ].ID * 2 )) & 0b11;
                if( AButtonIndex )
                    APoints[ AIndex ].Y = 245;

                switch( AButtonIndex )
                {
                    case 1 : APoints[ AIndex ].X = 12; break;
                    case 2 : APoints[ AIndex ].X = 135; break;
                    case 3 : APoints[ AIndex ].X = 235; break;
                }

                if( APoints[ AIndex ].Y >= 240 )
                {
                    if( FFlags() & ( 1 << APoints[ AIndex ].ID ) )
                    {

                        APoints[ AIndex ].Y = 239;
                        continue;
                    }

                    if( ( APoints[ AIndex ].X >= 10 ) && ( APoints[ AIndex ].X <= 10 + 110 ) )
                    {
                        if( APoints[ AIndex ].Status != TTouchStatus::Up )
                        {
                            T_ButtonsOutputPins_A::SetPinValue( true );
                            if( APoints[ AIndex ].ID )
                                T_FID_Button() = FID_Button() | 0b0100;

                            else
                                T_FID_Button() = FID_Button() | 1;

                        }

                        else
                        {
                            T_ButtonsOutputPins_A::SetPinValue( false );
                            if( APoints[ AIndex ].ID )
                                T_FID_Button() = FID_Button() & 0b0011;

                            else
                                T_FID_Button() = FID_Button() & 0b1100;
                        }

                    }

                    else if( ( APoints[ AIndex ].X >= 130 ) && ( APoints[ AIndex ].X <= 130 + 70 ) )
                    {
                        if( APoints[ AIndex ].Status != TTouchStatus::Up )
                        {
                            T_ButtonsOutputPins_B::SetPinValue( true );
                            if( APoints[ AIndex ].ID )
                                T_FID_Button() = FID_Button() | 0b1000;

                            else
                                T_FID_Button() = FID_Button() | 2;

                        }

                        else
                        {
                            T_ButtonsOutputPins_B::SetPinValue( false );
                            if( APoints[ AIndex ].ID )
                                T_FID_Button() = FID_Button() & 0b0011;

                            else
                                T_FID_Button() = FID_Button() & 0b1100;
                        }

                    }

                    else if( ( APoints[ AIndex ].X >= 230 ) && ( APoints[ AIndex ].X <= 230 + 80 ) )
                    {
                        if( APoints[ AIndex ].Status != TTouchStatus::Up )
                        {
                            T_ButtonsOutputPins_C::SetPinValue( true );
                            if( APoints[ AIndex ].ID )
                                T_FID_Button() = FID_Button() | 0b1100;

                            else
                                T_FID_Button() = FID_Button() | 3;

                        }

                        else
                        {
                            T_ButtonsOutputPins_C::SetPinValue( false );
                            if( APoints[ AIndex ].ID )
                                T_FID_Button() = FID_Button() & 0b0011;

                            else
                                T_FID_Button() = FID_Button() & 0b1100;
                        }

                    }

                    if( ! AIndex )
                    {
                        ++ APoints;
                        -- AIndex;
                    }

                    -- ACount;
                }
            }

            if( ! ACount )
                return;

            FFlags() = 0;
            for( int i = 0; i < ACount; ++ i )
                if( APoints[ i ].Status != TTouchStatus::Up )
                    FFlags() = FFlags() | ( 1 << APoints[ i ].ID );

            TArduinoTouchInfo ATouchData( ACount, APoints );

//            uint8_t ATest1 = Read( FocalTech_FT6336U::REG_TH_GROUP );
//            Serial.println( ATest1 );

            T_OutputPin::SetPinValue( ATouchData );
        }

	public:
		inline void SystemInit()
		{
            pinMode( TOUCH_INTERRUPT_PIN, INPUT );

            // By default, the FT6336 will pulse the INT line for every touch
            // event. But because it shares the Wire1 TwoWire/I2C with other
            // devices, we cannot easily create an interrupt service routine to
            // handle these events. So instead, we set the INT wire to polled mode,
            // so it simply goes low as long as there is at least one valid touch.
            Write( FocalTech_FT6336U::REG_G_MODE, 0x00 );

//            Write( FocalTech_FT6336U::REG_TH_GROUP, 10 );
//            Write( FocalTech_FT6336U::REG_TH_DIFF, 10 );

            Update_EnableMonitor();
            Update_InactivityPeriod();
            Update_ReportPeriod_Active();
            Update_ReportPeriod_Monitor();
		}

		inline void SystemLoopBegin()
		{
		    if( digitalRead( TOUCH_INTERRUPT_PIN ) != LOW )
		    {
		        bool ASend = false;
		        if( FLastCount() )
		        {
		            if( FLastPoints[ 0 ].Status != TTouchStatus::Up )
		            {
		                FLastPoints[ 0 ].Status = TTouchStatus::Up;
		                ASend = true;
		            }

		            if( FLastPoints[ 1 ].Status != TTouchStatus::Up )
		            {
		                FLastPoints[ 1 ].Status = TTouchStatus::Up;
		                ASend = true;
		            }
		        }

		        if( ASend )
		            ReportResult( FLastPoints, FLastCount() );

		        return;
		    }

            uint8_t ACount = Read( FocalTech_FT6336U::REG_TD_STATUS );

//            Serial.println( ACount );

            if( ! ACount )
		        return;

            if( ACount > 2 )
		        return;

            TArduino2DTouchPoint APoints[ 2 ];

//            Serial.println( ACount );
            if( ACount == 2 )
                BeginRead( FocalTech_FT6336U::REG_P1_XH, 10 );

            else
                BeginRead( FocalTech_FT6336U::REG_P1_XH, 4 );

            uint16_t AValue = Read16();
            APoints[ 0 ].X = AValue & 0x3FFF;

            switch( AValue >> ( 8 + 6 ) )
            {
                case 0b00: APoints[ 0 ].Status = TTouchStatus::Down; break;
                case 0b01: APoints[ 0 ].Status = TTouchStatus::Up; break;
                default: APoints[ 0 ].Status = TTouchStatus::Contact; break;
            }

            AValue = Read16();
            APoints[ 0 ].Y = AValue & 0x0FFF;
            APoints[ 0 ].ID = AValue >> ( 8 + 4 );

            if( ACount == 2 )
            {
                Read16(); // Skip 2 bytes

                AValue = Read16();
                APoints[ 1 ].X = AValue & 0x3FFF;

                switch( AValue >> ( 8 + 6 ) )
                {
                    case 0b00: APoints[ 1 ].Status = TTouchStatus::Down; break;
                    case 0b01: APoints[ 1 ].Status = TTouchStatus::Up; break;
                    default: APoints[ 1 ].Status = TTouchStatus::Contact; break;
                }

                AValue = Read16();
                APoints[ 1 ].Y = AValue & 0x0FFF;
                APoints[ 1 ].ID = AValue >> ( 8 + 4 );
            }

            else
            {
                if( FLastPoints[ 1 ].Status != TTouchStatus::Up )
                {
//                    if( ( ( FLastPoints[ 0 ].X >> 2 ) == ( APoints[ 0 ].X >> 2 ) ) && ( ( FLastPoints[ 0 ].Y >> 2 ) == ( APoints[ 0 ].Y >> 2 ) ) )
                    if( FLastPoints[ 0 ].ID == APoints[ 0 ].ID )
                        APoints[ 1 ] = FLastPoints[ 1 ];

                    else
                        APoints[ 1 ] = FLastPoints[ 0 ];

                    ACount = 2;
                }

                else
                {
                    APoints[ 1 ].X = -1;
                    APoints[ 1 ].Y = -1;
                    APoints[ 1 ].ID = 3;
                }

                APoints[ 1 ].Status = TTouchStatus::Up;
            }

            if( FLastCount() == ACount )
                if( memcmp( FLastPoints, APoints, sizeof( APoints ) ) == 0 )
                    return;

            ReportResult( APoints, ACount );

            FLastCount() = ACount;
            memcpy( FLastPoints, APoints, sizeof( APoints ) );
		}

	public:
	    inline TArduinoM5StackCore2DisplayModuleTouch()
	    {
            FLastCount() = 0;
            FFlags() = 0;
            FID_Button() = 0;
	    }

    };
//---------------------------------------------------------------------------
	template <
		typename T_0_IMPLEMENTATION,
		typename T_BackgroundColor,
		typename T_Brightness,
        bool C_HAS_READ,
		typename T_Elements_Render,
        typename T_FCursorX,
        typename T_FCursorY,
		typename T_Inverted,
		typename T_Orientation,
		typename T_Text
//		typename T_TouchOutputPin
    > class DisplayILI9341_M5Stack_Core2 :
		public DisplayILI9341_Basic <
				T_0_IMPLEMENTATION,
                Mitov::DisplayILI9341_Otientation_Up,
				T_BackgroundColor,
				C_HAS_READ,
                240,
                320,
				T_Elements_Render,
                T_FCursorX,
                T_FCursorY,
				false,
				T_Orientation,
				T_Text
			>,
		public T_Brightness,
		public T_Inverted
//		public T_TouchOutputPin
    {
		typedef DisplayILI9341_Basic<
				T_0_IMPLEMENTATION,
                Mitov::DisplayILI9341_Otientation_Up,
				T_BackgroundColor,
				C_HAS_READ,
                240,
                320,
				T_Elements_Render,
                T_FCursorX,
                T_FCursorY,
				false,
				T_Orientation,
				T_Text
			> inherited;

//	public:
//		_V_PIN_( TouchOutputPin )

	public:
		_V_PROP_( Brightness )
		_V_PROP_( Inverted )

	public:
		inline void UpdateBrightness()
		{
//			Analog.Write( C_BRIGHNESS_PIN, Brightness() );

            if( Brightness().GetValue() == 0 )
            {
//            uint8_t buf = ;

//            if (State == true)
//                buf = (1 << 1) | buf;

//            else
//                buf = ~(1 << 1) & buf;

                Core2_AXP192_Func::Write1Byte( 0x12, Core2_AXP192_Func::Read8bit( 0x12 ) & 0b11111101 );

            }

            else
            {
                uint16_t AmlVoltage = Brightness().GetValue() * ( 3300 - 2500 ) + 2500;
//                Serial.begin( 9600 );
//                Serial.println( AmlVoltage );
                AmlVoltage = (AmlVoltage < 700) ? 0 : (AmlVoltage - 700) / 25;
                // Serial.printf("result:%hhu\n", (Read8bit(addr) & 0X80) | (AmlVoltage & 0X7F));
                // Serial.printf("result:%d\n", (Read8bit(addr) & 0X80) | (AmlVoltage & 0X7F));
                // Serial.printf("result:%x\n", (Read8bit(addr) & 0X80) | (AmlVoltage & 0X7F));
                Core2_AXP192_Func::Write1Byte( 0x27, ( Core2_AXP192_Func::Read8bit( 0x27 ) & 0X80 ) | ( AmlVoltage & 0X7F ) );

                Core2_AXP192_Func::Write1Byte( 0x12, Core2_AXP192_Func::Read8bit( 0x12 ) | 0b00000010 );
            }
		}

		inline void UpdateInverted()
		{
			if( Inverted() )
				inherited::cmd( 0x20 );

			else
				inherited::cmd( 0x21 );

		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
			inherited::SendCommandSequence( ILI9341_M5_Stack::Init, sizeof( ILI9341_M5_Stack::Init ) );
		}

		inline void SystemStart()
		{
//			pinMode( C_BRIGHNESS_PIN, OUTPUT );
			UpdateInverted();
			UpdateBrightness();
			inherited::SystemStart();
		}

	public:
		using inherited::inherited;

    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif


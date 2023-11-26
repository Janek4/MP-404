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
#include <ETH.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
    namespace TArduinoESP32BasicWiFiModuleEthernet_Transducer
    {
        enum TArduinoESP32BasicWiFiModuleEthernet_Transducer
        {
            LAN8720 = ETH_PHY_LAN8720,
            TLK110 = ETH_PHY_TLK110,
            RTL8201 = ETH_PHY_RTL8201,
            DP83848 = ETH_PHY_DP83848,
            DM9051 = ETH_PHY_DM9051,
            KSZ8041 = ETH_PHY_KSZ8041,
            KSZ8081 = ETH_PHY_KSZ8081
        };
    }
//---------------------------------------------------------------------------
    namespace TArduinoESP32BasicWiFiModuleEthernet_Clock
    {
        enum TArduinoESP32BasicWiFiModuleEthernet_Clock
        {
            Input_0 = ETH_CLOCK_GPIO0_IN,
            Output_0 = ETH_CLOCK_GPIO0_OUT,
            Output_16 = ETH_CLOCK_GPIO16_OUT,
            Output_17 = ETH_CLOCK_GPIO17_OUT
        };
    }
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Pin
    > class TArduinoESP32WiFiModuleEthernet_PowerPin :
		public T_Enabled,
		public T_Pin
    {
    public:
        _V_PROP_( Enabled )
		_V_PROP_( Pin )

    };
//---------------------------------------------------------------------------
	template <
        typename T_OWNER, T_OWNER &C_OWNER,
        typename T_AddressOutputPin,
		typename T_Clock,
		typename T_Config,
		typename T_ConnectedOutputPin,
		typename T_Enabled,
		typename T_FConnected,
		typename T_FullDuplexOutputPin,
		typename T_GatewayIPOutputPin,
		typename T_HostName,
		typename T_INSTANCE_EthernetCallback,
		typename T_LinkUpOutputPin,
		typename T_MACOutputPin,
		typename T_MDCPin,
		typename T_MDIOPin,
		typename T_PhysicalAddress,
		typename T_PowerPin,
		typename T_SpeedOutputPin,
		typename T_SubnetMaskIPOutputPin,
		typename T_Transducer,
		typename T_UseEFuseMAC
	> class TArduinoESP32WiFiModuleEthernet :
        public T_AddressOutputPin,
		public T_Clock,
		public T_Config,
		public T_ConnectedOutputPin,
		public T_Enabled,
		public T_FConnected,
		public T_FullDuplexOutputPin,
		public T_GatewayIPOutputPin,
		public T_HostName,
		public T_LinkUpOutputPin,
		public T_MACOutputPin,
		public T_MDCPin,
		public T_MDIOPin,
		public T_PhysicalAddress,
		public T_PowerPin,
		public T_SpeedOutputPin,
		public T_SubnetMaskIPOutputPin,
		public T_Transducer,
		public T_UseEFuseMAC
	{
    public:
        _V_PIN_( AddressOutputPin )
	    _V_PIN_( ConnectedOutputPin )
	    _V_PIN_( FullDuplexOutputPin )
		_V_PIN_( GatewayIPOutputPin )
	    _V_PIN_( LinkUpOutputPin )
	    _V_PIN_( SpeedOutputPin )
		_V_PIN_( SubnetMaskIPOutputPin )

    public:
		_V_PROP_( Clock )
		_V_PROP_( Config )
        _V_PROP_( Enabled )
	    _V_PROP_( HostName )
		_V_PROP_( MACOutputPin )
		_V_PROP_( MDCPin )
		_V_PROP_( MDIOPin )
		_V_PROP_( PhysicalAddress )
		_V_PROP_( PowerPin )
		_V_PROP_( Transducer )
		_V_PROP_( UseEFuseMAC )

    protected:
		_V_PROP_( FConnected )

    public:
        void Update_Enabled()
        {
            if (Enabled())
                StartEthernet();

            else
            {
                esp_eth_stop( ETH.eth_handle );
                esp_eth_driver_uninstall( ETH.eth_handle );
                ETH.started = false;
                ETH.eth_handle = nullptr;
            }
        }

    protected:
        void StartEthernet()
        {
            WiFi.onEvent(WiFiEvent);
            ETH.begin(PhysicalAddress().GetValue(), (PowerPin().Enabled()) ? PowerPin().Pin() : -1, MDCPin().GetValue(), MDIOPin().GetValue(), eth_phy_type_t(Transducer().GetValue()), eth_clock_mode_t(Clock().GetValue()), UseEFuseMAC().GetValue());
            if (Config().Enabled())
                ETH.config(GetIPAddressValue(Config().IP()), GetIPAddressValue(Config().Gateway()), GetIPAddressValue(Config().Subnet()), GetIPAddressValue(Config().DNS1()), GetIPAddressValue(Config().DNS2()));

        }

    public:
	    inline void EthernetCallback( WiFiEvent_t event )
	    {
            switch (event)
            {
                case ARDUINO_EVENT_ETH_START:
//                    Serial.println("ETH Started");
                    //set eth hostname here
                    if( HostName().GetValue() != "" )
                        ETH.setHostname( HostName().c_str() );

                    else if( C_OWNER.HostName().GetValue() != "" )
                        ETH.setHostname( C_OWNER.HostName().c_str() );

                    break;

//                case ARDUINO_EVENT_ETH_CONNECTED:
//                    Serial.println("ETH Connected");
//                    T_ConnectedOutputPin::SetPinValue( true );
//                    break;

                case ARDUINO_EVENT_ETH_GOT_IP:
//                    Serial.print("ETH MAC: ");
//                    Serial.print(ETH.macAddress());

                    T_MACOutputPin::SetPinValue( ETH.macAddress() );

					if( T_AddressOutputPin::GetPinIsConnected() )
						T_AddressOutputPin::SetPinValue( IPAdressToString( ETH.localIP()).c_str() );

        			if( T_GatewayIPOutputPin::GetPinIsConnected() )
                        T_GatewayIPOutputPin::SetPinValue( IPAdressToString( ETH.gatewayIP() ).c_str() );

        			if( T_SubnetMaskIPOutputPin::GetPinIsConnected() )
        				T_SubnetMaskIPOutputPin::SetPinValue( IPAdressToString( ETH.subnetMask() ).c_str() );

//                    Serial.print(", IPv4: ");
//                    Serial.print(ETH.localIP());

                    T_FullDuplexOutputPin::SetPinValue( ETH.fullDuplex() );

//                    if( ETH.fullDuplex() )
//                        Serial.print(", FULL_DUPLEX");

                    T_LinkUpOutputPin::SetPinValue( ETH.linkUp() );

//                    Serial.print(", ");
//                    Serial.print(ETH.linkSpeed());
//                    Serial.println("Mbps");

                    T_SpeedOutputPin::SetPinValue( ETH.linkSpeed() );

                    FConnected() = true;
                    T_ConnectedOutputPin::SetPinValue( true );
                    break;

                case ARDUINO_EVENT_ETH_DISCONNECTED:
//                    Serial.println("ETH Disconnected");
                    FConnected() = false;
                    T_ConnectedOutputPin::SetPinValue( false );
                    break;

                case ARDUINO_EVENT_ETH_STOP:
//                    Serial.println("ETH Stopped");
                    FConnected() = false;
                    T_ConnectedOutputPin::SetPinValue( false );
                    break;

                default:
                    break;
            }
	    }

    protected:
        static void WiFiEvent( WiFiEvent_t event )
        {
            T_INSTANCE_EthernetCallback::Call( event );
        }

    public:
        inline void GetIsStarted( bool &AValue )
        {
            AValue = FConnected();
        }

    public:
        inline void SystemInit()
        {
            T_ConnectedOutputPin::SetPinValue( false );
            T_FullDuplexOutputPin::SetPinValue( false );
            T_LinkUpOutputPin::SetPinValue( false );

            if (Enabled())
                StartEthernet();

        }

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
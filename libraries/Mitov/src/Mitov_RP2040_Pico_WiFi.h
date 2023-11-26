////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <WiFi.h>
#include <Mitov.h>
#include <Mitov_BasicEthernet.h>
//#include <cyw43.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_ChannelOutputPin,
		typename T_CountOutputPin,
        typename T_Enabled,
		typename T_EncryptionOutputPin,
		typename T_FScanRequest,
		typename T_FailedOutputPin,
		typename T_FoundNetworkOutputPin,
		typename T_MACAddressOutputPin,
		typename T_SSIDOutputPin,
		typename T_ScanningOutputPin,
		typename T_SignalStrengthOutputPin
//		typename T_Timeout
	> class RP2040_Pico_WiFiScanNetworksOperation :
		public T_ChannelOutputPin,
		public T_CountOutputPin,
        public T_Enabled,
		public T_EncryptionOutputPin,
		public T_FScanRequest,
		public T_FailedOutputPin,
		public T_FoundNetworkOutputPin,
		public T_MACAddressOutputPin,
		public T_SSIDOutputPin,
		public T_ScanningOutputPin,
		public T_SignalStrengthOutputPin
//		public T_Timeout
	{
	public:
		_V_PIN_( ScanningOutputPin )
		_V_PIN_( FailedOutputPin )
		_V_PIN_( CountOutputPin )
		_V_PIN_( SignalStrengthOutputPin )
		_V_PIN_( SSIDOutputPin )
		_V_PIN_( MACAddressOutputPin )
		_V_PIN_( EncryptionOutputPin )
		_V_PIN_( ChannelOutputPin )
		_V_PIN_( FoundNetworkOutputPin )

	public:
		_V_PROP_( Enabled )
//		_V_PROP_( Timeout )

	protected:
		_V_PROP_( FScanRequest )

//	protected:
//		unsigned long FScanTime;

	public:
		inline void SystemStart()
		{
			T_ScanningOutputPin::SetPinValue( FScanRequest(), false );
			ScanInputPin_o_Receive( nullptr );
		}

		inline void SystemLoopBegin()
		{
//			Serial.println( "." );
			T_ScanningOutputPin::SetPinValue( FScanRequest(), true );
			if( FScanRequest() )
			{
//				Serial.print( "+" );
				int8_t AComplete = WiFi.scanComplete();
//				Serial.print( "-" );
//				if( AComplete != -1 )
//				    Serial.println( AComplete );

/*
				if( AComplete == -1 )
				{
					T_FailedOutputPin::ClockPin();
					T_CountOutputPin::SetPinValue( 0 );
					FScanRequest() = false;
				}

				else 
 */
                if( AComplete >= 0 )
				{
//					Serial.println( "TEST1" );
					T_ScanningOutputPin::SetPinValue( false );
					if( Enabled() )
					{
//						Serial.println( "TEST2" );
//						delay( 1000 );
						T_CountOutputPin::SetPinValue( (int)AComplete );
						for( int i = 0; i < AComplete; ++ i )
						{
							Mitov::String ssid;
							uint8_t encryptionType;
							int32_t ASetrength;
							uint8_t* BSSID;
							int32_t channel;

							if( T_SSIDOutputPin::GetPinIsConnected() )
								T_SSIDOutputPin::SetPinValue( WiFi.SSID( i ));

							if( T_SignalStrengthOutputPin::GetPinIsConnected() )
								T_SignalStrengthOutputPin::SetPinValue( WiFi.RSSI( i ));

							if( T_EncryptionOutputPin::GetPinIsConnected() )
								T_EncryptionOutputPin::SetPinValue( WiFi.encryptionType( i ));

							if( T_MACAddressOutputPin::GetPinIsConnected() )
							{
								uint8_t AMACAddress[6] = {0};

								WiFi.BSSID( i, AMACAddress );

								T_MACAddressOutputPin::SetPinValue( MACAdressToString( AMACAddress ) );
							}

							if( T_ChannelOutputPin::GetPinIsConnected() )
								T_ChannelOutputPin::SetPinValue( WiFi.channel( i ) );

							T_FoundNetworkOutputPin::ClockPin();
						}
					}

                    WiFi.scanDelete();
					FScanRequest() = false;
				}
			}
		}

	public:
		inline void ScanInputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			if( ! C_OWNER.Enabled().GetValue() )
				return;

			if( WiFi.scanComplete() != -1 )
			{
				WiFi.scanNetworks( true );
//				Serial.println( "scanNetworks" );
//				WiFi.scanNetworks( false, ShowHidden );
				FScanRequest() = true;
				T_ScanningOutputPin::SetPinValue( true );
			}
/*
			for( int i = 0; i < nearbyAccessPointCount; i ++ )
			{
				FoundSSIDOutputPin.Notify( LWiFi.SSID( i ));

				int32_t ASetrength = LWiFi.RSSI( i );
				FoundSignalStrengthOutputPin.Notify( &ASetrength );
			}
*/
		}

	public:
		inline RP2040_Pico_WiFiScanNetworksOperation()
		{
			FScanRequest() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
//		typename T_Channel,
		typename T_Config,
		typename T_Enabled,
		typename T_MacAddress,
		typename T_Password,
		typename T_SSID
	> class RP2040_Pico_WiFiModuleRemoteAccessPoint :
//		public T_Channel,
		public T_Config,
		public T_Enabled,
		public T_MacAddress,
		public T_Password,
		public T_SSID
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( SSID )
	    _V_PROP_( Password )
//		_V_PROP_( Channel )
		_V_PROP_( MacAddress )
		_V_PROP_( Config )

	public:
        inline void UpdateEnabled()
        {
            C_OWNER.WiFi_Disconnect_Update();
        }

	public:
		void Connect()
		{
			if( ! Enabled() )
				return;

			if( SSID().GetValue() == "" )
				return;

			if( Config().Enabled() )
				C_OWNER.WiFi_config( GetIPAddressValue( Config().IP() ), GetIPAddressValue( Config().Gateway() ), GetIPAddressValue( Config().Subnet() ), GetIPAddressValue( Config().DNS1() ), GetIPAddressValue( Config().DNS2() ) );

//			Serial.println( SSID );
//			Serial.println( APassword );

			Begin();

//			if( C_OWNER.HostName != "" )
//				WiFi.setHostname( (char *)C_OWNER.HostName.c_str() );

		}

		void Begin()
		{
//		    Serial.println( "TEST1" );
			const char *APassword;
			if( Password().GetValue() == "" )
				APassword = nullptr;

			else
				APassword = Password().c_str();

/*
			int32_t channel;

			if( Channel().Enabled() )
				channel = Channel().Channel();

			else
				channel = 0;
*/
			uint8_t AMacAddress[ 6 ];
			const uint8_t* bssid;

			if( MacAddress().Enabled() )
			{
				MacAddress().MacAddress().GetMacAddress( AMacAddress );
				bssid = AMacAddress;
			}

			else
				bssid = nullptr;

#ifdef __RP2040_WIFI__DEBUG__
			Serial.print( "WiFi.begin " );
			Serial.print( SSID().c_str() );
			Serial.print( " : " );
			Serial.println( APassword );

#endif // __RP2040_WIFI__DEBUG__

			C_OWNER.WiFi_disconnect();
			if( C_OWNER.HostName().GetValue() != "" )
				C_OWNER.WiFi_setHostname( (char *)C_OWNER.HostName().c_str() );

			C_OWNER.WiFi_begin( (char *)SSID().c_str(), APassword, /*channel,*/ bssid ); //, false );


//			C_OWNER.WiFi_reconnect();
		}

	public:
		inline void IsEnabled( bool &AIsEnabled )
		{
			if( ( ! Enabled() ) || ( SSID().GetValue() == "" ))
				return;

//			Serial.println( "IsEnabled" );

			AIsEnabled = true;
		}

		inline void TryConnect( bool &AIsEnabled )
		{
			if( ( ! Enabled().GetValue() ) || ( SSID().GetValue() == "" ))
				return;

//			Serial.println( "TryConnect" );
			AIsEnabled = true;
			Connect();
		}

		inline void AssignDNS()
		{
//				Serial.println( Config.DNS.IP.toString() );
			if( Config().DNS1().GetUInt32Value() == 0 )
			{
				IPAddress AIP = C_OWNER.WiFi_gatewayIP();
//				C_OWNER.WiFi_disconnect();

//					delay( 10000 );

//					Serial.println( AIP.toString() );
				if( Config().Enabled() )
					C_OWNER.WiFi_config( GetIPAddressValue( Config().IP() ), GetIPAddressValue( Config().Gateway() ), GetIPAddressValue( Config().Subnet() ), AIP, GetIPAddressValue( Config().DNS2() ) );

				else
					C_OWNER.WiFi_config( ::IPAddress( 0, 0, 0, 0 ), ::IPAddress( 0, 0, 0, 0 ), ::IPAddress( 255, 255, 255, 0 ), AIP, ::IPAddress( 0, 0, 0, 0 ) );

/*
				ip_addr_t d;
				d.type = IPADDR_TYPE_V4;

				// Set DNS1-Server
				d.u_addr.ip4.addr = static_cast<uint32_t>( AIP );

				dns_setserver( 0, &d );
*/
				Begin();
//					WiFi.reconnect();
//					if(esp_wifi_disconnect() == ESP_OK)
//						esp_wifi_connect();
			}
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Channel,
		typename T_Config,
		typename T_Enabled,
		typename T_IsHidden,
		typename T_MaxConnections,
		typename T_Password,
		typename T_SSID
	> class RP2040_Pico_WiFiModuleAccessPoint :
		public T_Channel,
		public T_Config,
		public T_Enabled,
		public T_IsHidden,
		public T_MaxConnections,
		public T_Password,
		public T_SSID
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( IsHidden )
		_V_PROP_( Channel )

		_V_PROP_( SSID )
		_V_PROP_( MaxConnections )
		_V_PROP_( Password )

		_V_PROP_( Config )

	public:
		inline void TryStart()
		{
//			if( AOwner->Enabled )
				if( Enabled() )
					if( SSID().GetValue() != "" )
					{
	//Serial.println( "TEST1" );
	//Serial.println( SSID );
	//Serial.println( IsHidden );
						if( Config().Enabled() )
							WiFi.softAPConfig( GetIPAddressValue( Config().IP() ), GetIPAddressValue( Config().Gateway() ), GetIPAddressValue( Config().Subnet() ) );

						if( Password().GetValue() != "" )
							WiFi.softAP( SSID().c_str(), Password().c_str(), Channel(), IsHidden(), MaxConnections() );

						else
	//						WiFi.softAP( SSID().c_str() );
							WiFi.softAP( SSID().c_str(), nullptr, Channel(), IsHidden(), MaxConnections() );

//						if( AOwner->HostName != "" )
//							WiFi.setHostname( (char *)AOwner->HostName.c_str() );

//						WiFi.mode( WIFI_AP );
						WiFi.softAPIP();
					}

		}

	};
//---------------------------------------------------------------------------
	namespace TArduinoRP2040_RaspberryPi_Pico_WiFi_PowerMode
	{
		enum TArduinoRP2040_RaspberryPi_Pico_WiFi_PowerMode
		{
			LowPowerAggressive,
			LowPower,
			Normal,
//			Performance
		};
	}
//---------------------------------------------------------------------------
	template <
		typename T_AccessPoint,
		typename T_AccessPoints_AssignDNS,
		typename T_AccessPoints_IsEnabled,
		typename T_AccessPoints_TryConnect,
		typename T_AddressOutputPin,
		typename T_BSSIDOutputPin,
		uint32_t C_COUNT_AccessPoints,
		uint32_t C_COUNT_Scan_Elements,
		uint32_t C_COUNT_Sockets,
		typename T_Enabled,
		typename T_FAccessPointIndex,
		typename T_FDNSUpdated,
		typename T_FIsStarted,
		typename T_FRemoteStarted,
		typename T_GatewayIPOutputPin,
		typename T_HostName,
		typename T_MACOutputPin,
		typename T_PowerMode,
		typename T_RemoteConnectedOutputPin,
		typename T_Sockets_StopSocket,
		typename T_SubnetMaskIPOutputPin
	> class RP2040_Pico_WiFiModule :
		public T_AccessPoint,
		public T_AddressOutputPin,
		public T_BSSIDOutputPin,
		public T_Enabled,
		public T_FAccessPointIndex,
		public T_FDNSUpdated,
		public T_FIsStarted,
		public T_FRemoteStarted,
		public T_GatewayIPOutputPin,
		public T_HostName,
		public T_MACOutputPin,
		public T_PowerMode,
		public T_RemoteConnectedOutputPin,
		public T_SubnetMaskIPOutputPin
	{
	public:
		_V_PIN_( AddressOutputPin )
		_V_PIN_( BSSIDOutputPin )
		_V_PIN_( GatewayIPOutputPin )
		_V_PIN_( MACOutputPin )
		_V_PIN_( RemoteConnectedOutputPin )
		_V_PIN_( SubnetMaskIPOutputPin )

	public:
		_V_PROP_( AccessPoint )
		_V_PROP_( Enabled )
		_V_PROP_( PowerMode )
		_V_PROP_( HostName )

	protected:
		_V_PROP_( FAccessPointIndex )
		_V_PROP_( FDNSUpdated )
		_V_PROP_( FIsStarted )
		_V_PROP_( FRemoteStarted )

	public:
		unsigned long	FLastReconnectTime;

	public:
		void WiFi_Disconnect_Update()
		{
//			T_Sockets_StopSocket::Call();

//			for( int i = 0; i < C_COUNT_Sockets; ++i )
//				Sockets[ i ]->StopSocket();

#ifdef __RP2040_WIFI__DEBUG__
			Serial.println( "WiFi_Disconnect_Update" );
#endif
//			WiFi.disconnect( false, true );
			WiFi.disconnect( false );
			FIsStarted() = false;
			FRemoteStarted() = false;
			FDNSUpdated() = false;
		}

		inline void WiFi_disconnect()
		{
			WiFi.disconnect();
		}

/*
		inline void WiFi_reconnect()
		{
//			WiFi.reconnect();
			if( ! Enabled() )
				return;

            StopEthernet();
            StartEthernet();
		}
*/
		inline void WiFi_config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2)
		{
//			Serial.println( "WiFi_config" );
//			WiFi.config(local_ip, gateway, subnet, dns1, dns2);
			WiFi.config( local_ip, gateway, subnet );
			WiFi.setDNS( dns1, dns2 );
		}

		inline void WiFi_setHostname(const char * hostname)
		{
			WiFi.setHostname( hostname );
		}

		inline IPAddress WiFi_gatewayIP()
		{
			return WiFi.gatewayIP();
		}

		inline void WiFi_begin(const char* ssid, const char *passphrase, /*int32_t channel,*/ const uint8_t* bssid ) //, bool connect)
		{
//			Serial.println( ssid );
//			Serial.println( passphrase );
			WiFi.begin( ssid, passphrase, /*channel,*/ bssid ); //, connect );

//			for(;;)
//			  delay( 1000 );
		}

	public:
		void UpdateAccessPoint() // Used by bindings
		{
//		    Serial.println( "UpdateAccessPoint" );
//            if( ManualConnections() )
//                return;

			if( AccessPoint().Enabled() )
			{
				if( Enabled() )
					AccessPoint().TryStart();
			}

			else
				WiFi.softAPdisconnect();

		}

        void RestartEthernet()
		{
			if( ! Enabled() )
				return;

//			C_SERIAL.end();
//			Ethernet.end();
			StartEthernet();
		}

		bool GetIPFromHostName( Mitov::String AHostName, ::IPAddress &AAdress )
		{
//			Serial.println( "GetIPFromHostName" );
//			Serial.println( AHostName );
			bool AResult = ( WiFi.hostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

//			Serial.println( AAdress.toString());
			return AResult;
		}

	protected:
		void StopEthernet()
		{
			T_Sockets_StopSocket::Call();

//			for( int i = 0; i < C_COUNT_Sockets; ++i )
//				Sockets[ i ]->StopSocket();

#ifdef __RP2040_WIFI__DEBUG__
			Serial.println( "StopEthernet" );
#endif
			WiFi.disconnect( true );
			FIsStarted() = false;
			FRemoteStarted() = false;
			FDNSUpdated() = false;
		}

		void StartEthernet()
		{
#ifdef __RP2040_WIFI__DEBUG__
			Serial.println( "StartEthernet" );
#endif
/*
            if( ManualConnections() )
            {

            }

            else
*/
			{

			//WiFi.begin();
//			WiFi.disconnect( true, true );
//			WiFi.disconnect( false, true );
//WiFi.mode( WIFI_STA );
//return;
//			Serial.println( "TRY CONNECT" );

	    		if( HostName().GetValue() != "" )
    				WiFi.setHostname( (char *)HostName().c_str() );

//	Serial.println( "TEST3" );
//	    		WiFi.begin();
//    			WiFi.disconnect( false, true );
    			WiFi.disconnect( false );
//WiFi.mode( WIFI_OFF );
//delay( 3000 );
//WiFi.mode( WIFI_STA );
//WiFi.mode( WIFI_AP_STA );
//			WiFi.begin();
//				esp_wifi_set_ps( WIFI_PS_NONE );
//delay( 3000 );
//return;
//			esp_wifi_disconnect();

//		    	WiFi.setAutoReconnect( AutoReconnect().GetValue() );

//	Serial.println( "TEST4" );
	    		if( HostName().GetValue() != "" )
    			{
				    WiFi.setHostname( (char *)HostName().c_str() );
//				    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, (char *)HostName.c_str() );
//				    WiFi.setHostname( "robot" );
//				    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_ETH, "robot" );
//				    espconn_mdns_set_hostname( (char *) HostName.c_str() );
			    }
//#ifndef VISUINO_ESP32_C3
//		    	WiFi.mode( WIFI_OFF );
#ifdef __RP2040_WIFI__DEBUG__
	    		Serial.println( "WIFI_OFF" );
#endif
//#endif // VISUINO_ESP32_C3

#if 0
                if( ( AccessPoint().Enabled() || ( ! AccessPoint().Enabled().GetIsConstant() )) &&
                    ( ( AccessPoint().SSID().GetValue() != "" ) || ( ! AccessPoint().SSID().GetIsConstant() ))
                  )
                {
                    if( /*C_ESPNow ||*/ C_COUNT_AccessPoints )
                    {
        	        	WiFi.mode( WIFI_AP_STA );
#ifdef __RP2040_WIFI__DEBUG__
    		    	    Serial.println( "WIFI_AP_STA" );
#endif
            	    }

                    else
            	    {
        	            WiFi.mode( WIFI_AP );
#ifdef __RP2040_WIFI__DEBUG__
    	    		    Serial.println( "WIFI_AP" );
#endif
            	    }

                }

                else if( C_COUNT_AccessPoints > 0 )
                {
                	WiFi.mode( WIFI_STA );
#ifdef __RP2040_WIFI__DEBUG__
    		    	    Serial.println( "WIFI_STA" );
#endif
                }

                else
                {
                    if( /*C_ESPNow ||*/ C_COUNT_Scan_Elements )
                    {
                        WiFi.mode( WIFI_STA );
#ifdef __RP2040_WIFI__DEBUG__
    	    		    Serial.println( "WIFI_STA" );
#endif
                    }

                    else
                    {
        	    	    WiFi.mode( WIFI_OFF );
#ifdef __RP2040_WIFI__DEBUG__
    	    		    Serial.println( "WIFI_OFF" );
#endif
            		}
                }
#endif
//WiFi.mode( WIFI_STA );
//return;
//	Serial.println( "TEST5" );
			    if( Enabled() )
				    AccessPoint().TryStart();

			}

			if( T_BSSIDOutputPin::GetPinIsConnected() )
			{
				uint8_t mac[6];
				WiFi.BSSID( mac );
				T_BSSIDOutputPin::SetPinValue( MACAdressToString( mac ) );
			}

			if( T_GatewayIPOutputPin::GetPinIsConnected() )
				T_GatewayIPOutputPin::SetPinValue( IPAdressToString( WiFi.gatewayIP() ).c_str() );

			if( T_SubnetMaskIPOutputPin::GetPinIsConnected() )
				T_SubnetMaskIPOutputPin::SetPinValue( IPAdressToString( WiFi.subnetMask() ).c_str() );

		}

	public:
		inline void UpdateEnabled()
		{
//		    Serial.println( "UpdateEnabled" );
			if( Enabled() )
				StartEthernet();

			else
				StopEthernet();

		}

		inline void UpdatePowerMode()
		{
//			uint32_t AValue;
			switch( PowerMode().GetValue() )
			{
				case TArduinoRP2040_RaspberryPi_Pico_WiFi_PowerMode::LowPowerAggressive : WiFi.aggressiveLowPowerMode(); break;
				case TArduinoRP2040_RaspberryPi_Pico_WiFi_PowerMode::LowPower : WiFi.defaultLowPowerMode(); break;
				case TArduinoRP2040_RaspberryPi_Pico_WiFi_PowerMode::Normal : WiFi.noLowPowerMode(); break;
//				case TArduinoRP2040_RaspberryPi_Pico_WiFi_PowerMode::Performance : AValue = CYW43_PERFORMANCE_PM; break;					
			}

		}

	public:
		inline bool GetIsStarted()
		{
            return FIsStarted();
/*
		    if( FIsStarted() )
		        return true;

		    bool AStarted = false;
		    T_Ethernet_GetIsStarted::Call( AStarted );
		    return AStarted;
*/
		}

	public:
		inline void SystemInit()
		{
			if( Enabled() )
				StartEthernet();

		}

		inline void SystemStart()
		{
			FLastReconnectTime = millis() - 30000;
			UpdatePowerMode();

			T_RemoteConnectedOutputPin::SetPinValue( WiFi.isConnected(), false );
			if( T_MACOutputPin::GetPinIsConnected() )
				T_MACOutputPin::SetPinValue( WiFi.macAddress().c_str() );

		}

		inline void SystemLoopBegin()
		{
			int ARes = WiFi.status();

//			Serial.print( "STATUS: " );
//			Serial.println( ARes );

			bool AIsEnabled = false; //( C_COUNT_AccessPoints == 0 );

			if( ! WiFi.isConnected() )
			{
				if( FRemoteStarted() )
					FLastReconnectTime = millis();

#ifdef __RP2040_WIFI__DEBUG__
//				Serial.println( "! WiFi.isConnected()" );
#endif
//				Serial.println( "! WiFi.isConnected()" );
//				FIsStarted() = false;
				FRemoteStarted() = false;
//				FDNSUpdated() = false;
			}

			if( ( C_COUNT_AccessPoints > 0 ) /* && ( ! ManualConnections().GetValue() ) */ )
//				if( ! FRemoteStarted() )
				{
//#ifdef __RP2040_WIFI__DEBUG__
//					Serial.print( "TRY CONNECT: " );
//					Serial.println( ARes );
//#endif // __RP2040_WIFI__DEBUG__

//					if( ( ARes != WL_CONNECTED )
#ifndef SEEEDUINO_WIO_TERMINAL
//						&& ( ARes != WL_DISCONNECTED )
#endif
//						&& ( ARes != WL_IDLE_STATUS ) )
    				if( ! FRemoteStarted().GetValue() )
					{
						unsigned long ATime = millis();
						if( ( ATime - FLastReconnectTime ) > 30000 )
						{
#ifdef __RP2040_WIFI__DEBUG__
							Serial.print( "STATUS: " );
							Serial.println( ARes );
							Serial.println( FAccessPointIndex().GetValue() );
#endif // __RP2040_WIFI__DEBUG__
							FAccessPointIndex() = FAccessPointIndex() + 1;
							if( FAccessPointIndex().GetValue() > C_COUNT_AccessPoints )
								FAccessPointIndex() = 1;

							T_AccessPoints_TryConnect::Call( FAccessPointIndex().GetValue() - 1, AIsEnabled );
							FLastReconnectTime = millis();
							FDNSUpdated() = false;
						}

    					else
    						T_AccessPoints_IsEnabled::Call( FAccessPointIndex().GetValue() - 1, AIsEnabled );
					}

					else
						T_AccessPoints_IsEnabled::Call( FAccessPointIndex().GetValue() - 1, AIsEnabled );

				}

			T_RemoteConnectedOutputPin::SetPinValue( WiFi.isConnected(), true );

//			Serial.println( AIsEnabled );

			if( ! AIsEnabled )
				FIsStarted() = true;

			else if( WiFi.isConnected() )
				if( ( ! FRemoteStarted() ) /*|| ManualConnections().GetValue() */ )
				{
					if( ! FDNSUpdated() )
					{
#ifdef __RP2040_WIFI__DEBUG__
						Serial.println( "DNS Updated" );
#endif // __RP2040_WIFI__DEBUG__
						FDNSUpdated() = true;
//							bool AIsEnabled = false;
						T_AccessPoints_AssignDNS::Call( FAccessPointIndex().GetValue() - 1 );

//						Serial.println( "DNS Updated 1" );
						return;
					}

					else
					{
#ifdef __RP2040_WIFI__DEBUG__
						Serial.println( "FRemoteStarted() = true" );
#endif // __RP2040_WIFI__DEBUG__
						FIsStarted() = true;
						FRemoteStarted() = true;
					}

/*
					if( HostName != "" )
					{
						WiFi.setHostname( (char *)HostName.c_str() );
//						WiFi.disconnect();
//						WiFi.reconnect();
//						ESP.restart();
//						Serial.println( HostName );
					}
*/
					if( T_AddressOutputPin::GetPinIsConnected() )
						T_AddressOutputPin::SetPinValue( IPAdressToString( WiFi.localIP()).c_str() );

					if( T_MACOutputPin::GetPinIsConnected() )
						T_MACOutputPin::SetPinValue( WiFi.macAddress().c_str() );

					if( T_BSSIDOutputPin::GetPinIsConnected() )
                    {
                        uint8_t mac[6];
                        WiFi.BSSID( mac );
                        T_BSSIDOutputPin::SetPinValue( MACAdressToString( mac ) );
                    }

					if( T_GatewayIPOutputPin::GetPinIsConnected() )
						T_GatewayIPOutputPin::SetPinValue( IPAdressToString( WiFi.gatewayIP() ).c_str() );

					if( T_SubnetMaskIPOutputPin::GetPinIsConnected() )
						T_SubnetMaskIPOutputPin::SetPinValue( IPAdressToString( WiFi.subnetMask() ).c_str() );

				}

//			for( int i = 0; i < C_COUNT_Sockets; ++i )
//				Sockets[ i ]->TryStartSocket();

//			WiFi.setHostname( (char *)HostName.c_str() );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Count,
		typename T_Enabled,
		typename T_IdlePeriod,
		typename T_Interval
	> class TArduinoRP2040_RaspberryPi_Pico_KeepAlive :
		public T_Count,
		public T_Enabled,
		public T_IdlePeriod,
		public T_Interval
	{
	public:
		_V_PROP_( Count )
		_V_PROP_( Enabled )
		_V_PROP_( IdlePeriod )
		_V_PROP_( Interval )

	};
//---------------------------------------------------------------------------
	template <
		typename T_CLIENT,
		typename T_KeepAlive,
		typename T_NoDelay,
		typename T_Sync
	> class RP2040_Pico_TCPClientSocket_Implementation :
		public T_KeepAlive,
		public T_NoDelay,
		public T_Sync
	{
	public:
		_V_PROP_( KeepAlive )
		_V_PROP_( NoDelay )
		_V_PROP_( Sync )

	protected:
		T_CLIENT FClient;

	public:
		inline void Update_KeepAlive()
		{
			if( KeepAlive().Enabled() )
				FClient.keepAlive( KeepAlive().IdlePeriod(), KeepAlive().Interval(), KeepAlive().Count() );

			else
				FClient.disableKeepAlive();
		}

		inline void Update_Sync()
		{
			FClient.setSync( Sync() );
		}

		inline void Update_NoDelay()
		{
			FClient.setNoDelay( NoDelay() );
		}

	public:
		inline void ApplyStartParameters()
		{
			Update_KeepAlive();
			Update_Sync();
			Update_NoDelay();
//			Serial.println( "ApplyStartParameters" );
//			Serial.println( FClient.isKeepAliveEnabled() );
		}

		constexpr inline bool GetIsSecure() { return false; }


	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
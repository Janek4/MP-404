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
#include <Mitov_BasicEthernet.h>
#ifdef SEEEDUINO_WIO_TERMINAL
    #include <rpcWiFi.h>

#else
    #include <WiFi.h>
    #include <esp_wifi.h>
    #include <lwip/dns.h>
//    #include <esp_wifi_types.h>
//    #include "ip_addr.h"
//    #include "espconn.h"
#endif

#include <Mitov_ESP32_WiFi_Elements.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

//  #define __ESP32_WIFI__DEBUG__

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_Channel,
		typename T_Config,
		typename T_Enabled,
		typename T_IsHidden,
		typename T_MaxConnections,
		typename T_Password,
		typename T_SSID
	> class ESP32WiFiModuleAccessPoint :
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
	template <
	    typename T_Enabled,
	    typename T_Number
    > class TArduinoESP32WiFiModuleScanNetworksOperationChannel :
	    public T_Enabled,
	    public T_Number
    {
	public:
		_V_PROP_( Enabled )
		_V_PROP_( Number )

    };
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
#ifndef SEEEDUINO_WIO_TERMINAL
	    typename T_Channel,
#endif
		typename T_ChannelOutputPin,
		typename T_CountOutputPin,
		typename T_Enabled,
		typename T_EncryptionOutputPin,
		typename T_FScanRequest,
		typename T_FailedOutputPin,
		typename T_FoundNetworkOutputPin,
		typename T_MACAddressOutputPin,
		typename T_MaxTimrPerChannel,
		typename T_Passive,
		typename T_SSIDOutputPin,
		typename T_ScanningOutputPin,
		typename T_ShowHidden,
		typename T_SignalStrengthOutputPin
	> class ESP32ModuleScanNetworksOperation :
#ifndef SEEEDUINO_WIO_TERMINAL
	    public T_Channel,
#endif
		public T_ChannelOutputPin,
		public T_CountOutputPin,
		public T_Enabled,
		public T_EncryptionOutputPin,
		public T_FScanRequest,
		public T_FailedOutputPin,
		public T_FoundNetworkOutputPin,
		public T_MACAddressOutputPin,
		public T_MaxTimrPerChannel,
		public T_Passive,
		public T_SSIDOutputPin,
		public T_ScanningOutputPin,
		public T_ShowHidden,
		public T_SignalStrengthOutputPin
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
#ifndef SEEEDUINO_WIO_TERMINAL
	    _V_PROP_( Channel )
#endif
	    _V_PROP_( Enabled )
		_V_PROP_( ShowHidden )
		_V_PROP_( MaxTimrPerChannel )
		_V_PROP_( Passive )

	protected:
		_V_PROP_( FScanRequest )

	public:
		inline void SystemStart()
		{
			T_ScanningOutputPin::SetPinValue( FScanRequest(), false );
			ScanInputPin_o_Receive( nullptr );
		}

		inline void SystemLoopBegin()
		{
			T_ScanningOutputPin::SetPinValue( FScanRequest(), true );
			if( FScanRequest() )
			{
				int8_t AComplete = WiFi.scanComplete();
//				if( AComplete != -1 )
//				    Serial.println( AComplete );

				if( AComplete == WIFI_SCAN_FAILED )
				{
					if( Enabled() )
					{
						T_FailedOutputPin::ClockPin();
						T_CountOutputPin::SetPinValue( 0 );
					}

					FScanRequest() = false;
				}

				else if( AComplete >= 0 )
				{
					if( Enabled() )
					{
						T_CountOutputPin::SetPinValue( (int)AComplete );
						for( int i = 0; i < AComplete; ++ i )
						{
							Mitov::String ssid;
							uint8_t encryptionType;
							int32_t ASetrength;
							uint8_t* BSSID;
							int32_t channel;
	//						bool isHidden;
							if( WiFi.getNetworkInfo( i, ssid, encryptionType, ASetrength, BSSID, channel )) //, isHidden ))
							{
								Mitov::String BSSIDStr = WiFi.BSSIDstr( i );

								T_SSIDOutputPin::SetPinValue( ssid.c_str() );
								T_SignalStrengthOutputPin::SetPinValue( ASetrength );
								T_EncryptionOutputPin::SetPinValue( encryptionType );
								T_MACAddressOutputPin::SetPinValue( BSSIDStr.c_str() );
								T_ChannelOutputPin::SetPinValue( channel );
	//							IsHiddenOutputPin.Notify( &isHidden );
							}

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

			if( WiFi.scanComplete() != WIFI_SCAN_RUNNING )
			{
#ifdef SEEEDUINO_WIO_TERMINAL
				WiFi.scanNetworks( true, ShowHidden(), Passive(), MaxTimrPerChannel() );
#else
                uint8_t AChannel = Channel().Enabled() ? Channel().Number() : 0;
				WiFi.scanNetworks( true, ShowHidden(), Passive(), MaxTimrPerChannel(), AChannel );
#endif
//				Serial.println( "scanNetworks" );
//				WiFi.scanNetworks( false, ShowHidden );
				FScanRequest() = true;
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
		inline ESP32ModuleScanNetworksOperation()
		{
			FScanRequest() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
        typename T_Enabled,
        typename T_MaxSaving
    > class TArduinoESP32WiFiLowPower :
        public T_Enabled,
        public T_MaxSaving
    {
	public:
		_V_PROP_( Enabled )
		_V_PROP_( MaxSaving )

    };
//---------------------------------------------------------------------------
	template <
		typename T_AccessPoint,
		typename T_AccessPoints_AssignDNS,
		typename T_AccessPoints_IsEnabled,
		typename T_AccessPoints_TryConnect,
		typename T_AddressOutputPin,
		typename T_AutoReconnect,
		typename T_BSSIDOutputPin,
		uint32_t C_COUNT_AccessPoints,
		uint32_t C_COUNT_Scan_Elements,
		uint32_t C_COUNT_Sockets,
		bool C_ESPNow,
		typename T_Enabled,
		typename T_Ethernet_GetIsStarted,
		typename T_FAccessPointIndex,
		typename T_FDNSUpdated,
		typename T_FIsStarted,
		typename T_FRemoteStarted,
		typename T_GatewayIPOutputPin,
		typename T_HostName,
		typename T_LowPower,
		typename T_MACOutputPin,
		typename T_ManualConnections,
		typename T_RemoteConnectedOutputPin,
		typename T_Sockets_StopSocket,
		typename T_SubnetMaskIPOutputPin,
		typename T_TransmitPower
	> class ESP32WiFiModule :
		public T_AccessPoint,
		public T_AddressOutputPin,
		public T_AutoReconnect,
		public T_BSSIDOutputPin,
		public T_FDNSUpdated,
		public T_Enabled,
		public T_FAccessPointIndex,
		public T_FIsStarted,
		public T_GatewayIPOutputPin,
		public T_HostName,
		public T_LowPower,
		public T_MACOutputPin,
		public T_ManualConnections,
		public T_RemoteConnectedOutputPin,
		public T_FRemoteStarted,
		public T_SubnetMaskIPOutputPin,
		public T_TransmitPower
	{
	public:
		_V_PIN_( AddressOutputPin )
		_V_PIN_( MACOutputPin )
		_V_PIN_( BSSIDOutputPin )
		_V_PIN_( GatewayIPOutputPin )
		_V_PIN_( SubnetMaskIPOutputPin )
		_V_PIN_( RemoteConnectedOutputPin )

	public:
		_V_PROP_( AccessPoint )
		_V_PROP_( AutoReconnect )
		_V_PROP_( HostName )
		_V_PROP_( LowPower )
		_V_PROP_( ManualConnections )
		_V_PROP_( TransmitPower )

	public:
		_V_PROP_( FAccessPointIndex )
		_V_PROP_( FIsStarted )
		_V_PROP_( FRemoteStarted )
		_V_PROP_( FDNSUpdated )

	public:
		_V_PROP_( Enabled )

	public:
		unsigned long	FLastReconnectTime;

	public:
		inline void UpdateEnabled()
		{
//		    Serial.println( "UpdateEnabled" );
			if( Enabled() )
				StartEthernet();

			else
				StopEthernet();

		}

		void Update_TransmitPower()
		{
/*
    WIFI_POWER_19_5dBm = 78,// 19.5dBm
    WIFI_POWER_19dBm = 76,// 19dBm
    WIFI_POWER_18_5dBm = 74,// 18.5dBm
    WIFI_POWER_17dBm = 68,// 17dBm
    WIFI_POWER_15dBm = 60,// 15dBm
    WIFI_POWER_13dBm = 52,// 13dBm
    WIFI_POWER_11dBm = 44,// 11dBm
    WIFI_POWER_8_5dBm = 34,// 8.5dBm
    WIFI_POWER_7dBm = 28,// 7dBm
    WIFI_POWER_5dBm = 20,// 5dBm
    WIFI_POWER_2dBm = 8,// 2dBm
    WIFI_POWER_MINUS_1dBm = -4// -1dBm
*/
			wifi_power_t APower;
			if( TransmitPower() > 19 )
				APower = WIFI_POWER_19_5dBm;

			else if( TransmitPower() > 18.5 )
				APower = WIFI_POWER_19dBm;

			else if( TransmitPower() >  17 )
				APower = WIFI_POWER_18_5dBm;

			else if( TransmitPower() > 15 )
				APower = WIFI_POWER_17dBm;

			else if( TransmitPower() > 13 )
				APower = WIFI_POWER_15dBm;

			else if( TransmitPower() > 11 )
				APower = WIFI_POWER_13dBm;

			else if( TransmitPower() > 8.5 )
				APower = WIFI_POWER_11dBm;

			else if( TransmitPower() > 7 )
				APower = WIFI_POWER_8_5dBm;

			else if( TransmitPower() > 5 )
				APower = WIFI_POWER_7dBm;

			else if( TransmitPower() > 2 )
				APower = WIFI_POWER_5dBm;

			else if( TransmitPower() > -1 )
				APower = WIFI_POWER_2dBm;

			else
				APower = WIFI_POWER_MINUS_1dBm;

			WiFi.setTxPower( APower );
		}

		void UpdateLowPower()
		{
#ifndef SEEEDUINO_WIO_TERMINAL
			if( ! LowPower().Enabled().GetValue() )
				esp_wifi_set_ps( WIFI_PS_NONE );

			else if( LowPower().MaxSaving() )
				esp_wifi_set_ps( WIFI_PS_MAX_MODEM );

            else
				esp_wifi_set_ps( WIFI_PS_MIN_MODEM );

#endif
		}

		inline void UpdateAutoReconnect()
		{
			WiFi.setAutoReconnect( AutoReconnect() );
		}

	public:
		inline void SystemInit()
		{
			if( Enabled() )
				StartEthernet();

		}

	public:
		inline bool GetIsStarted()
		{
		    if( FIsStarted() )
		        return true;

		    bool AStarted = false;
		    T_Ethernet_GetIsStarted::Call( AStarted );
		    return AStarted;
		}

	public:
		void WiFi_Disconnect_Update()
		{
//			T_Sockets_StopSocket::Call();

//			for( int i = 0; i < C_COUNT_Sockets; ++i )
//				Sockets[ i ]->StopSocket();

#ifdef __ESP32_WIFI__DEBUG__
			Serial.println( "WiFi_Disconnect_Update" );
#endif
			WiFi.disconnect( false, true );
			FIsStarted() = false;
			FRemoteStarted() = false;
			FDNSUpdated() = false;
		}

		inline void WiFi_disconnect()
		{
			WiFi.disconnect();
		}

		inline void WiFi_reconnect()
		{
			WiFi.reconnect();
		}

		inline void WiFi_config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2)
		{
			WiFi.config(local_ip, gateway, subnet, dns1, dns2);
		}

		inline void WiFi_setHostname(const char * hostname)
		{
			WiFi.setHostname( hostname );
		}

		inline IPAddress WiFi_gatewayIP()
		{
			return WiFi.gatewayIP();
		}

		inline void WiFi_begin(const char* ssid, const char *passphrase, int32_t channel, const uint8_t* bssid, bool connect)
		{
			WiFi.begin( ssid, passphrase, channel, bssid, connect );
		}

#ifndef SEEEDUINO_WIO_TERMINAL
		inline void WiFi_begin_WPA2(const char* ssid, ::Mitov::TArduinoESP32WiFiModuleRemoteEnterpriseWPA2AuthenticationMethod::TArduinoESP32WiFiModuleRemoteEnterpriseWPA2AuthenticationMethod AMethod,
			const char* identity, const char* username, const char *password, const char* ca_pem, const char* client_crt, const char* client_key, int32_t channel, const uint8_t* bssid, bool connect )
		{
			WiFi.begin( ssid, wpa2_auth_method_t( AMethod ), identity, username, password, ca_pem, client_crt, client_key, channel, bssid, connect );
		}
#endif // SEEEDUINO_WIO_TERMINAL

	public:
		void UpdateAccessPoint() // Used by bindings
		{
//		    Serial.println( "UpdateAccessPoint" );
            if( ManualConnections() )
                return;

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

#ifdef __ESP32_WIFI__DEBUG__
			Serial.println( "StopEthernet" );
#endif
			WiFi.disconnect( true, true );
			FIsStarted() = false;
			FRemoteStarted() = false;
			FDNSUpdated() = false;
		}

		void StartEthernet()
		{
#ifdef __ESP32_WIFI__DEBUG__
			Serial.println( "StartEthernet" );
#endif
            if( ManualConnections() )
            {

            }

            else
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
	    		WiFi.begin();
//			WiFi.disconnect( true, true );
    			WiFi.disconnect( false, true );
//WiFi.mode( WIFI_OFF );
//delay( 3000 );
//WiFi.mode( WIFI_STA );
//WiFi.mode( WIFI_AP_STA );
//			WiFi.begin();
//				esp_wifi_set_ps( WIFI_PS_NONE );
//delay( 3000 );
//return;
//			esp_wifi_disconnect();

		    	WiFi.setAutoReconnect( AutoReconnect().GetValue() );

//	Serial.println( "TEST4" );
	    		if( HostName().GetValue() != "" )
    			{
				    WiFi.setHostname( (char *)HostName().c_str() );
//				    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, (char *)HostName.c_str() );
//				    WiFi.setHostname( "robot" );
//				    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_ETH, "robot" );
//				    espconn_mdns_set_hostname( (char *) HostName.c_str() );
			    }
#ifndef VISUINO_ESP32_C3
		    	WiFi.mode( WIFI_OFF );
#ifdef __ESP32_WIFI__DEBUG__
	    		Serial.println( "WIFI_OFF" );
#endif
#endif // VISUINO_ESP32_C3

                if( ( AccessPoint().Enabled() || ( ! AccessPoint().Enabled().GetIsConstant() )) &&
                    ( ( AccessPoint().SSID().GetValue() != "" ) || ( ! AccessPoint().SSID().GetIsConstant() ))
                  )
                {
                    if( C_ESPNow || C_COUNT_AccessPoints )
                    {
        	        	WiFi.mode( WIFI_AP_STA );
#ifdef __ESP32_WIFI__DEBUG__
    		    	    Serial.println( "WIFI_AP_STA" );
#endif
            	    }

                    else
            	    {
        	            WiFi.mode( WIFI_AP );
#ifdef __ESP32_WIFI__DEBUG__
    	    		    Serial.println( "WIFI_AP" );
#endif
            	    }

                }

                else if( C_COUNT_AccessPoints > 0 )
                {
                	WiFi.mode( WIFI_STA );
#ifdef __ESP32_WIFI__DEBUG__
    		    	    Serial.println( "WIFI_STA" );
#endif
                }

                else
                {
                    if( C_ESPNow || C_COUNT_Scan_Elements )
                    {
                        WiFi.mode( WIFI_STA );
#ifdef __ESP32_WIFI__DEBUG__
    	    		    Serial.println( "WIFI_STA" );
#endif
                    }

                    else
                    {
        	    	    WiFi.mode( WIFI_OFF );
#ifdef __ESP32_WIFI__DEBUG__
    	    		    Serial.println( "WIFI_OFF" );
#endif
            		}
                }

//WiFi.mode( WIFI_STA );
//return;
//	Serial.println( "TEST5" );
			    if( Enabled() )
				    AccessPoint().TryStart();

			}

			if( T_BSSIDOutputPin::GetPinIsConnected() )
				T_BSSIDOutputPin::SetPinValue( WiFi.BSSIDstr().c_str() );

			if( T_GatewayIPOutputPin::GetPinIsConnected() )
				T_GatewayIPOutputPin::SetPinValue( IPAdressToString( WiFi.gatewayIP() ).c_str() );

			if( T_SubnetMaskIPOutputPin::GetPinIsConnected() )
				T_SubnetMaskIPOutputPin::SetPinValue( IPAdressToString( WiFi.subnetMask() ).c_str() );

		}

	public:
		inline void SystemStart()
		{
			FLastReconnectTime = millis() - 30000;
			UpdateLowPower();
			Update_TransmitPower();
			T_RemoteConnectedOutputPin::SetPinValue( WiFi.isConnected(), false );
			if( T_MACOutputPin::GetPinIsConnected() )
				T_MACOutputPin::SetPinValue( WiFi.macAddress().c_str() );

		}

		inline void SystemLoopBegin() // Called only for RemoteConnectedOutputPin.IsConnected
		{
			int ARes = WiFi.status();

//			Serial.println( "TEST1......" );
//			Serial.print( "STATUS: " );
//			Serial.println( ARes );

			bool AIsEnabled = false; //( C_COUNT_AccessPoints == 0 );

			if( ! WiFi.isConnected() )
			{
				if( FRemoteStarted() )
					FLastReconnectTime = millis();

#ifdef __ESP32_WIFI__DEBUG__
//				Serial.println( "! WiFi.isConnected()" );
#endif
//				Serial.println( "! WiFi.isConnected()" );
//				FIsStarted() = false;
				FRemoteStarted() = false;
//				FDNSUpdated() = false;
			}

			if( ( C_COUNT_AccessPoints > 0 ) && ( ! ManualConnections().GetValue() ) )
//				if( ! FRemoteStarted() )
				{
//#ifdef __ESP32_WIFI__DEBUG__
//					Serial.print( "TRY CONNECT: " );
//					Serial.println( ARes );
//#endif // __ESP32_WIFI__DEBUG__

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
#ifdef __ESP32_WIFI__DEBUG__
							Serial.println( "Timeout..." );
							Serial.print( "STATUS: " );
							Serial.println( ARes );
							Serial.println( FAccessPointIndex().GetValue() );
#endif // __ESP32_WIFI__DEBUG__
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
				if( ( ! FRemoteStarted() ) || ManualConnections().GetValue() )
				{
					if( ! FDNSUpdated() )
					{
#ifdef __ESP32_WIFI__DEBUG__
						Serial.println( "DNS Updated" );
#endif // __ESP32_WIFI__DEBUG__
						FDNSUpdated() = true;
//							bool AIsEnabled = false;
						T_AccessPoints_AssignDNS::Call( FAccessPointIndex().GetValue() - 1 );

//						Serial.println( "DNS Updated 1" );
						FLastReconnectTime = millis();
						return;
					}

					else
					{
#ifdef __ESP32_WIFI__DEBUG__
						Serial.println( "FRemoteStarted() = true" );
#endif // __ESP32_WIFI__DEBUG__
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
						T_BSSIDOutputPin::SetPinValue( WiFi.BSSIDstr().c_str() );

					if( T_GatewayIPOutputPin::GetPinIsConnected() )
						T_GatewayIPOutputPin::SetPinValue( IPAdressToString( WiFi.gatewayIP() ).c_str() );

					if( T_SubnetMaskIPOutputPin::GetPinIsConnected() )
						T_SubnetMaskIPOutputPin::SetPinValue( IPAdressToString( WiFi.subnetMask() ).c_str() );

				}

//			for( int i = 0; i < C_COUNT_Sockets; ++i )
//				Sockets[ i ]->TryStartSocket();

//			WiFi.setHostname( (char *)HostName.c_str() );
		}

/*
	void DoCheckSignalStrength( void *_Data )
		{
			if( FIsStarted )
				if( SignalStrengthOutputPin.IsConnected() )
				{
					int32_t ASetrength = WiFi.RSSI();
					SignalStrengthOutputPin.Notify( &ASetrength );
				}

		}
*/
	public:
		inline ESP32WiFiModule()
		{
			FAccessPointIndex() = C_COUNT_AccessPoints;
//			FIsConnected() = false;
			FIsStarted() = false;
			FRemoteStarted() = false;
			FDNSUpdated() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled
	> class ESP32ModuleReconnectOperation :
		public T_Enabled
	{
	public:
		_V_PROP_( Enabled )

	public:
		inline void ClockInputPin_o_Receive( void *_Data )
		{
			if( Enabled() )
				WiFi.reconnect();

		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
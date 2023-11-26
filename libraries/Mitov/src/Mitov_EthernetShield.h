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
#include <SPI.h>

#ifdef VISUINO_M_DUINO_ETHERNET_58
	#include <Ethernet2.h>
	#include <Dns.h>
#else

	#ifdef __SEED_ETHERNET_SHIELD_2_0__
	  #include <EthernetV2_0.h>
	  #include <DnsV2_0.h>
	#else
		#ifdef __ETHERNET_SHIELD_2_0__
			#include <Ethernet2.h>
		#else
			#include <Ethernet.h>
		#endif

		#include <Dns.h>
	#endif
#endif // VISUINO_M_DUINO_ETHERNET_58

#include <Mitov_BasicEthernet.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#define Console UNGUARDED DEBUG PRINT!!!
#endif

// #define __ETHERNET_SHIELD__DEBUG__

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
//		uint32_t C_SOCKETS_COUNT,
        typename T_AddressOutputPin,
        typename T_DNSOutputPin,
		typename T_Enabled,
		typename T_FIsStarted,
		typename T_GatewayIPOutputPin,
		typename T_IPAddress,
		typename T_MacAddress,
		typename T_Sockets_StopSocket,
		typename T_SubnetMaskIPOutputPin,
		typename T_UseDHCP
	> class EthernetShield :
        public T_AddressOutputPin,
        public T_DNSOutputPin,
		public T_Enabled,
		public T_FIsStarted,
		public T_GatewayIPOutputPin,
		public T_IPAddress,
		public T_MacAddress,
		public T_Sockets_StopSocket,
		public T_SubnetMaskIPOutputPin,
		public T_UseDHCP
	{
	public:
        _V_PIN_( AddressOutputPin )
        _V_PIN_( DNSOutputPin )
		_V_PIN_( GatewayIPOutputPin )
		_V_PIN_( SubnetMaskIPOutputPin )

	public:
		_V_PROP_( MacAddress )

	public:
		_V_PROP_( IPAddress )
		_V_PROP_( Enabled )
		_V_PROP_( UseDHCP )

	protected:
		_V_PROP_( FIsStarted )

	public:
		inline bool GetIsStarted() { return FIsStarted(); }

	protected:
//#ifdef VISUINO_M_DUINO_ETHERNET_58
//		virtual void StopEthernet()
//		{
//			inherited::StopEthernet();
//		}
//#endif
//		virtual void StartEthernet()
		void StartEthernet()
		{
#ifdef VISUINO_SENS_OPER_ETHERNET
            Ethernet.init( 26 );  // ESP32 with Adafruit Featherwing Ethernet
#endif

#ifdef VISUINO_M_DUINO_ETHERNET_58
			pinMode( 53, OUTPUT); // SPI_ETHERNET
			digitalWrite( 53,LOW);
#endif
//			int AIndex = ((int)Parity) * 2 * 4 + ( StopBits - 1 ) + ( DataBits - 5);
//			C_SERIAL.begin( Speed );

#ifdef __ETHERNET_SHIELD__DEBUG__
			Serial.println( "StartEthernet" );
#endif
			FIsStarted() = true;
			uint8_t AMacAddress[ 6 ];
			MacAddress().GetMacAddress( AMacAddress );
			if( ! IPAddress().Enabled() )
				Ethernet.begin( AMacAddress );

			else
			{
				if( UseDHCP() )
					if( Ethernet.begin( AMacAddress ))
						return;

				if( ! IPAddress().DNS().Enabled() )
				{
/*
					Serial.println( "StartEthernet IP" );
					Serial.print( AMacAddress[ 0 ] );
					Serial.print( AMacAddress[ 1 ] );
					Serial.print( AMacAddress[ 2 ] );
					Serial.print( AMacAddress[ 3 ] );
					Serial.println();
					IPAddress.IP.printTo( Serial );
					Serial.println();
*/
					Ethernet.begin( AMacAddress, GetIPAddressValue( IPAddress().IP() ) );
				}

				else
				{
					if( ! IPAddress().DNS().Gateway().Enabled() )
						Ethernet.begin( AMacAddress, GetIPAddressValue( IPAddress().IP() ), GetIPAddressValue( IPAddress().DNS().IP() ) );

					else
					{
						if( ! IPAddress().DNS().Gateway().Subnet().Enabled() )
							Ethernet.begin( AMacAddress, GetIPAddressValue( IPAddress().IP() ), GetIPAddressValue( IPAddress().DNS().IP() ), GetIPAddressValue( IPAddress().DNS().Gateway().IP() ) );

						else
							Ethernet.begin( AMacAddress, GetIPAddressValue( IPAddress().IP() ), GetIPAddressValue( IPAddress().DNS().IP() ), GetIPAddressValue( IPAddress().DNS().Gateway().IP() ), GetIPAddressValue( IPAddress().DNS().Gateway().Subnet().IP() ) );

					}
				}
			}

//			T_Sockets_TryStartSocket::Call();
//			for( int i = 0; i < C_SOCKETS_COUNT; ++i )
//				Sockets[ i ]->TryStartSocket();

//delay( 6000 );
//Serial.println( "TEST1" );

            if( T_GatewayIPOutputPin::GetPinIsConnected() )
                T_GatewayIPOutputPin::SetPinValue( IPAdressToString( Ethernet.gatewayIP() ) );

            if( T_DNSOutputPin::GetPinIsConnected() )
                T_DNSOutputPin::SetPinValue( IPAdressToString( Ethernet.dnsServerIP() ) );

            if( T_SubnetMaskIPOutputPin::GetPinIsConnected() )
                T_SubnetMaskIPOutputPin::SetPinValue( IPAdressToString( Ethernet.subnetMask() ) );

            if( T_AddressOutputPin::GetPinIsConnected() )
                T_AddressOutputPin::SetPinValue( IPAdressToString( Ethernet.localIP() ) );

		}

	public:
		void UpdateEnabled()
		{
			if( Enabled() )
				StartEthernet();

			else
				StopEthernet();

		}

	public:
		inline void StopEthernet()
		{
#ifdef __ETHERNET_SHIELD__DEBUG__
			Serial.println( "StopEthernet" );
#endif
			T_Sockets_StopSocket::Call();
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
			DNSClient ADNSClient;

			ADNSClient.begin( Ethernet.dnsServerIP() );

			bool AResult = ( ADNSClient.getHostByName( AHostName.c_str(), AAdress ) == 1 );
			if( ! AResult )
				AAdress = INADDR_NONE;

			return AResult;
		}

	public:
		inline void SystemInit()
		{
			if( Enabled() )
				StartEthernet();

		}

	public:
		EthernetShield()
		{
			FIsStarted() = false;
		}

	};
//---------------------------------------------------------------------------
}


#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#undef Console
#endif


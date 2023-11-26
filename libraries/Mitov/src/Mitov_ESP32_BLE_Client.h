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
#include <Mitov_ESP32_BLE_Scan.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_MACAddress,
		typename T_Name,
		typename T_ServiceUUID
	> class ESP32BluetoothLERemoteDeviceID :
		public T_MACAddress,
		public T_Name,
		public T_ServiceUUID
	{
	public:
		_V_PROP_( Name )
		_V_PROP_( ServiceUUID )
		_V_PROP_( MACAddress )

	public:
//		void TryConnect( uint32_t AIndex, uint32_t ACurrentIndex )
		void TryConnect( BLEScanResults *foundDevices )
		{
			if( C_OWNER.IsConnected() || C_OWNER.IsConnecting() )
				return;

			if( Name().GetValue() == "" )
				if( ServiceUUID().GetValue() == "" )
					if( MACAddress().IsZero() )
						return;

			int ACount = foundDevices->getCount();
			for( int i = 0; i < ACount; ++i )
			{
				BLEAdvertisedDevice ADevice = foundDevices->getDevice( i );
//				Serial.println( ADevice.getName().c_str() );
				if( ( Name().GetValue() == "" ) || ( Name().GetValue() == ADevice.getName().c_str() ))
					if( ServiceUUID().GetValue() == "" || ServiceUUID().GetValue() == ADevice.getServiceUUID().toString().c_str() )
					{
						uint8_t AMacAddress[ 6 ];
						MACAddress().GetMacAddress( AMacAddress );

						if( MACAddress().IsZero() || ( ! memcmp( ADevice.getAddress().getNative(), AMacAddress, 6 )))
						{
							C_OWNER.Connect( ADevice );
							return;
						}
					}

			}

//			FClient->connect
//			if( ( ! Enabled ) || ( SSID == "" ))
//				return;

//			AIsEnabled = true;

//			if( AIndex != ACurrentIndex )
//				return;

		}

	};
//---------------------------------------------------------------------------
	template <
		uint32_t C_DEVICES_COUNT,
		typename T_Client_TryStart,
		typename T_DeviceIDs_TryConnect,
		typename T_DeviceNameOutputPin,
		typename T_FScanRequest,
		typename T_INSTANCE_ScanCompleteCB,
		typename T_MACAddressOutputPin
	> class ESP32BluetoothLERemoteDevice :
		public BLEClientCallbacks,
		public T_DeviceIDs_TryConnect,
		public T_DeviceNameOutputPin,
		public T_FScanRequest,
		public T_MACAddressOutputPin
	{
	public:
		_V_PIN_( DeviceNameOutputPin )
		_V_PIN_( MACAddressOutputPin )

	public:
		_V_PROP_( FScanRequest )

	public:
		BLEClient *FClient;

	protected:
//		static	void *FInstance;

		BLEAdvertisedDevice *FConnectDevice;

	public:
		virtual void onConnect(BLEClient *pClient) override
		{
		}

		virtual void onDisconnect( BLEClient *pClient ) override
		{
			bool AResult = true;
			T_Client_TryStart::Call( false, AResult );

			FScanRequest() = false;
//			delete FClient;
//			FClient = BLEDevice::createClient();
//			FClient->disconnect();
		}

	protected:
		static void SharedScanCompleteCB( BLEScanResults foundDevices )
		{
			ESP32ModuleScanBLE::InScanning = false;
			T_INSTANCE_ScanCompleteCB::Call( &foundDevices );
//			((ESP32BluetoothLERemoteDevice *)FInstance )->ScanCompleteCB( &foundDevices );
		}

	public:
		void ScanCompleteCB( BLEScanResults *foundDevices )
		{
			T_DeviceIDs_TryConnect::Call( foundDevices ); //, AIsEnabled );

			FScanRequest() = false;
		}

	protected:
		void TryScan()
		{
			if( ESP32ModuleScanBLE::InScanning )
				return;

//			Serial.println( "SCAN" );
			ESP32ModuleScanBLE::InScanning = true;
			FScanRequest() = true;
//			FInstance = this;
			BLEScan* AScan = BLEDevice::getScan();
			AScan->setActiveScan( true );
			AScan->start( 10, SharedScanCompleteCB, false );
		}

	public:
		inline bool IsConnecting()
		{
			return ( FConnectDevice != nullptr );
		}

		inline bool IsConnected()
		{
			return FClient->isConnected();
		}

		void Connect( BLEAdvertisedDevice &ADevice )
		{
			FConnectDevice = new BLEAdvertisedDevice( ADevice );
		}

	public:
		inline void SystemInit()
		{
			FClient = BLEDevice::createClient();
			if( C_DEVICES_COUNT > 0 )
				TryScan();

		}

		inline void SystemLoopBegin()
		{
			if( FConnectDevice )
			{
//				Serial.println( "CONNECTING" );
				if( FClient->connect( FConnectDevice ))
				{
				    FScanRequest() = true;
//					FInstance = this;
					T_DeviceNameOutputPin::SetPinValue( FConnectDevice->getName().c_str() );
					T_MACAddressOutputPin::SetPinValue( FConnectDevice->getAddress().toString().c_str() );
//					Serial.println( "TEST1" );
					FClient->setClientCallbacks( this );

					bool AResult = true;
					T_Client_TryStart::Call( true, AResult );
				}

//				Serial.println( "CONNECTING END" );
				delete FConnectDevice;
				FConnectDevice = nullptr;
			}

			else if( ! FScanRequest().GetValue() )
			{
				TryScan();
			}

//			bool AIsEnabled = false;

//			if( C_DEVICES_COUNT > 0 )
//				if( DeviceIDs_CallChain )
//				{
//					DeviceIDs_CallChain( FDeviceIndex ); //, AIsEnabled );
//				}

		}

	};
//---------------------------------------------------------------------------
/*
	template <
			uint32_t C_DEVICES_COUNT,
			typename T_Client_TryStart,
			typename T_DeviceIDs_TryConnect,
			typename T_DeviceNameOutputPin,
			typename T_MACAddressOutputPin
		> void * ESP32BluetoothLERemoteDevice<
			C_DEVICES_COUNT,
			T_Client_TryStart,
			T_DeviceIDs_TryConnect,
			T_DeviceNameOutputPin,
			T_MACAddressOutputPin
		>::FInstance;
*/
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Enabled,
		typename T_Serial_TryStart
	> class ESP32BluetoothLEClient :
		public T_Enabled
	{
	public:
		_V_PROP_( Enabled )

	public:
		inline void TryStart( bool AValue, bool &AResult )
		{
			T_Serial_TryStart::Call( AValue, AResult );
		}

	public:
		inline bool IsConnected()
		{
			return C_OWNER.IsConnected();
		}

	public:
		inline BLEClient *GetClient()
		{
			return C_OWNER.FClient;
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
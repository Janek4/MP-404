////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2023 Mitov Software. All Rights Reserved.        //
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
    class ArduinoNeuroShieldNeurons
    {
	protected:
		static const uint32_t NEURONSIZE = 256;

		static const uint8_t MOD_NM = 0x01;

		static const uint8_t NM_NCR			= 0x00;
		static const uint8_t NM_COMP		= 0x01;
		static const uint8_t NM_LCOMP		= 0x02;
		static const uint8_t NM_DIST		= 0x03;
		static const uint8_t NM_INDEXCOMP	= 0x03;
		static const uint8_t NM_CAT			= 0x04;
		static const uint8_t NM_AIF			= 0x05;
		static const uint8_t NM_MINIF		= 0x06;
		static const uint8_t NM_MAXIF		= 0x07;
		static const uint8_t NM_TESTCOMP	= 0x08;
		static const uint8_t NM_TESTCAT		= 0x09;
		static const uint8_t NM_NID			= 0x0A;;
		static const uint8_t NM_GCR			= 0x0B;
		static const uint8_t NM_RESETCHAIN	= 0x0C;
		static const uint8_t NM_NSR			= 0x0D;
		static const uint8_t NM_NCOUNT		= 0x0F;
		static const uint8_t NM_FORGET		= 0x0F;

		static const int SPIspeed			= 1000000; //2000000;

		static const uint8_t SPI_NMSelect	= 7;

	protected:
		static int16_t read_internal( uint8_t mod, uint8_t reg )
		{
			Digital.Write( SPI_NMSelect, false );

			SPI.beginTransaction(SPISettings(SPIspeed, MSBFIRST, SPI_MODE0));
			SPI.transfer(1);  // Dummy for ID
			SPI.transfer(mod);
			SPI.transfer(0);
			SPI.transfer(0);
			SPI.transfer(reg);
			SPI.transfer(0); // length[23-16]
			SPI.transfer(0); // length [15-8]
			SPI.transfer(1); // length [7-0]
			int16_t data = SPI.transfer(0); // Send 0 to push upper data out
			data = (data << 8) + SPI.transfer(0); // Send 0 to push lower data out
			SPI.endTransaction();

			Digital.Write( SPI_NMSelect, true );
			return(data);
		}

		static void write_internal( uint8_t mod, uint8_t reg, int16_t data )
		{
			Digital.Write( SPI_NMSelect, false );

			SPI.beginTransaction(SPISettings(SPIspeed, MSBFIRST, SPI_MODE0));
			SPI.transfer(1);  // Dummy for ID
			SPI.transfer(mod + 0x80); // module and write flag
			SPI.transfer(0);
			SPI.transfer(0);
			SPI.transfer(reg);
			SPI.transfer(0); // length[23-16]
			SPI.transfer(0); // length[15-8]
			SPI.transfer(1); // length[7-0]
			SPI.transfer( uint8_t( data >> 8 )); // upper data
			SPI.transfer( uint8_t( data & 0x00FF )); // lower data
			SPI.endTransaction();

			Digital.Write( SPI_NMSelect, true );
		}

	protected:
		static inline int16_t read( uint8_t reg )
		{
			return read_internal( MOD_NM, reg );
		}

		static inline void write( uint8_t reg, int16_t data )
		{
			write_internal( MOD_NM, reg, data );
		}

	public:
		static constexpr inline uint32_t GetNumberFeatures()
		{
			return NEURONSIZE;
		}

	public:
		static inline void ResetChain()
		{
			write( NM_RESETCHAIN, 0 );
		}

		static inline int16_t Read_NetworkStatusRegister()
		{
			return read( NM_NSR );
		}

		static inline void Write_NetworkStatusRegister( int16_t AValue )
		{
			write( NM_NSR, AValue );
		}

		static inline int16_t Read_NeuronContextRegister()
		{
			return read( NM_NCR );
		}

		static inline void Write_NeuronContextRegister( int16_t AValue )
		{
			write( NM_NCR, AValue );
		}

		static inline void Forget()
		{
			write( NM_FORGET, 0 );
		}

		static inline int16_t Read_Component()
		{
			return read( NM_COMP );
		}

		static inline void Write_Component( int16_t AValue )
		{
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.print( "Write_Component: " ); Serial.println( AValue );
#endif
			write( NM_COMP, AValue );
		}

		static inline void Write_LastComponent( int16_t AValue )
		{
#ifdef __GENERAL_VISION_NEURONS__DEBUG__
			Serial.print( "Write_LastComponent: " ); Serial.println( AValue );
#endif
			write( NM_LCOMP, AValue );
		}

		static inline int16_t Read_ActiveInfluenceField()
		{
			return read( NM_AIF );
		}

		static inline void Write_ActiveInfluenceField( int16_t AValue )
		{
			write( NM_AIF, AValue );
		}
		
		static inline int16_t Read_MinInfluenceField()
		{
			return read( NM_MINIF );
		}

		static inline void Write_MinInfluenceField( int16_t AValue )
		{
			write( NM_MINIF, AValue );
		}		

		static inline void Write_MaxInfluenceField( int16_t AValue )
		{
			write( NM_MAXIF, AValue );
		}		

		static inline int16_t Read_CountCommittedNeurons()
		{
			return read( NM_NCOUNT );
		}

		static inline int16_t Read_Category()
		{
			return read( NM_CAT );
		}

		static inline void Write_Category( int16_t AValue )
		{
			write( NM_CAT, AValue );
		}	

		static inline int16_t Read_NeuronID()
		{
			return read( NM_NID );
		}

		static inline void Write_GlobalControlRegister( int16_t AValue )
		{
			write( NM_GCR, AValue );
		}

		static inline int16_t Read_Distance()
		{
			return read( NM_DIST );
		}

		static inline void TestComponent()
		{
			write( NM_TESTCOMP, 0 );
		}

/*
		static inline void ReadNeurons( int16_t *AFeaturesData )
		{
		}
*/
	public:
		inline void SystemInit()
		{
			pinMode( SPI_NMSelect, OUTPUT );
			Digital.Write( SPI_NMSelect, true );
			delay(100);
		}


/*
	public:
		void clearNeurons()
		{
			write_internal(MOD_NM, NM_NSR, 16);
			write_internal(MOD_NM, NM_TESTCAT, 0x0001);
			write_internal(MOD_NM, NM_NSR, 0);
			for (int i=0; i< NEURONSIZE; i++)
			{
				write_internal(MOD_NM, NM_INDEXCOMP,i);
				write_internal(MOD_NM, NM_TESTCOMP,0);
			}
			write_internal(MOD_NM, NM_FORGET,0);
		}

		int countNeuronsAvailable()
		{
			write_internal(MOD_NM, NM_FORGET, 0);
			write_internal(MOD_NM, NM_NSR, 0x0010);
			write_internal(MOD_NM, NM_TESTCAT, 0x0001);
			write_internal(MOD_NM, NM_RESETCHAIN, 0);
			int read_cat=0xFFFF;	
			int navail = 0;
			while (1) 
			{
				read_cat = read_internal(MOD_NM, NM_CAT);
//				Serial.print( "read_cat: " );
//				Serial.println( read_cat );
				if (read_cat == 0xFFFF)
					break;

				navail++;
			}
			write_internal(MOD_NM, NM_NSR, 0x0000);
			write_internal(MOD_NM, NM_FORGET, 0);
			return(navail);
		}

		void MAXIF(int value)
		{
			write_internal(MOD_NM, NM_MAXIF, value);
		}
		int MAXIF()
		{
			return(read_internal(MOD_NM, NM_MAXIF));
		}

		void forget()
		{
			write_internal(MOD_NM, NM_FORGET, 0);
		}

		void forget(int Maxif)
		{
			write_internal(MOD_NM, NM_FORGET, 0);
			write_internal(MOD_NM, NM_MAXIF, Maxif);
		}

		inline void SystemStart()
		{
			Serial.println( "SystemStart" );
/ *
			write_internal(MOD_NM, NM_TESTCAT, 0x0001);
			write_internal(MOD_NM, NM_RESETCHAIN, 0);
			write_internal(MOD_NM, NM_NSR, 0x0000);
			write_internal(MOD_NM, NM_FORGET, 0);
* /
//			countNeuronsAvailable();
//			clearNeurons();

//			forget(DEF_MAXIF);
//			forget( 1000 );
//			auto maxif= MAXIF();
//			Serial.print("\nNN Maxif = "); Serial.print(maxif); 

//			Serial.println( countNeuronsAvailable() );
		}
*/
    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif

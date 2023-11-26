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
//#include <CurieNeurons.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
	namespace CurieNeuronRegisters
	{
		const uint32_t NEURONSIZE = 128;

		volatile uint8_t* PVP_REGISTER_BASE = (volatile uint8_t*)0xB0600000;
		volatile int*	const NM_NCR =      (volatile int*)(PVP_REGISTER_BASE + 0x00);
		volatile int*	const NM_COMP =     (volatile int*)(PVP_REGISTER_BASE + 0x04);
		volatile int*	const NM_LCOMP =    (volatile int*)(PVP_REGISTER_BASE + 0x08);
		volatile int*	const NM_DIST =     (volatile int*)(PVP_REGISTER_BASE + 0x0C);
		volatile int*	const NM_IDX =     	(volatile int*)(PVP_REGISTER_BASE + 0x0C);
		volatile int*	const NM_CAT =     	(volatile int*)(PVP_REGISTER_BASE + 0x10);
		volatile int*	const NM_AIF =     	(volatile int*)(PVP_REGISTER_BASE + 0x14);
		volatile int*	const NM_MINIF =    (volatile int*)(PVP_REGISTER_BASE + 0x18);
		volatile int*	const NM_MAXIF =    (volatile int*)(PVP_REGISTER_BASE + 0x1C);
		volatile int*	const NM_TESTCOMP = (volatile int*)(PVP_REGISTER_BASE + 0x20);
		volatile int*	const NM_TESTCAT =  (volatile int*)(PVP_REGISTER_BASE + 0x24);
		volatile int*	const NM_NID =  	(volatile int*)(PVP_REGISTER_BASE + 0x28);
		volatile int*	const NM_GCR =  	(volatile int*)(PVP_REGISTER_BASE + 0x2C);
		volatile int*	const NM_RSTCHAIN = (volatile int*)(PVP_REGISTER_BASE + 0x30);
		volatile int*	const NM_NSR =  	(volatile int*)(PVP_REGISTER_BASE + 0x34);
		volatile int*	const NM_FORGET =  	(volatile int*)(PVP_REGISTER_BASE + 0x3C);
		volatile int*	const NM_NCOUNT =  	(volatile int*)(PVP_REGISTER_BASE + 0x3C);
	}
//---------------------------------------------------------------------------
	class Arduino101CurieNeurons
	{
	public:
		constexpr inline uint32_t GetNumberFeatures()
		{
			return CurieNeuronRegisters::NEURONSIZE;
		}

	public:
		inline void ResetChain()
		{
			*CurieNeuronRegisters::NM_RSTCHAIN = 0;
		}

		inline int Read_NetworkStatusRegister()
		{
			return *CurieNeuronRegisters::NM_NSR;
		}

		inline void Write_NetworkStatusRegister( int AValue )
		{
			*CurieNeuronRegisters::NM_NSR = AValue;
		}

		inline int Read_NeuronContextRegister()
		{
			return *CurieNeuronRegisters::NM_NCR;
		}

		inline void Write_NeuronContextRegister( int AValue )
		{
			*CurieNeuronRegisters::NM_NCR = AValue;
		}

		inline void Forget()
		{
			*CurieNeuronRegisters::NM_FORGET = 0;
		}

		inline int Read_Component()
		{
			return *CurieNeuronRegisters::NM_COMP;
		}

		inline void Write_Component( int AValue )
		{
			*CurieNeuronRegisters::NM_COMP = AValue;
		}

		inline void Write_LastComponent( int AValue )
		{
			*CurieNeuronRegisters::NM_LCOMP = AValue;
		}

		inline int Read_ActiveInfluenceField()
		{
			return *CurieNeuronRegisters::NM_AIF;
		}

		inline void Write_ActiveInfluenceField( int AValue )
		{
			*CurieNeuronRegisters::NM_AIF = AValue;
		}
		
		inline int Read_MinInfluenceField()
		{
			return *CurieNeuronRegisters::NM_MINIF;
		}

		inline void Write_MinInfluenceField( int AValue )
		{
			*CurieNeuronRegisters::NM_MINIF = AValue;
		}		

/*
		inline int Read_MaxInfluenceField()
		{
			return *CurieNeuronRegisters::NM_MAXIF;
		}
*/
		inline void Write_MaxInfluenceField( int AValue )
		{
			*CurieNeuronRegisters::NM_MAXIF = AValue;
		}		

		inline int Read_CountCommittedNeurons()
		{
			return *CurieNeuronRegisters::NM_NCOUNT;
		}

		inline int Read_Category()
		{
			return *CurieNeuronRegisters::NM_CAT;
		}

		inline void Write_Category( int AValue )
		{
			*CurieNeuronRegisters::NM_CAT = AValue;
		}	

		inline int Read_NeuronID()
		{
			return *CurieNeuronRegisters::NM_NID;
		}

		inline void Write_GlobalControlRegister( int AValue )
		{
			*CurieNeuronRegisters::NM_GCR = AValue;
		}

		inline int Read_Distance()
		{
			return *CurieNeuronRegisters::NM_DIST;
		}
		
		inline void TestComponent()
		{
			*CurieNeuronRegisters::NM_TESTCOMP = 0;
		}

	public:
		inline void SystemInit()
		{
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
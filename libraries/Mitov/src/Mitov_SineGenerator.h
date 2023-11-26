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
#include "Mitov_BasicGenerator.h"

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	class SineAnalogGenerator
	{
	public:
		template <typename T> typename T::T_DATA CalculateValue( T *AInherited )
		{
			return sin( V_FMOD( ( AInherited->FPhase + AInherited->Phase() ), 1.0f ) * 2 * PI ) * AInherited->Amplitude() + AInherited->Offset();
		}

	};
//---------------------------------------------------------------------------
	class SineComplexGenerator
	{
	public:
		template <typename T> typename T::T_DATA CalculateValue( T *AInherited )
		{
			float ACoerricient = sin( V_FMOD( ( AInherited->FPhase + AInherited->Phase() ), 1.0f ) * 2 * PI );
		    return TComplex( ACoerricient * AInherited->Amplitude().GetValue().Real + AInherited->Offset().GetValue().Real, ACoerricient * AInherited->Amplitude().GetValue().Imaginary + AInherited->Offset().GetValue().Imaginary );
//			return sin( V_FMOD( ( AInherited->FPhase + AInherited->Phase() ), 1 ) * 2 * PI ) * AInherited->Amplitude() + AInherited->Offset();
		}

	};
//---------------------------------------------------------------------------
	class SineIntegerGenerator
	{
	protected:
		template <typename T> typename T::T_DATA CalculateValue( T *AInherited )
		{
			return sin( V_FMOD( ( AInherited->FPhase + AInherited->Phase() ), 1.0f ) * 2 * PI ) * AInherited->Amplitude() + AInherited->Offset() + 0.5;
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
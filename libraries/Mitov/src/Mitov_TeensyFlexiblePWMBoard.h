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

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_PWMResolution
	> class TeensyFlexiblePWMBoard :
		public T_PWMResolution
	{
	public:
		_V_PROP_( PWMResolution )

	public:
		inline void Update_PWMResolution()
		{
			analogWriteResolution( PWMResolution().GetValue() );
		}

	public:
		inline void SystemInit()
		{
			Update_PWMResolution();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OutputPin,
		uint8_t C_INDEX,
		typename T_PWMFrequency
	> class TeensyPWMBoardChannel :
		public T_PWMFrequency
	{
	public:
		_V_PROP_( PWMFrequency )

	public:
		inline void Update_PWMFrequency()
		{
			analogWriteFrequency( C_INDEX, PWMFrequency().GetValue() );
		}

	public:
		inline void SystemInit()
		{
			Update_PWMFrequency();
		}

	};
//---------------------------------------------------------------------------
}


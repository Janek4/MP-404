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
#include <Mitov_NeoPixel.h>

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER, 
		typename T_BackgroundColor,
		typename T_Brightness,
		typename T_Elements_Render,
		typename T_Enabled,
   		typename T_FCursorX,
   		typename T_FCursorY,
   		typename T_Mirror,
		typename T_Orientation,
		typename T_Text
	> class ArduinoBpiBitNeoPixelsModule : 
		public LedPixels2DGraphicsGroup <
				T_OWNER, 
				C_OWNER, 
				Mitov::LedPixelsStructure_SingleDirection,
				25, 
				0,
				T_BackgroundColor,
				T_Elements_Render,
                T_Enabled,
                T_FCursorX,
                T_FCursorY,
				5,
                T_Mirror,
				T_Orientation,
				Mitov::TRGBColorTransfrom, 
				T_Text,
				5
			>
	{
		typedef LedPixels2DGraphicsGroup <
				T_OWNER, 
				C_OWNER, 
				Mitov::LedPixelsStructure_SingleDirection,
				25, 
				0,
				T_BackgroundColor,
				T_Elements_Render,
                T_Enabled,
                T_FCursorX,
                T_FCursorY,
				5,
                T_Mirror,
				T_Orientation,
				Mitov::TRGBColorTransfrom, 
				T_Text,
				5
			> inherited;

	public:
		_V_PROP_( Brightness )

	public:
		inline void	UpdateEnabled()
		{
			digitalWrite( 2, ( inherited::Enabled().GetValue() ) ? HIGH : LOW );
		}

/*
	public:
		int16_t width(void) const
		{
			return 5;
		}

		int16_t height(void) const
		{
			return 5;
		}
*/
	public:
		inline void SystemStart()
		{
//			Serial.println( "TEST1" );
			pinMode( 2, OUTPUT );
			UpdateEnabled();

			inherited::SystemStart();
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
//			FPixel.setPin( C_PIN_NUMBER );

			C_OWNER.SystemInit();
			C_OWNER.IntSetBrightness( Brightness() );
		}

		inline void SystemLoopEnd()
		{
			C_OWNER.SystemLoopEnd();
		}

	public:
		inline void UpdateBrightness()
		{
			inherited::IntSetBrightness( Brightness() );
		}

/*
	public:
		inline void RefreshInputPin_o_Receive()
		{
		}
*/
/*
	public:
		ArduinoBpiBitNeoPixelsModule( float ABrightness, const unsigned char * AFont ) :
			T_PARENT( AFont )
		{
			T_PARENT::SetBrightness( ABrightness );
		}
*/
	public:
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
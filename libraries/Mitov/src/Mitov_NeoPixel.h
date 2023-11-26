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
#include <Mitov_Graphics_RGBW.h>
#include <Mitov_LedPixels.h>

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
  #include <NeoPixelBrightnessBus.h>
#else
//  #include <MitovEmbedded_Adafruit_NeoPixel/MitovEmbedded_Adafruit_NeoPixel.h>
  #include <Adafruit_NeoPixel.h>
#endif

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_IMPLEMENTATION, 
		T_IMPLEMENTATION &C_IMPLEMENTATION,
		typename T_FModified
	> class NeoPixelsBasic :
		public T_FModified
	{
	public:
		_V_PROP_( FModified )

	public:
		inline void SetPixelColor( int AIndex, TRGBWColor AColor )
		{
//			Serial.print( "SetPixelColor: " );
//			Serial.print( AIndex );
//			Serial.print( " " );
//			Serial.print( AColor.ToString() );

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			RgbColor ARgbColor( AColor.Red, AColor.Green, AColor.Blue );
			C_IMPLEMENTATION.SetPixelColor( AIndex, ARgbColor );
#else
			C_IMPLEMENTATION.setPixelColor( AIndex, AColor.Red, AColor.Green, AColor.Blue, AColor.White );
#endif
//			C_IMPLEMENTATION.show();
//			delay( 200 );
			FModified() = true;
		}

		inline TRGBWColor GetPixelColor( int AIndex )
		{
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			RgbColor ARgbColor = C_IMPLEMENTATION.GetPixelColor( AIndex );
			return TRGBWColor( ARgbColor.R, ARgbColor.G, ARgbColor.B, 0 );
#else
			return TRGBWColor( C_IMPLEMENTATION.getPixelColor( AIndex ), true );
#endif
		}

/*
		inline void SystemLoopBegin()
		{
			for( int i = 0; i < PixelGroups.size(); ++i )
				PixelGroups[ i ]->PixelsClock();

		}
*/
		inline void SystemLoopEnd()
		{
			ClockInputPin_o_Receive( nullptr );
		}

		inline void SystemInit()
		{
//			FPixel.setPin( C_PIN_NUMBER );

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			C_IMPLEMENTATION.Begin();
#else
			C_IMPLEMENTATION.begin();
#endif
		}

	protected:
		void IntSetBrightness( float AValue )
		{
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			C_IMPLEMENTATION.SetBrightness( AValue * 255 );
#else
			C_IMPLEMENTATION.setBrightness( AValue * 255 );
#endif
			FModified() = true;
		}

	public:
		void ClockInputPin_o_Receive( void * )
		{
			if( FModified() )
			{
//				Serial.println( "TEST11" );
//				Serial.println( "TEST12" );
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
				C_IMPLEMENTATION.Show();
#else
				C_IMPLEMENTATION.show();
#endif
				FModified() = false;
			}
		}

	public:
		inline NeoPixelsBasic()
		{
			FModified() = true;
		}
	};
//---------------------------------------------------------------------------
	template <
		typename T_IMPLEMENTATION, 
		T_IMPLEMENTATION &C_IMPLEMENTATION,
		typename T_Brightness,
		typename T_FModified
	> class NeoPixels : public NeoPixelsBasic <
				T_IMPLEMENTATION, 
				C_IMPLEMENTATION,
				T_FModified
			>,
		public T_Brightness
	{
		typedef NeoPixelsBasic <
				T_IMPLEMENTATION, 
				C_IMPLEMENTATION,
				T_FModified
		> inherited;

	public:
		_V_PROP_( Brightness )

	public:
		void UpdateBrightness()
		{
			inherited::IntSetBrightness( Brightness() );
		}

	public:
		inline void SystemInit()
		{
			inherited::SystemInit();
//			FPixel.setPin( C_PIN_NUMBER );

			inherited::IntSetBrightness( Brightness() );
		}

	};
//---------------------------------------------------------------------------
	template <
//		uint8_t C_Pin0,
		typename T_OWNER, T_OWNER &C_OWNER,
//		uint8_t C_Pin1
		uint8_t C_COUNT,
		uint8_t C_START
	> class GroveNeoPixelLEDs
	{
	public:
		inline void SetPixelColor( int AIndex, TRGBWColor AColor )
		{
//			Serial.println( AIndex );
//			Serial.println( AColor.ToString() );
			if( AIndex < C_COUNT )				
				C_OWNER.SetPixelColor( C_START + AIndex, AColor );

		}

		inline TRGBWColor GetPixelColor( int AIndex )
		{
			if( AIndex < C_COUNT )				
				return C_OWNER.GetPixelColor( C_START + AIndex );

			return TRGBWColor();
		}


	public:
		inline void SystemInit()
		{
			C_OWNER.SystemInit();
		}

		inline void SystemLoopEnd()
		{
			C_OWNER.SystemLoopEnd();
		}

};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_MATRIX_MAP,
		uint16_t C_COUNT_PIXELS, 
		typename T_BackgroundColor,
		typename T_Brightness,
		typename T_Elements_Render,
		typename T_Enabled,
        typename T_FCursorX,
        typename T_FCursorY,
		int HEIGHT, 
		typename T_Mirror,
//		typename T_MATRIX_MAP, 
		typename T_Orientation,
		typename T_COLOR_TRANSFORM, 
		typename T_Text,
		int WIDTH
	> class ArduinoNeoPixels2DGraphicsModule : 
		public LedPixels2DGraphicsGroup <
				T_OWNER, 
				C_OWNER, 
                T_MATRIX_MAP,
				C_COUNT_PIXELS, 
				0,
				T_BackgroundColor,
				T_Elements_Render,
                T_Enabled,
                T_FCursorX,
                T_FCursorY,
				HEIGHT,
                T_Mirror,
//				T_MATRIX_MAP,
				T_Orientation,
				T_COLOR_TRANSFORM, 
				T_Text,
				WIDTH
			>,
		public T_Enabled
	{
		typedef LedPixels2DGraphicsGroup <
				T_OWNER, 
				C_OWNER, 
                T_MATRIX_MAP,
				C_COUNT_PIXELS, 
				0,
				T_BackgroundColor,
				T_Elements_Render,
                T_Enabled,
                T_FCursorX,
                T_FCursorY,
				HEIGHT,
                T_Mirror,
//				T_MATRIX_MAP,
				T_Orientation,
				T_COLOR_TRANSFORM, 
				T_Text,
				WIDTH
			> inherited;

	public:
		_V_PROP_( Brightness )

	public:
		inline void UpdateBrightness()
		{
			C_OWNER.IntSetBrightness( Brightness() );
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
		using inherited::inherited;

	};
//---------------------------------------------------------------------------
/*
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
	class Mitov_NeoGbrFeature : public Neo3ByteElementsNoSettings
	{
	public:
		static void applyPixelColor(uint8_t* pPixels, uint16_t indexPixel, ColorObject color)
		{
			uint8_t* p = getPixelAddress(pPixels, indexPixel);

			*p++ = color.G;
			*p++ = color.B;
			*p = color.R;
		}

		static ColorObject retrievePixelColor(const uint8_t* pPixels, uint16_t indexPixel)
		{
			ColorObject color;
			const uint8_t* p = getPixelAddress(pPixels, indexPixel);

			color.G = *p++;
			color.B = *p++;
			color.R = *p;

			return color;
		}
    
		static ColorObject retrievePixelColor_P(PGM_VOID_P pPixels, uint16_t indexPixel)
		{
			ColorObject color;
			const uint8_t* p = getPixelAddress((const uint8_t*)pPixels, indexPixel);

			color.G = pgm_read_byte(p++);
			color.B = pgm_read_byte(p++);
			color.R = pgm_read_byte(p);

			return color;
		}
    
	};
//---------------------------------------------------------------------------
	class Mitov_NeoGrbFeature : public Neo3ByteElementsNoSettings
	{
	public:
		static void applyPixelColor(uint8_t* pPixels, uint16_t indexPixel, ColorObject color)
		{
			uint8_t* p = getPixelAddress(pPixels, indexPixel);

			*p++ = color.G;
			*p++ = color.R;
			*p = color.B;
		}

		static ColorObject retrievePixelColor(const uint8_t* pPixels, uint16_t indexPixel)
		{
			ColorObject color;
			const uint8_t* p = getPixelAddress(pPixels, indexPixel);

			color.G = *p++;
			color.R = *p++;
			color.B = *p;

			return color;
		}
    
		static ColorObject retrievePixelColor_P(PGM_VOID_P pPixels, uint16_t indexPixel)
		{
			ColorObject color;
			const uint8_t* p = getPixelAddress((const uint8_t*)pPixels, indexPixel);

			color.G = pgm_read_byte(p++);
			color.R = pgm_read_byte(p++);
			color.B = pgm_read_byte(p);

			return color;
		}
    
	};
#endif // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
*/
//---------------------------------------------------------------------------
	class NeoPixelSingleLEDModule_GBRAdapter
	{
	public:
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
//		NeoPixelBrightnessBus<Mitov_NeoGbrFeature, Neo800KbpsMethod>	FPixel;
		NeoPixelBrightnessBus<NeoGbrFeature, Neo800KbpsMethod>	FPixel;
#else // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
		Adafruit_NeoPixel	FPixel;
#endif // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__

	public:
		NeoPixelSingleLEDModule_GBRAdapter( uint16_t APinNumber ) :
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			FPixel( 1, APinNumber )
#else // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			FPixel( 1, APinNumber, NEO_GBR + NEO_KHZ800 )
#endif // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
		{
		}

	};
//---------------------------------------------------------------------------
	class NeoPixelSingleLEDModule_GRBAdapter
	{
	public:
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
//		NeoPixelBrightnessBus<Mitov_NeoGrbFeature, Neo800KbpsMethod>	FPixel;
		NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod>	FPixel;
#else // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
		Adafruit_NeoPixel	FPixel;
#endif // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__

	public:
		NeoPixelSingleLEDModule_GRBAdapter( uint16_t APinNumber ) :
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			FPixel( 1, APinNumber )
#else // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			FPixel( 1, APinNumber, NEO_GRB + NEO_KHZ800 )
#endif // __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
		{
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Brightness,
		uint8_t C_PIN,
		typename T_Enabled,
		typename T_InitialValue,
		typename T_TYPE
	> class NeoPixelSingleLEDModule :
		public T_TYPE,
		public T_Brightness,
		public T_Enabled,
		public T_InitialValue
	{
		typedef T_TYPE inherited;

	public:
		_V_PROP_( Enabled )
		_V_PROP_( Brightness )
		_V_PROP_( InitialValue )

	public:
		inline void SetPixelColor( TColor AColor )
		{
//			Serial.println( AColor.ToString() );
			if( Enabled() )
				IntSetPixelColor( 0, AColor.Red, AColor.Green, AColor.Blue );

			else
				IntSetPixelColor( 0, 0, 0, 0 );

//			inherited::FPixel.setPixelColor( AIndex, 255, 0, 0 );
		}

	public:
		inline void UpdateOutputs()
		{
			SetPixelColor( InitialValue() );
		}

		void UpdateBrightness()
		{
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			inherited::FPixel.SetBrightness( Brightness().GetValue() * 255 + 0.5 );
#else
			inherited::FPixel.setBrightness( Brightness().GetValue() * 255 + 0.5 );
#endif
		}

	protected:
		inline void IntSetPixelColor( uint8_t AWhite, uint8_t ARed, uint8_t AGreen, uint8_t ABlue )
		{
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			RgbColor ARgbColor( ARed, AGreen, ABlue );
			inherited::FPixel.SetPixelColor( 0, ARgbColor );
			inherited::FPixel.Show();
#else
			inherited::FPixel.setPixelColor( 0, ARed, AGreen, ABlue );
			inherited::FPixel.show();
#endif
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			TColor AColor = *(TColor *)_Data;
			if( AColor == InitialValue().GetReference() )
				return;

			InitialValue() = AColor;
			UpdateOutputs();
		}

	public:
		inline void SystemStart()
		{
//			inherited::FPixel.setPin( C_PIN );

#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			inherited::FPixel.Begin();
#else
			inherited::FPixel.begin();
#endif
//			inherited::FPixel.updateType(NEO_GRB + NEO_KHZ800);
//			inherited::FPixel.updateLength(1);
			UpdateBrightness();
			UpdateOutputs();
#ifdef __VISUINO_USE_NEO_PIXELS_BUS_LIBRARY__
			inherited::FPixel.Show();
#else
			inherited::FPixel.show();
#endif
		}

	public:
		inline NeoPixelSingleLEDModule() :
			inherited( C_PIN )
		{
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Brightness,
		uint8_t C_PIN,
		uint8_t C_POWER_PIN,
		typename T_Enabled,
		typename T_InitialValue,
		typename T_TYPE
	> class NeoPixelSinglePowerControlLEDModule :
		public NeoPixelSingleLEDModule <
				T_Brightness,
				C_PIN,
				T_Enabled,
				T_InitialValue,
				T_TYPE
			>
	{
		typedef NeoPixelSingleLEDModule <
				T_Brightness,
				C_PIN,
				T_Enabled,
				T_InitialValue,
				T_TYPE
			> inherited;

	public:
		inline void UpdateEnabled()
		{
			if( inherited::Enabled() )
				digitalWrite( C_POWER_PIN, HIGH );

			inherited::UpdateOutputs();

			if( ! inherited::Enabled().GetValue() )
				digitalWrite( C_POWER_PIN, LOW );

		}

		inline void SystemStart()
		{
			pinMode( C_POWER_PIN, OUTPUT );
			digitalWrite( C_POWER_PIN, inherited::Enabled() ? HIGH : LOW );

			inherited::SystemStart();
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif


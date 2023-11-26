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

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		uint16_t C_COUNT_PIXELS,
		uint16_t C_START_PIXEL,
		typename T_Enabled,
		typename T_InitialColor
	> class LedPixelsGroup :
		public T_Enabled,
		public T_InitialColor
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialColor )

	protected:
		TRGBWColor	FValues[ C_COUNT_PIXELS ];

	public:
		inline void SystemStart()
		{
			if( ! Enabled().GetValue() )
				return;

			for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
			{
				FValues[ i ] = TRGBWColor( InitialColor().GetValue());
				C_OWNER.SetPixelColor( C_START_PIXEL + i, TRGBWColor( InitialColor().GetValue()) );
			}
		}

	public:
		void InputPins_o_Receive( int AIndex, void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			TRGBWColor AValue = *(TRGBWColor *)_Data;
			if( FValues[ AIndex ] == AValue )
				return;

			FValues[ AIndex ] = AValue;
			C_OWNER.SetPixelColor( AIndex, AValue );
		}
	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		uint16_t C_COUNT_PIXELS,
		uint16_t C_START_PIXEL,
		typename T_Enabled,
		typename T_InitialColor,
//		uint16_t C_REPEAT_COUNT,
		uint16_t C_SUB_PIXEL_COUNT
	> class LedPixelsRepeatGroup : 
		public T_Enabled,
		public T_InitialColor
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialColor )

	public:
		inline void SystemStart()
		{
			if( Enabled() )
				for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
					C_OWNER.SetPixelColor( C_START_PIXEL + i, InitialColor().GetValue() );
		}

	public:
		void SetPixelColor( int AIndex, TRGBWColor AColor )
		{
			if( ! Enabled() )
				return;

			if( ! C_SUB_PIXEL_COUNT )
				return;

//			for( int i = 0; i < C_REPEAT_COUNT; ++i )
			for( int i = 0; i < ( C_COUNT_PIXELS + C_SUB_PIXEL_COUNT - 1 ) / C_SUB_PIXEL_COUNT; ++i )
				C_OWNER.SetPixelColor( C_START_PIXEL + AIndex + i * C_SUB_PIXEL_COUNT, AColor );
		}

		TRGBWColor GetPixelColor( int AIndex )
		{
			return C_OWNER.GetPixelColor( C_START_PIXEL + AIndex );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		uint16_t C_COUNT_PIXELS,
		uint16_t C_START_PIXEL,
		typename T_Enabled,
		typename T_InitialColor
	> class LedPixelsSingleColorGroup : 
		public T_Enabled,
		public T_InitialColor
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialColor )

	public:
		void ApplyColorsAll()
		{
			if( Enabled() )
				for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
					C_OWNER.SetPixelColor( C_START_PIXEL + i, InitialColor().GetValue() );

		}

	protected:
		void IntSetColor( TRGBWColor AValue )
		{
			if( InitialColor().GetValue() == AValue )
				return;

			InitialColor() = AValue;
			ApplyColorsAll(); 
//			FOwner.FModified = true;
		}

	public:
		inline void ColorInputPin_o_Receive( void *_Data )
		{
			IntSetColor( *(TRGBWColor *)_Data );
		}

	public:
		inline void SystemStart()
		{
			ApplyColorsAll();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_Brightness,
		typename T_InitialColor
	> class LedPixelsSingleColorLed : 
		public T_Brightness,
		public T_InitialColor
	{
	public:
		_V_PROP_( Brightness )
		_V_PROP_( InitialColor )

	public:
		inline void SetBrightness( float AValue ) // Used for Owner Delegation
		{
			C_OWNER.SetBrightness( AValue );
		}

		inline void UpdateBrightness()
		{
			C_OWNER.SetBrightness( Brightness() );
		}

	public:
		void ApplyColorsAll()
		{
			C_OWNER.SetPixelColor( 0, Func::SwapRB( InitialColor().GetValue() ));
		}

	protected:
		void IntSetColor( TColor AValue )
		{
			if( InitialColor().GetValue() == AValue )
				return;

			InitialColor() = AValue;
			ApplyColorsAll(); 
//			FOwner.FModified = true;
		}

	public:
		inline void ColorInputPin_o_Receive( void *_Data )
		{
			IntSetColor( *(TColor *)_Data );
		}

	public:
		inline void SystemStart()
		{
			ApplyColorsAll();
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
		uint16_t C_COUNT_PIXELS,
		uint16_t C_START_PIXEL,
		typename T_ColorOutputPin,
		typename T_Enabled,
		typename T_InitialColor,
		typename T_Reversed,
		typename T_StepInputPin_o_IsConnected
	> class LedPixelsRunningColorGroup : 
		public T_ColorOutputPin,
		public T_Enabled,
		public T_InitialColor,
		public T_Reversed,
		public T_StepInputPin_o_IsConnected
	{
	public:
		_V_PIN_( Enabled )
		_V_PIN_( ColorOutputPin )

	public:
		_V_PROP_( InitialColor )
		_V_PROP_( Reversed )

	public:
		_V_PROP_( StepInputPin_o_IsConnected )

	public:
		inline void SystemStart()
		{
			ApplyColorsAll();
		}

	public:
		inline void ApplyColorsAll()
		{
			if( Enabled() )
				for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
					C_OWNER.SetPixelColor( C_START_PIXEL + i, InitialColor().GetValue() );
		}

	protected:
		void ApplyColors() {}

		void IntSetColor( TRGBWColor AValue )
		{
			if( InitialColor().GetValue() == AValue )
				return;

			InitialColor() = AValue;
			ApplyColors(); 
//			FOwner.FModified = true;
		}

	public:
		inline void ColorInputPin_o_Receive( void *_Data )
		{
			IntSetColor( *(TRGBWColor *)_Data );
		}

	public:
		inline void UpdateReversePixels()
		{
			if( Reversed().AllPixels() )
				ReversePixels();

		}

	public:
		void ReversePixels()
		{
			if( Enabled() )
				for( uint32_t i = 0; i < C_COUNT_PIXELS / 2; ++i )
				{
					TRGBWColor AOldColor1 = C_OWNER.GetPixelColor( C_START_PIXEL + ( C_COUNT_PIXELS - i - 1 ));
					TRGBWColor AOldColor2 = C_OWNER.GetPixelColor( C_START_PIXEL + i );

					C_OWNER.SetPixelColor( C_START_PIXEL + i, AOldColor1 );
					C_OWNER.SetPixelColor( C_START_PIXEL + ( C_COUNT_PIXELS - i - 1 ), AOldColor2 );
				}

		}

	protected:
		void AnimatePixels()
		{
			if( ! Enabled().GetValue() )
				return;

			if( Reversed().Reversed() )
			{
				TRGBWColor AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL );
				T_ColorOutputPin::SetPinValue( AOldColor );
				for( uint32_t i = 0; i < C_COUNT_PIXELS - 1; ++i )
				{
					AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL + i + 1 );
					C_OWNER.SetPixelColor( C_START_PIXEL + i, AOldColor );
				}

				C_OWNER.SetPixelColor( C_START_PIXEL + C_COUNT_PIXELS - 1, InitialColor().GetValue() );
			}

			else
			{
				TRGBWColor AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL + C_COUNT_PIXELS - 1 );
				T_ColorOutputPin::SetPinValue( AOldColor );
				for( int i = C_COUNT_PIXELS - 1; i--; )
				{
					AOldColor = C_OWNER.GetPixelColor( C_START_PIXEL + i );
					C_OWNER.SetPixelColor( C_START_PIXEL + i + 1, AOldColor );
				}

				C_OWNER.SetPixelColor( C_START_PIXEL, InitialColor().GetValue() );
			}
//			FOwner.FModified = true;
		}

	public:
		inline void StepInputPin_o_Receive( void *_Data )
		{
			AnimatePixels();
		}

	public:
		inline void SystemLoopBegin()
		{
			if( StepInputPin_o_IsConnected() )
				return;

			AnimatePixels();
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		uint16_t C_COUNT_PIXELS,
		uint16_t C_START_PIXEL,
		typename T_Enabled,
		typename T_FModified,
		typename T_InitialColor,
		typename T_InitialIndex
	> class LedPixelsColorPixelGroup : 
		public T_Enabled,
		public T_FModified,
		public T_InitialColor,
		public T_InitialIndex
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialIndex )

	protected:
		_V_PROP_( FModified )

	public:
		_V_PROP_( InitialColor )

	public:
		inline void SystemStart()
		{
			if( Enabled() )
				for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
					C_OWNER.SetPixelColor( C_START_PIXEL + i, InitialColor().GetValue() );
		}

	public:
		inline void ApplyColorsAll()
		{
			if( Enabled() )
				for( uint32_t i = 0; i < C_COUNT_PIXELS; ++i )
					C_OWNER.SetPixelColor( C_START_PIXEL + i, InitialColor().GetValue() );
		}

	protected:
		void IntSetColor( TRGBWColor AValue )
		{
			if( InitialColor().GetValue() == AValue )
				return;

			InitialColor() = AValue;
			ApplyColors(); 
//			FOwner.FModified = true;
		}

	public:
		inline void ColorInputPin_o_Receive( void *_Data )
		{
			IntSetColor( *(TRGBWColor *)_Data );
		}

	public:
		inline void IndexInputPin_o_Receive( void *_Data )
		{
			uint32_t AIndex = *(uint32_t *)_Data;
			if( AIndex >= C_COUNT_PIXELS )
				AIndex = C_COUNT_PIXELS - 1;

			if( InitialIndex().GetValue() == AIndex )
				return;

			InitialIndex() = AIndex;
			FModified() = true;
		}

	protected:
		inline void ApplyColors()
		{
			FModified() = true;
		}

	public:
		inline void SystemLoopBegin()
		{
			if( FModified() )
			{
				C_OWNER.SetPixelColor( C_START_PIXEL + InitialIndex(), InitialColor().GetValue() );
//				FOwner.FModified = true;
				FModified() = false;
			}
		}

	public:
		inline LedPixelsColorPixelGroup()
		{
			FModified() = true;
		}

	};
//---------------------------------------------------------------------------
	class TRGBWColorTransfrom
	{
	public:
		static const TUniColor BLACK = 0xFF00000000;

	public:
		inline static uint8_t	ExtractGray( TUniColor AColor )
		{
			return ( AColor & 0xFF );
		}

		inline static uint8_t	ExtractAlpha( TUniColor AColor )
		{
			return ( AColor >> 32 );
		}

		inline static TUniColor	ExtractColor( TUniColor AColor )
		{
			return ( AColor & 0xFFFFFFFF );
		}

		inline static uint32_t	MakeGray( TUniColor AColor )
		{
			uint32_t AGray = ( AColor & 0xFF );
			return ( 0xFF00000000 | ( AGray << 16 ) | ( AGray << 16 ) | ( AGray << 8 ) | AGray );
		}

		inline static TUniColor	MakeColor( TUniColor AColor )
		{
			uint64_t AAlpha = ( AColor & 0xFF000000 );
			return ( AAlpha << 8 ) | ( AColor & 0xFFFFFF );
		}

		inline static TUniColor	BlendColor( TUniColor AOldColor, TUniColor AColor, float AAlpha )
		{
			float ACoefficient = AAlpha / 255.0f;

			uint8_t AWhite = (( AOldColor >> 24 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 24 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t ARed = (( AOldColor >> 16 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 16 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t AGreen = (( AOldColor >> 8 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 8 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t ABlue = ( AOldColor & 0xFF ) * ( 1 - ACoefficient ) + ( AColor & 0xFF ) * ACoefficient + 0.5;

			return ( ( uint32_t( AWhite ) << 24 ) | ( uint32_t( ARed ) << 16 ) | ( uint32_t( AGreen ) << 8 ) | ABlue );
		}

		inline static TUniColor	TransformBackgroundColor( TUniColor AColor )
		{
			return AColor;
		}
	};
//---------------------------------------------------------------------------
	class TRGBColorTransfrom
	{
	public:
		static const TUniColor BLACK = 0xFF000000;

	public:
		inline static uint8_t	ExtractGray( TUniColor AColor )
		{
			return ( AColor & 0xFF );
		}

		inline static uint8_t	ExtractAlpha( TUniColor AColor )
		{
			return ( AColor >> 24 );
		}

		inline static TUniColor	ExtractColor( TUniColor AColor )
		{
			return ( AColor & 0xFFFFFF );
		}

		inline static uint32_t	MakeGray( TUniColor AColor )
		{
			uint16_t AGray = ( AColor & 0xFF );
			return ( 0xFF000000 | ( AColor << 16 ) | ( AGray << 8 ) | AGray );
		}

		inline static TUniColor	MakeColor( TUniColor AColor )
		{
			return AColor;
//			uint64_t AAlpha = ( AColor & 0xFF000000 );
//			return ( AAlpha << 8 ) | ( AColor & 0xFFFFFF );
		}

		inline static TUniColor	BlendColor( TUniColor AOldColor, TUniColor AColor, float AAlpha )
		{
			float ACoefficient = AAlpha / 255.0f;

			uint8_t ARed = (( AOldColor >> 16 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 16 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t AGreen = (( AOldColor >> 8 ) & 0xFF ) * ( 1 - ACoefficient ) + (( AColor >> 8 ) & 0xFF ) * ACoefficient + 0.5;
			uint8_t ABlue = ( AOldColor & 0xFF ) * ( 1 - ACoefficient ) + ( AColor & 0xFF ) * ACoefficient + 0.5;

			return ( ( uint32_t( ARed ) << 16 ) | ( uint32_t( AGreen ) << 8 ) | ABlue );
		}

		inline static TUniColor	TransformBackgroundColor( TUniColor AColor )
		{
			return (( AColor & 0xFF ) << 16 ) | ( AColor & 0x00FF00 ) | (( AColor >> 16 ) & 0xFF );
		}
	};
//---------------------------------------------------------------------------
    class LedPixelsStructure_SingleDirection
	{
	public:
		inline static uint32_t	Remap( uint32_t x, uint32_t y, uint32_t AWidth, uint32_t AHeight )
		{
			return x + y * AWidth;
		}

	};
//---------------------------------------------------------------------------
    class LedPixelsStructure_ZigZag
	{
	public:
		inline static uint32_t	Remap( uint32_t x, uint32_t y, uint32_t AWidth, uint32_t AHeight )
		{
			if( y & 1 )				
				return ( AWidth - x - 1 ) + y * AWidth;

			return x + y * AWidth;
		}

	};
//---------------------------------------------------------------------------
//#if 0
	template <
		typename T_OWNER, T_OWNER &C_OWNER, 
		typename T_MATRIX_MAP,
		uint16_t C_COUNT_PIXELS, 
		uint16_t C_START_PIXEL,
		typename T_BackgroundColor,
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
	> class LedPixels2DGraphicsGroup : 
		public T_MATRIX_MAP,
		public GraphicsImpl<
			LedPixels2DGraphicsGroup<
						T_OWNER, C_OWNER, 
                        T_MATRIX_MAP,
						C_COUNT_PIXELS, 
                        C_START_PIXEL,
						T_BackgroundColor,
						T_Elements_Render,
						T_Enabled,
                        T_FCursorX,
                        T_FCursorY,
						HEIGHT,
                        T_Mirror,
						T_Orientation,
						T_COLOR_TRANSFORM, 
						T_Text,
						WIDTH
					>, 
				T_BackgroundColor,
				T_Text
			>,
        public T_FCursorX,
        public T_FCursorY,
		public T_Enabled,
		public T_Mirror,
		public T_Orientation
//		public T_Text
	{
		typedef LedPixels2DGraphicsGroup T_SELF;

		typedef GraphicsImpl<
				T_SELF,
				T_BackgroundColor,
				T_Text
			>	graphics;

	public:
		_V_PROP_( Enabled )
		_V_PROP_( Mirror )
		_V_PROP_( Orientation )
		_V_PROP_( Text )

	public:
		_V_PROP_( FCursorX )
		_V_PROP_( FCursorY )

	public:
		static inline void Elements_CallChain()
		{
			T_Elements_Render::Call();
		}

	public:
		void SetCursor( int16_t x, int16_t y )
		{
			FCursorX() = x;
			FCursorY() = y;
		}

	public:
		constexpr inline static bool ImplementsPolygon()
		{
			return false;
		}

		template <typename T_POLYGON> void RenderPolygon( T_POLYGON &APolygon, TGraphicsPos AX, TGraphicsPos AY, bool AFill, TUniColor AColor ) {} // Placeholder

		inline int16_t width(void) const
		{
			if( T_Orientation::GetValue() == goUp || T_Orientation::GetValue() == goDown )
				return WIDTH;

			return HEIGHT;
		}

		inline int16_t height(void) const
		{
			if( T_Orientation::GetValue() == goUp || T_Orientation::GetValue() == goDown )
				return HEIGHT;

			return WIDTH;
		}

		static inline TUniColor NonTransparent( TUniColor color )
		{
			return 0xFF000000 | ::Mitov::Func::SwapRB( color );
		}

	public:
		inline T_OWNER &GetOwner() { return C_OWNER; }

	public:
		inline void SetBrightness( float AValue ) // Used for Owner Delegation
		{
			C_OWNER.SetBrightness( AValue );
		}

	public:
		inline void RefreshInputPin_o_Receive( void *_Data ) // Used for Owner Delegation
		{
			C_OWNER.ClockInputPin_o_Receive( _Data );
		}

	public:
		static inline void drawGrayscalePixel( T_SELF &AImplementation, int16_t x, int16_t y, TUniColor color)
		{
			AImplementation.drawPixel( x, y, T_COLOR_TRANSFORM::MakeGray( color ));
		}

		static inline void fillGrayscaleRect( T_SELF &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
			fillGrayscaleRect( AImplementation, x, y, w, h, T_COLOR_TRANSFORM::MakeGray( color ));
		}

		static inline void drawColorPixel( T_SELF &AImplementation, int16_t x, int16_t y, TUniColor color )
		{
			AImplementation.drawPixel( x, y, T_COLOR_TRANSFORM::MakeColor( color ) );
		}

		static inline void fillColorRect( T_SELF &AImplementation, int16_t x, int16_t y, int16_t w, int16_t h, TUniColor color)
		{
//			Serial.println( "TEST88" );
			fillGrayscaleRect( AImplementation, x, y, w, h, T_COLOR_TRANSFORM::MakeColor( color ) );
		}

		void drawPixel( int16_t x, int16_t y, TUniColor color )
		{
			if( ! Enabled().GetValue() )
				return;

//			Serial.println( "GRAPHICS" );
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return;

//			Serial.println( uint32_t( color ), HEX );
//			Serial.print( "drawPixel: " );
//			color = 0xFFFFFF0000;
			uint8_t AAlpha = T_COLOR_TRANSFORM::ExtractAlpha( color );
			if( AAlpha == 0 )
				return;

//			Serial.print( x ); Serial.print( ":" ); Serial.print( y ); Serial.print( " = " ); Serial.print( uint32_t( AAlpha ), HEX ); Serial.print( " " ); Serial.println( uint32_t( color ), HEX ); 

			switch ( Orientation() ) 
			{
				case goRight:
					swap(x, y);
					x = WIDTH - x - 1;
					break;

				case goDown:
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = HEIGHT - y - 1;
					break;
			}  

			color = T_COLOR_TRANSFORM::ExtractColor( color );

//			Serial.print( "Color: " ); Serial.println( uint32_t( color ), HEX ); 
			if( Mirror().Horizontal() )
				x = WIDTH - x - 1;

			if( Mirror().Vertical() )
				y = HEIGHT - y - 1;

			uint32_t APixelIndex = C_START_PIXEL + T_MATRIX_MAP::Remap( x, y, WIDTH, HEIGHT );

			if( AAlpha < 255 )
			{
				TUniColor AOldColor = C_OWNER.GetPixelColor( APixelIndex );
				color = T_COLOR_TRANSFORM::BlendColor( AOldColor, color, AAlpha );
			}

//			color = 0xFFFF0000;
//			Serial.print( C_START_PIXEL + x + y * WIDTH ); Serial.print( " = " ); Serial.println( uint32_t( color ));
			C_OWNER.SetPixelColor( APixelIndex, color );
//			FBuffer[ x + y * WIDTH ] = color;
//			FModified = true;
		}

		TUniColor GetPixelColor( int16_t x, int16_t y )
		{
			if ((x < 0) || (x >= width() ) || (y < 0) || (y >= height() ))
				return T_COLOR_TRANSFORM::BLACK;

			switch ( Orientation() ) 
			{
				case goRight:
					swap(x, y);
					x = WIDTH - x - 1;
					break;

				case goDown:
					x = WIDTH - x - 1;
					y = HEIGHT - y - 1;
					break;

				case goLeft:
					swap(x, y);
					y = HEIGHT - y - 1;
					break;
			}  

			if( Mirror().Horizontal() )
				x = WIDTH - x - 1;

			if( Mirror().Vertical() )
				y = HEIGHT - y - 1;

			return TUniColor( C_OWNER.GetPixelColor( C_START_PIXEL + T_MATRIX_MAP::Remap( x, y, WIDTH, HEIGHT ) )) | T_COLOR_TRANSFORM::BLACK;
//			return TUniColor( FBuffer[ x + y * WIDTH ] ) | 0xFF00000000;
		}

/*
	protected:
		inline void RenderElements()
		{
			graphics::RenderElements();
		}
*/
	public:
		inline void SystemStart()
		{
//			graphics::setTextSize( Text().Size() );
//			graphics::setTextColor( Text().Color(), Text().BackgroundColor().GetValue() );
//			graphics::setTextWrap( Text().Wrap() );

			graphics::ClearScreen( *this, T_COLOR_TRANSFORM::TransformBackgroundColor( graphics::BackgroundColor().GetValue() ) | T_COLOR_TRANSFORM::BLACK );
//			graphics::fillRect( *this, 0, 0, width(), height(), graphics::BackgroundColor | T_COLOR_TRANSFORM::BLACK );
//			Serial.println( "SystemStart" );
			graphics::RenderElements();
//			for( int i = 0; i < FElements.size(); ++ i )
//				FElements[ i ]->Render( false );
		}

/*
		virtual void PixelsClock()
		{
			if( FModified )
			{
				for( int i = 0; i < WIDTH * HEIGHT; ++i )
				{
//					Serial.print( i ); Serial.print( " = " ); Serial.println( FBuffer[ i ], HEX ); 
					C_OWNER.SetPixelColor( C_START_PIXEL + i, FBuffer[ i ] );
				}

				FModified = false;
			}
		}
*/
	public:
		template <typename T_CONFIG> inline static size_t write( T_SELF &AImplementation, T_CONFIG &AConfig, uint8_t AValue )
		{
			return graphics::write( AImplementation, AConfig, AValue );
		}

		template <typename T> void PrintChar( T AValue )
		{
			write( *this, *this, AValue );
		}

	public:
		template <typename T> void Print( T AValue )
		{
			println( AValue );
		}

	public:
		template <typename T_CONFIG, typename T> size_t print( T_CONFIG *AConfig, T AValue )
		{
			GraphicsPrint<T_SELF, T_CONFIG> APrint( this, AConfig, (typename GraphicsPrint<T_SELF, T_CONFIG>::TCallback) &graphics::template write<T_CONFIG> );
			return APrint.print( AValue );
		}

		template <typename T_CONFIG, typename T> size_t println( T_CONFIG *AConfig, T AValue )
		{
			GraphicsPrint<T_SELF, T_CONFIG> APrint( this, AConfig, (typename GraphicsPrint<T_SELF, T_CONFIG>::TCallback) &graphics::template write<T_CONFIG> );
			return APrint.println( AValue );
		}

		template <typename T> inline size_t print( T AValue )
		{
			return print<T_SELF, T>( this, AValue );
		}

		template <typename T> inline size_t println( T AValue )
		{
			return println<T_SELF, T>( this, AValue );
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER, 
		uint8_t C_INDEX,
		typename T_Enabled,
		typename T_InitialValue
	> class LedPixelSingleLED :
		public T_Enabled,
		public T_InitialValue
	{
	public:
		_V_PROP_( Enabled )
		_V_PROP_( InitialValue )

	public:
		inline void SystemStart()
		{
			if( Enabled() )
				C_OWNER.SetPixelColor( C_INDEX, TRGBWColor( InitialValue().GetValue() ));

			else
				C_OWNER.SetPixelColor( C_INDEX, TRGBWColor( 0 ));

//			C_OWNER.SetChannelValue( C_INDEX, 1 - InitialValue );
		}

	public:
		inline void InputPin_o_Receive( void *_Data )
		{
			TColor AColor = *(TColor *)_Data;
			if( AColor == InitialValue().GetReference() )
				return;

			InitialValue() = AColor;
			SystemStart();
		}
	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif


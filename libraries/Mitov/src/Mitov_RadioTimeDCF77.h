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

#ifdef __TEST_FOR_DEBUG_PRINTS__
#define Serial UNGUARDED DEBUG PRINT!!!
#endif

namespace Mitov
{
//---------------------------------------------------------------------------
    template <
        typename T_Enabled,
        typename T_FBufferPosition,
        typename T_FCEST,
        typename T_FFilledBufferAvailable,
//        typename T_FParityDate,
//        typename T_FParityFlag,
//        typename T_FParityHour,
//        typename T_FParityMin,
        typename T_FPulseStart,
        typename T_FUp,
        typename T_OnRisingEdge,
        typename T_OutputPin,
        typename T_TimeZoneOffset
    > class TArduinoRadioTimeDCF77 :
        public T_Enabled,
        public T_FBufferPosition,
        public T_FCEST,
        public T_FFilledBufferAvailable,
//        public T_FParityDate,
//        public T_FParityFlag,
//        public T_FParityHour,
//        public T_FParityMin,
        public T_FPulseStart,
        public T_FUp,
        public T_OnRisingEdge,
        public T_OutputPin,
        public T_TimeZoneOffset
    {
    protected:
//        static const uint32_t   C_MIN_TIME = 1334102400;     // Date: 11-4-2012
//        static const uint32_t   C_MAX_TIME = 4102444800;     // Date:  1-1-2100

        static const uint16_t   C_DCFRejectionTime = 700;    // Pulse-to-Pulse rejection time.
        static const uint8_t    C_DCFRejectPulseWidth = 50;  // Minimal pulse width
        static const uint8_t    C_DCFSplitTime = 180;        // Specifications distinguishes pulse width 100 ms and 200 ms. In practice we see 130 ms and 230
        static const uint16_t   C_DCFSyncTime = 1500;        // Specifications defines 2000 ms pulse for end of sequence

    protected:
        struct DCF77Buffer
        {
            //uint64_t prefix       : 21;
            uint64_t prefix     : 17;
            uint64_t CEST       : 1; // CEST
            uint64_t CET        : 1; // CET
            uint64_t unused     : 2; // unused bits
            uint64_t Min        : 7; // minutes
            uint64_t P1         : 1; // parity minutes
            uint64_t Hour       : 6; // hours
            uint64_t P2         : 1; // parity hours
            uint64_t Day        : 6; // day
            uint64_t Weekday    : 3; // day of week
            uint64_t Month      : 5; // month
            uint64_t Year       : 8; // year (5 -> 2005)
            uint64_t P3         : 1; // parity
        };

    protected:
        unsigned long FLeadingEdge = 0;
        unsigned long FTrailingEdge = 0;
        unsigned long FPreviousLeadingEdge = 0;

        uint64_t FRunningBuffer = 0;

        volatile uint64_t FFilledBuffer;
//        volatile unsigned long FFilledTimestamp;

//        uint64_t FProcessingBuffer = 0;

    public:
        _V_PIN_( OutputPin )

    public:
        _V_PROP_( Enabled )
        _V_PROP_( OnRisingEdge )
        _V_PROP_( TimeZoneOffset )

    protected:
        _V_PROP_( FBufferPosition )
        _V_PROP_( FCEST )
        _V_PROP_( FFilledBufferAvailable )
//        _V_PROP_( FParityDate )
//        _V_PROP_( FParityFlag )
//        _V_PROP_( FParityHour )
//        _V_PROP_( FParityMin )
        _V_PROP_( FPulseStart )
        _V_PROP_( FUp )

    protected:
        inline unsigned long Now()
        {
            return millis();
        }

        void FinalizeBuffer()
        {
            if( FBufferPosition().GetValue() == 59 )
            {
                // Buffer is full
//                LogLn("BF");
                // Prepare filled buffer and time stamp for main loop
                FFilledBuffer = FRunningBuffer;
//                FFilledTimestamp = Now();
                // Reset running buffer
//                Bufferinit();
                FFilledBufferAvailable() = true;
            }
//            else
//            {
                // Buffer is not yet full at end of time-sequence
//                LogLn("EoM");
                // Reset running buffer
//                Bufferinit();
//            }

        	FRunningBuffer = 0;
        	FBufferPosition() = 0;
        }

        void AppendSignal( uint8_t ASignal )
        {
//            Log(signal, DEC);
            FRunningBuffer |= ((uint64_t) ASignal << FBufferPosition().GetValue() );
            FBufferPosition() = FBufferPosition() + 1;
            if( FBufferPosition().GetValue() > 59 )
            {
                // Buffer is full before at end of time-sequence
                // this may be due to noise giving additional peaks
//                LogLn("EoB");
                FinalizeBuffer();
            }
        }

        inline void ProcessBuffer()
        {
            if( ! Enabled().GetValue() )
                return;

            if( not FFilledBufferAvailable().GetValue() )
                return;

        	/////  Start interaction with interrupt driven loop  /////

        	// Copy filled buffer and timestamp from interrupt driven loop
            noInterrupts();
        	uint64_t AProcessingBuffer = FFilledBuffer;

//        	processingTimestamp = FFilledTimestamp;
        	// Indicate that there is no filled, unprocessed buffer anymore
        	FFilledBufferAvailable() = false;
            interrupts();

        	/////  End interaction with interrupt driven loop   /////

        	//  Calculate parities for checking buffer
        	uint8_t AParityFlag = false;
        	uint8_t AParityMin = false;
        	uint8_t AParityHour = false;
        	uint8_t AParityDate = false;

        	for( int pos = 0; pos < 59; ++ pos )
            {
        		bool s = ( AProcessingBuffer >> pos ) & 1;

        		// Update the parity bits. First: Reset when minute, hour or date starts.
        		if( pos == 21 || pos == 29 || pos == 36 )
        			AParityFlag = false;

        		// save the parity when the corresponding segment ends
        		if ( pos == 28 )
                    AParityMin = AParityFlag;

        		if ( pos == 35 )
                    AParityHour = AParityFlag;

        		if ( pos == 58 )
                    AParityDate = AParityFlag;

        		// When we received a 1, toggle the parity flag
        		if( s == 1 )
        			AParityFlag = AParityFlag ^ 1;

        	}

//        	tmElements_t time;
        	bool proccessedSucces;

        	DCF77Buffer *rx_buffer = (DCF77Buffer *)&AProcessingBuffer;

        	// Check parities
            if ( AParityMin == rx_buffer->P1 &&
                AParityHour == rx_buffer->P2 &&
                AParityDate == rx_buffer->P3 &&
        		rx_buffer->CEST != rx_buffer->CET )
            {
                //convert the received buffer into time
                uint16_t AMinute = rx_buffer->Min-((rx_buffer->Min/16)*6);
                uint16_t AHour   = rx_buffer->Hour-((rx_buffer->Hour/16)*6);
                uint16_t ADay    = rx_buffer->Day-((rx_buffer->Day/16)*6);
                uint16_t AMonth  = rx_buffer->Month-((rx_buffer->Month/16)*6);
                uint16_t AYear   = 2000 + rx_buffer->Year-((rx_buffer->Year/16)*6);

                TDateTime ADateTime;

                if( ADateTime.TryEncodeDateTime( AYear, AMonth, ADay, AHour, AMinute, 0, 0 ))
                {
    				if( TimeZoneOffset().GetValue() != 0 ) // For compiler optimization
    				{
    					int32_t	AIntOffset = TimeZoneOffset();

    //					Serial.println( AIntOffset );

    					ADateTime.AddHours( AIntOffset );
    					float AFraction = V_FMOD( TimeZoneOffset(), 1 );
    //					Serial.println( AFraction );

    					AIntOffset = AFraction * 60;

    //					Serial.println( AIntOffset );
    					ADateTime.AddMinutes( AIntOffset );
    				}

                    T_OutputPin::SetPinValue( ADateTime );
                }

//        	      latestupdatedTime = makeTime(time);
                FCEST() = rx_buffer->CEST;
                //Parity correct
                return true;
        	}

       	  //Parity incorrect
       	  return false;
        }

    public:
        void __ICACHE_RAM_ATTR__ InterruptHandler( bool ASensorValue )
        {
        	unsigned long AFlankTime = millis();

        	// If flank is detected quickly after previous flank up
        	// this will be an incorrect pulse that we shall reject
        	if(( AFlankTime - FPreviousLeadingEdge ) < C_DCFRejectionTime )
     	    {
//        		LogLn("rCT");
        		return;
        	}

        	// If the detected pulse is too short it will be an
        	// incorrect pulse that we shall reject as well
        	if(( AFlankTime - FLeadingEdge ) < C_DCFRejectPulseWidth )
      	    {
//        	    LogLn("rPW");
        		return;
        	}

        	if( ASensorValue == FPulseStart() )
       	    {
        		if ( ! FUp().GetValue() )
       		    {
        			// Flank up
        			FLeadingEdge = AFlankTime;
        			FUp() = true;
        		}
        	}
        	else
        	{
        		if( FUp() )
        		{
        			// Flank down
        			FTrailingEdge = AFlankTime;
        			int ADifference = FTrailingEdge - FLeadingEdge;

        			if(( FLeadingEdge - FPreviousLeadingEdge ) > C_DCFSyncTime )
        				FinalizeBuffer();

        			FPreviousLeadingEdge = FLeadingEdge;
        			// Distinguish between long and short pulses
        			if( ADifference < C_DCFSplitTime )
        			    AppendSignal( 0 );

        			else
        			    AppendSignal( 1 );

        			FUp() = false;
        		}
        	}
        }

    public:
        inline void SystemInit()
        {
            FPulseStart() = OnRisingEdge();
        	FUp() = false;
        	FFilledBufferAvailable() = false;

        	FBufferPosition() = 0;

//        	flags.parityDate      = 0;
//        	flags.parityFlag      = 0;
//        	flags.parityHour      = 0;
//        	flags.parityMin       = 0;

        	FCEST() = 0;
        }

        inline void SystemLoopBegin()
        {
            ProcessBuffer();
        }


    };
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
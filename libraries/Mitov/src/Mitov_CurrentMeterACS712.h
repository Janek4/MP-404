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
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_CompletedOutputPin,
		typename T_Enabled,
		typename T_FCalibrating,
		typename T_FSamplesCounter,
		typename T_Samples
	> class TArduinoCurrentMeterACS712CalibrateSamples :
		public T_CompletedOutputPin,
		public T_Enabled,
		public T_FCalibrating,
		public T_FSamplesCounter,
		public T_Samples
	{
	public:
		_V_PROP_( CompletedOutputPin )
		_V_PROP_( Enabled )
		_V_PROP_( Samples )

	protected:
		_V_PROP_( FCalibrating )
		_V_PROP_( FSamplesCounter )

	public:
		inline void StartInputPin_o_Receive( void *_Data )
		{
			FCalibrating() = true;
			C_OWNER.ZeroOffset() = 0.0;
			FSamplesCounter() = Samples().GetValue();
		}

	public:
		inline void ProcessSample( float AValue, bool & ACalibrating )
		{
			if( ! FCalibrating().GetValue() )
				return;

			ACalibrating = true;
			C_OWNER.ZeroOffset() = C_OWNER.ZeroOffset() + AValue;
			FSamplesCounter() = FSamplesCounter() - 1;
			if( FSamplesCounter() == 0 )
			{
				C_OWNER.ZeroOffset() = C_OWNER.ZeroOffset().GetValue() / Samples().GetValue();
				FCalibrating() = false;
			}
		}

	public:
		inline TArduinoCurrentMeterACS712CalibrateSamples()
		{
			FCalibrating() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_OWNER, T_OWNER &C_OWNER,
		typename T_CompletedOutputPin,
		typename T_Enabled,
		typename T_FCalibrating,
		typename T_Period
	> class TArduinoCurrentMeterACS712CalibratePeriod :
		public T_CompletedOutputPin,
		public T_Enabled,
		public T_FCalibrating,
		public T_Period
	{
	public:
		_V_PROP_( CompletedOutputPin )
		_V_PROP_( Enabled )
		_V_PROP_( Period )

	protected:
		_V_PROP_( FCalibrating )

		uint32_t FSamples;

		unsigned long FStartTime;

	public:
		inline void StartInputPin_o_Receive( void *_Data )
		{
			FCalibrating() = true;
			C_OWNER.ZeroOffset() = 0.0;
			FSamples = 0;
			FStartTime = micros();
		}

	public:
		inline void ProcessSample( float AValue, bool & ACalibrating )
		{
			if( ! FCalibrating().GetValue() )
				return;

			ACalibrating = true;

			C_OWNER.ZeroOffset() = C_OWNER.ZeroOffset() + AValue;
			++ FSamples;

			if( ( micros() - FStartTime ) > Period().GetValue() )
			{
				C_OWNER.ZeroOffset() = C_OWNER.ZeroOffset().GetValue() / FSamples;
				FCalibrating() = false;
			}
		}

	public:
		inline TArduinoCurrentMeterACS712CalibratePeriod()
		{
			FCalibrating() = false;
		}

	};
//---------------------------------------------------------------------------
	template <
		typename T_Enabled,
		typename T_Operations_ProcessSample,
		typename T_OutputPin,
		typename T_Range,
		typename T_VoltageReference,
		typename T_ZeroOffset
	> class TArduinoCurrentMeterACS712 :
		public T_Enabled,
		public T_OutputPin,
		public T_Range,
		public T_VoltageReference,
		public T_ZeroOffset
	{
	public:
		_V_PIN_( OutputPin )

	public:
		_V_PROP_( Enabled )
		_V_PROP_( Range )
		_V_PROP_( VoltageReference )
		_V_PROP_( ZeroOffset )

	public:
		inline float GetVoltageReference()
		{
			if( VoltageReference().Enabled() )
				return VoltageReference().Value().GetValue();

			return __V_REFF_;
		}

		inline void InputPin_o_Receive( void *_Data )
		{
			if( ! Enabled().GetValue() )
				return;

			float AValue = ( *(float *)_Data ) - ZeroOffset().GetValue();

			bool AInValibration = false;
			T_Operations_ProcessSample::Call( AValue, AInValibration );
			if( AInValibration )
				return;

			if( Range().GetValue() <= 5.0 )
				AValue *= GetVoltageReference() * 0.185;

			else if( Range().GetValue() <= 20.0 )
				AValue *= GetVoltageReference() * 0.100;

			else
				AValue *= GetVoltageReference() * 0.066;

			T_OutputPin::SetPinValue( AValue );
		}

	};
//---------------------------------------------------------------------------
}

#ifdef __TEST_FOR_DEBUG_PRINTS__
#undef Serial
#endif
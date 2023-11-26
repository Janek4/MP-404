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
    namespace XPowers_AXP192
    {
        const uint8_t REG_00_PowerSupplyStatus = 0x00;
        const uint8_t REG_01_PowerSupplyOperatingMode_ChargingStatusIndication = 0x01;
        const uint8_t REG_02_USB_OTG_VBUS_StatusIndication = 0x02;
        const uint8_t REG_06_DataBuffer = 0x06;
        const uint8_t REG_10_EXTEN_DC_DC2_Output_Control = 0x10;
        const uint8_t REG_12_PowerOutputControl = 0x12;
        const uint8_t REG_23_DC_DC2_OutputVoltage = 0x23;
        const uint8_t REG_25_DC_DC2_DynamicVoltageRegulationParameters = 0x25;
        const uint8_t REG_26_DC_DC1_OutputVoltage = 0x26;
        const uint8_t REG_27_DC_DC3_OutputVoltage = 0x27;
        const uint8_t REG_28_LDO2_3_OutputVoltage = 0x28;
        const uint8_t REG_30_VBUS_IPSOUT_ChannelManagement = 0x30;
        const uint8_t REG_31_V_OFF_ShutdownVoltage = 0x31;
        const uint8_t REG_32_Shutdown_BatteryDetection_CHGLED_Control = 0x32;
        const uint8_t REG_33_ChargingControl_1 = 0x33;
        const uint8_t REG_34_ChargingControl_2 = 0x34;
        const uint8_t REG_35_BackupBatteryChargingControl = 0x35;
        const uint8_t REG_36_PEK_Control = 0x36;
        const uint8_t REG_37_DC_DC_OperatingFrequency = 0x37;
        const uint8_t REG_38_V_LTF_Charge_BatteryChargingLowTemperatureThreshold = 0x38;
        const uint8_t REG_39_V_HTF_Charge_BatteryChargingHighTemperatureThreshold = 0x39;
        const uint8_t REG_3A_APS_LowPowerLevel_1 = 0x3A;
        const uint8_t REG_3B_APS_LowPowerLevel_2 = 0x3B;
        const uint8_t REG_3C_V_LTF_Discharge_BatteryDischargeLowTemperatureThreshold = 0x3C;
        const uint8_t REG_3D_V_HTF_Discharge_BatteryDischargeHighTemperatureThreshold = 0x3D;
        const uint8_t REG_80_DC_DC_WorkingMode = 0x80;
        const uint8_t REG_82_ADC_Control_1 = 0x82;
        const uint8_t REG_83_ADC_Control_2 = 0x83;
        const uint8_t REG_84_ADC_SampleRate_TS_PinControl = 0x84;
        const uint8_t REG_85_ADC_InputRange = 0x85;
        const uint8_t REG_86_GPIO1_ADC_IRQ_RisingEdgeThreshold = 0x86;
        const uint8_t REG_87_GPIO1_ADC_IRQ_FallingEdgeThreshold = 0x87;
        const uint8_t REG_8A_TimerControl = 0x8A;
        const uint8_t REG_8B_VBUS_Monitoring_SRP_Control = 0x8B;
        const uint8_t REG_8F_OverTemperatureShutdown = 0x8F;
        const uint8_t REG_90_GPIO0_FunctionControl = 0x90;
        const uint8_t REG_91_GPIO0_LDO_VoltageControl = 0x91;
        const uint8_t REG_92_GPIO1_FunctionControl = 0x92;
        const uint8_t REG_93_GPIO2_FunctionControl = 0x93;
        const uint8_t REG_94_GPIO_2_0_SignalControl_Status = 0x94;
        const uint8_t REG_95_GPIO_4_3_FunctionControl = 0x95;
        const uint8_t REG_96_GPIO_4_3_SignalStatus = 0x96;
        const uint8_t REG_97_GPIO_2_0_InputPullDownControl = 0x97;
        const uint8_t REG_98_PWM1_OutputFrequency = 0x98;
        const uint8_t REG_99_PWM1_DutyCycle_1 = 0x99;
        const uint8_t REG_9A_PWM1_DutyCycle_2 = 0x9A;
        const uint8_t REG_9B_PWM2_OutputFrequency = 0x9B;
        const uint8_t REG_9C_PWM2_DutyCycle_1 = 0x9C;
        const uint8_t REG_9D_PWM2_DutyCycle_2 = 0x9D;
        const uint8_t REG_9E_N_RSTO_GPIO5_FunctionControl = 0x9E;
        const uint8_t REG_40_IRQ_Control_1 = 0x40;
        const uint8_t REG_41_IRQ_Control_2 = 0x41;
        const uint8_t REG_42_IRQ_Control_3 = 0x42;
        const uint8_t REG_43_IRQ_Control_4 = 0x43;
        const uint8_t REG_4A_IRQ_Control_5 = 0x4A;
        const uint8_t REG_44_IRQ_Status_1 = 0x44;
        const uint8_t REG_45_IRQ_Status_2 = 0x45;
        const uint8_t REG_46_IRQ_Status_3 = 0x46;
        const uint8_t REG_47_IRQ_Status_4 = 0x47;
        const uint8_t REG_4D_IRQ_Status_5 = 0x4D;
        const uint8_t REG_B8_CoulombCounterControl = 0xB8;
    }
//---------------------------------------------------------------------------
}


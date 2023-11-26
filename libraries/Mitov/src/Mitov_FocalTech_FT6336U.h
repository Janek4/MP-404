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
    namespace FocalTech_FT6336U
    {
        const uint8_t REG_DEV_MODE = 0x00;
        const uint8_t REG_GEST_ID = 0x01;
        const uint8_t REG_TD_STATUS = 0x02;

        const uint8_t REG_P1_XH = 0x03;
        const uint8_t REG_P1_XL = 0x04;
        const uint8_t REG_P1_YH = 0x05;
        const uint8_t REG_P1_YL = 0x06;
        const uint8_t REG_P1_WEIGHT = 0x07;
        const uint8_t REG_P1_MISC = 0x08;

        const uint8_t REG_P2_XH = 0x09;
        const uint8_t REG_P2_XL = 0x0A;
        const uint8_t REG_P2_YH = 0x0B;
        const uint8_t REG_P2_YL = 0x0C;
        const uint8_t REG_P2_WEIGHT = 0x0D;
        const uint8_t REG_P2_MISC = 0x0E;

        const uint8_t REG_TH_GROUP = 0x80;
        const uint8_t REG_TH_DIFF = 0x85;
        const uint8_t REG_CTRL = 0x86;
        const uint8_t REG_TIMEENTERMONITOR = 0x87;
        const uint8_t REG_PERIODACTIVE = 0x88;
        const uint8_t REG_PERIODMONITOR = 0x89;

        const uint8_t REG_RADIAN_VALUE = 0x91;
        const uint8_t REG_OFFSET_LEFT_RIGHT = 0x92;
        const uint8_t REG_OFFSET_UP_DOWN = 0x93;
        const uint8_t REG_DISTANCE_LEFT_RIGHT = 0x94;
        const uint8_t REG_DISTANCE_UP_DOWN = 0x95;
        const uint8_t REG_DISTANCE_ZOOM = 0x96;
        const uint8_t REG_LIB_VER_H = 0xA1;
        const uint8_t REG_LIB_VER_L = 0xA2;
        const uint8_t REG_CIPHER = 0xA3;
        const uint8_t REG_G_MODE = 0xA4;
        const uint8_t REG_PWR_MODE = 0xA5;
        const uint8_t REG_FIRMID = 0xA6;
        const uint8_t REG_FOCALTECH_ID = 0xA8;
        const uint8_t REG_RELEASE_CODE_ID = 0xAF;
        const uint8_t REG_STATE = 0xBC;
    }
//---------------------------------------------------------------------------
/*
    template <
		typename T_I2C, T_I2C &C_I2C,
		typename T_Enabled
    > class TArduino_FocalTech_Touch_FT6x36 :
		public T_Enabled
    {
    public:
        _V_PROP_( Enabled )

    };
*/
//---------------------------------------------------------------------------
}


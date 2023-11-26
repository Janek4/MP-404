////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//     This software is supplied under the terms of a license agreement or    //
//     nondisclosure agreement with Mitov Software and may not be copied      //
//     or disclosed except in accordance with the terms of that agreement.    //
//         Copyright(c) 2002-2022 Mitov Software. All Rights Reserved.        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Mitov
{
//---------------------------------------------------------------------------
    class TMutex;
//---------------------------------------------------------------------------
    class TMutexLock
    {
    protected:
        TMutex *FLock;

    public:
        inline TMutexLock( TMutex *ALock );
        inline ~TMutexLock();

    };
//---------------------------------------------------------------------------
    class TMutex
    {
        friend class TMutexLock;

    protected:
        SemaphoreHandle_t FMutex;
        uint16_t FNestingCounter;

    public:
        TMutexLock GetLock()
        {
            return TMutexLock( this );
        }

    public:
        inline TMutex()
        {
            FMutex = xSemaphoreCreateMutex();
        }

        inline ~TMutex()
        {
            vSemaphoreDelete( FMutex );
        }
    };
//---------------------------------------------------------------------------
    inline TMutexLock::TMutexLock( TMutex *ALock ) :
        FLock( ALock )
    {
        TaskHandle_t mutex_holder = xSemaphoreGetMutexHolder( FLock->FMutex );
        if( mutex_holder != NULL && mutex_holder == xTaskGetCurrentTaskHandle() )
        {
            ++ ( FLock->FNestingCounter );
            return; // we are already holding this mutex - no need to take it
        }

        // we are not holding the mutex - wait for it and take it
        xSemaphoreTake( FLock->FMutex, portMAX_DELAY );
    //            if(xSemaphoreTake(_i2s_general_mutex, portMAX_DELAY) != pdTRUE )
    //                log_e("I2S internal mutex take returned with error");

    }
//---------------------------------------------------------------------------
    inline TMutexLock::~TMutexLock()
    {
        if( FLock->FNestingCounter )
            -- ( FLock->FNestingCounter );

        else
            xSemaphoreGive( FLock->FMutex );
    //            {
    //                if(xSemaphoreGive(_i2s_general_mutex) != pdTRUE)
    //                    log_e("I2S internal mutex give error");
    //            }
    }
//---------------------------------------------------------------------------
} // Mitov

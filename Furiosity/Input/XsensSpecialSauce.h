////////////////////////////////////////////////////////////////////////////////
//  XsensSpecialSauce.h
//  Furiosity
//
//  Created by Bojan Endrovski on 09/12/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Defines.h"

#ifdef ANDROID

#include "xkf3ce.h"

namespace Furiosity
{
    class InputManager;
    
    class XsensSpecialSauce
    {
    protected:
        InputManager&       inputManager;
        
        struct xkf3ce*		xkf3ceFitler;        
        
    public:
        
        XsensSpecialSauce(InputManager& inputManager);
        
        void UpdateSensors(float values[], uint64_t timestamp, int type);
    };
}

#endif
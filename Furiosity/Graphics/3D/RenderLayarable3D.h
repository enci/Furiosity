////////////////////////////////////////////////////////////////////////////////
//  RenderLayarable3D.h
//
//  Created by Bojan Endrovski on 04/01/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Defines.h"

namespace Furiosity
{
    class RenderLayarable3D
    {
    private:
        uint        layerFlags = 1;
        
    public:
        
        uint RenderLayer() const            { return layerFlags; }
        
        void SetLayerFlags(uint layerFlags) { this->layerFlags = layerFlags; }
        
        bool TestLayer(uint testLayer)      { return (layerFlags & testLayer) != 0; }                
    };
}
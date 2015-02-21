////////////////////////////////////////////////////////////////////////////////
//  CubeMap.h
//  Furiosity
//
//  Created by Bojan Endrovski on 20/12/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gl.h"

namespace Furiosity
{
    ///
    /// Currently only used as a base class for other projects that have specific
    /// needs for a cube map of their own.
    ///
    class CubeMap
    {
    public:
        GLuint cubeMapID;
        
        CubeMap();
    };
}
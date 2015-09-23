////////////////////////////////////////////////////////////////////////////////
//  AssimpTools.h
//  Furiosity
//
//  Created by Bojan Endrovski on 11/01/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#if USE_ASSIMP

// Fr
#include "Frmath.h"
#include "Color.h"

// Assimp output data structures
#include "scene.h"

namespace Furiosity
{
    namespace AssimpTools
    {
        Color ConvertColor(const aiColor3D& color);
    
        Matrix44 ConvertMatrix(const aiMatrix4x4& matrix);
    }
}

#endif

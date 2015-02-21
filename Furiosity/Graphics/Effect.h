////////////////////////////////////////////////////////////////////////////////
//  Effect.h
//  Furiosity
//
//  Created by Bojan Endrovski on 04/07/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// STL
#include <map>
#include <memory>

// Fr
#include "Resource.h"
#include "Frmath.h"
#include "Color.h"
#include "gl.h"

using std::string;
using std::weak_ptr;
using std::unique_ptr;

namespace Furiosity
{
    class Effect;

    ///
    /// Effect Basically a handle to a Shader
    ///
    class Effect : public ResourceHandle<Shader>
    {
    protected:

        /// Protects againts errors when editing shaders on the fly 
        //  ShaderParameter nonValidEffectParameter;
        
    public:
        
        /// Create new effect from a vertex shader file path and
        /// a pixel shader path.
        Effect(const string& vertexShaderPath,
               const string& pixelShaderPath);

    };
    
    
}


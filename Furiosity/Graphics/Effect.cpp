////////////////////////////////////////////////////////////////////////////////
//  Effect.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 04/07/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Effect.h"
#include "Shader.h"
#include "gl.h"

using namespace Furiosity;
using namespace std;

Effect::Effect(const string& vertexShaderPath,
               const string& pixelShaderPath) :
    ResourceHandle(gResourceManager.LoadShader(vertexShaderPath, pixelShaderPath))
{
}
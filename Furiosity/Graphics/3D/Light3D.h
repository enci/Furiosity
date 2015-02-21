////////////////////////////////////////////////////////////////////////////////
//  Light3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 15/12/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Entity3D.h"
#include "Color.h"

namespace Furiosity
{
    class Light3D : public Entity3D
    {
    public:
        
        Color diffuse   = Color::White;
        
        Color specular  = Color::White;
        
        Light3D(World3D* world, Entity3D* parent) : Entity3D(world, parent, -1) {}
    };
}
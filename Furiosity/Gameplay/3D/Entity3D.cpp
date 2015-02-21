////////////////////////////////////////////////////////////////////////////////
//  Entity3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/17/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Entity3D.h"
#include "DebugDraw3D.h"

using namespace Furiosity;

Entity3D::Entity3D(World3D* world, Entity3D* parent, float radius) :
    radius(radius),
    world3D(world)
{
    if(parent != nullptr)
    {
        this->parent = parent;
        parent->childern.push_back(this);
    }
}

void Entity3D::Update(float dt)
{
    UpdateTransform();
}

void Entity3D::LoadFromXml(const XMLElement& settings)
{
    settings.QueryFloatAttribute("boundingRadius", &radius);
}

void Entity3D::DebugRender()
{
    if(radius > 0)
        gDebugDraw3D.AddSphere(Position(), radius);
}
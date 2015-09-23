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

void Entity3D::SetEnabled(bool enabled, bool recursive)
{
    this->enabled = enabled;
    
    if(recursive)
        for (auto c : childern)
            c->SetEnabled(enabled, recursive);
}

void Entity3D::LoadFromXml(const XMLElement& settings)
{
    settings.QueryFloatAttribute("boundingRadius", &radius);
}

// Find with linear search
Entity3D* Entity3D::FindChild(const string& name)
{
    for (auto c : childern)
        if(c->Name() == name)
            return c;
    
    return nullptr;
}

void Entity3D::DebugRender()
{
    if(radius > 0)
        gDebugDraw3D.AddSphere(Position(), radius);
}
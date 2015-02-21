////////////////////////////////////////////////////////////////////////////////
//  Vehicle2D.cpp
//
//  Created by Bojan Endrovski on 20/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Vehicle2D.h"
#include "Frmath.h"
#include "Furiosity.h"
#include "DebugDraw2D.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Many arguments constritor
////////////////////////////////////////////////////////////////////////////////
Vehicle2D::Vehicle2D(GameWorld* world,
                 const Vector2& position,
                 float radius,
                 float mass,
                 float maxSpeed,
                 float maxForce,
                 float maxTurnRate) :
    DynamicEntity2D(position, radius, mass, maxSpeed, maxForce, maxTurnRate),
    headingProcessor(1)
{
    this->gameWorld = world;
    
#ifdef DEBUG
    this->dbgColor = Color::Red;            // default to red
#endif    

    this->steering = new SteeringBehavior(this);
}

////////////////////////////////////////////////////////////////////////////////
// Ctor from xml stettings, this need to point inside the element and
// not on it's root. This alows for the ctor to be used for settings and
// streaming at the same time.
////////////////////////////////////////////////////////////////////////////////
Vehicle2D::Vehicle2D(GameWorld* world, const XMLElement* settings) :
    DynamicEntity2D(settings),
    headingProcessor(1)
{
    this->gameWorld = world;
    
#ifdef DEBUG
    this->dbgColor = Color::Red;            // default to red
#endif    

    steering = new SteeringBehavior(this);
}

////////////////////////////////////////////////////////////////////////////////
// Dtor - Free up some stuff
////////////////////////////////////////////////////////////////////////////////
Vehicle2D::~Vehicle2D()
{
    delete steering;
}

////////////////////////////////////////////////////////////////////////////////
// Upadte method, moves the vehicle 
////////////////////////////////////////////////////////////////////////////////
void Vehicle2D::Update(float dt)
{   
    force += steering->Calculate();
    
    DynamicEntity2D::Update(dt);
}

#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
// Debug Render
////////////////////////////////////////////////////////////////////////////////
void Vehicle2D::DebugRender(Color c)
{    
    Vector2 c0(0, BoundingRadius());
    Vector2 c1(-0.707107f * BoundingRadius(), -0.707107f * BoundingRadius());
    Vector2 c2(0.707107f * BoundingRadius(), -0.707107f * BoundingRadius());
    //
    transform.TransformVector2(c0);
    transform.TransformVector2(c1);
    transform.TransformVector2(c2);
    //
    gDebugDraw2D.AddLine(c1, c2, c);
    gDebugDraw2D.AddLine(c1, c0, c);
    gDebugDraw2D.AddLine(c2, c0, c);
}
#endif

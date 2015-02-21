////////////////////////////////////////////////////////////////////////////////
//  DynamicEntity3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/17/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "DynamicEntity3D.h"
#include "DebugDraw3D.h"


using namespace Furiosity;

DynamicEntity3D::DynamicEntity3D(World3D* world, Entity3D* parent, float radius) :
    Entity3D(world, parent, radius),
    maxForce(FLT_MAX),
    maxSpeed(FLT_MAX),
    linearDamping(0.98f),
    inverseMass(1.0f)
{}

void DynamicEntity3D::LoadFromXml(const XMLElement& settings)
{
    // Load bounding radius
    Entity3D::LoadFromXml(settings);
    
    // Params
    settings.QueryFloatAttribute("maxSpeed", &maxSpeed);
    settings.QueryFloatAttribute("maxForce", &maxForce);
    settings.QueryFloatAttribute("linearDamping", &linearDamping);
    //
    // Mass
    float mass;
    settings.QueryFloatAttribute("mass", &mass);
    inverseMass = 1 / mass;
}

void DynamicEntity3D::Update(float dt)
{
    // Trim max force
    force.Trim(maxForce);
    
    // gDebugDraw2D.AddLine(Position(), Position() + force, Color::Orange);
    
    // Acceleration = Force/Mass
    Vector3 acceleration = force * 1.0f; // inverseMass;
    
    // update velocity
    velocity += acceleration * dt;
    
    // Add fake linear damping
    velocity -= velocity * linearDamping;
    
    // make sure vehicle does not exceed maximum velocity
    velocity.Trim(maxSpeed);
    
    // clear the accumulator for the next frame
    force.Clear();
    
    //update the heading if the vehicle has a non zero velocity
    if(velocity.SquareMagnitude() > 0.001f)
    {
        // update the position
        Vector3 position = Position();
        position += velocity * dt;
        SetPosition(position);
        
        /*
        //        if(maxTurnRate > 0.0f)
        {
            // Set local coordinate frame
            Vector3 fwd = velocity;
            fwd.Normalize();
            //
            Vector3 up(0, 1, 0);
            // Vector3 up = position;
            // up.Normalize();
            Vector3 side    = fwd % up;    // Cross
            up              = side % fwd;
            //
            _trns.SetOrientation(side, up, fwd);
        }
         */
    }
    
#ifdef DEBUG
    gDebugDraw3D.AddAxis(Transform(), 0.3f);
    gDebugDraw3D.AddLine(Position(), Position() + force);
#endif
}
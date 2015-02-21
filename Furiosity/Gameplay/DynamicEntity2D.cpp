////////////////////////////////////////////////////////////////////////////////
//  DynamicEntity2D.cpp
//
//  Created by Bojan Endrovski on 20/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "DynamicEntity2D.h"
#include "Furiosity.h"
#include "DebugDraw2D.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Ctor 
////////////////////////////////////////////////////////////////////////////////
DynamicEntity2D::DynamicEntity2D()
: Entity2D() {}


////////////////////////////////////////////////////////////////////////////////
// Ctor 
////////////////////////////////////////////////////////////////////////////////
DynamicEntity2D::DynamicEntity2D(const Vector2& position, float radius, float mass,
                           float maxSpeed, float maxForce, float maxTurnRate)
:   Entity2D(position, radius),
    maxSpeed(maxSpeed),
    maxForce(maxForce),
    maxTurnRate(maxTurnRate),
    velocity(0.0f, 0.0f),
    linearDamping(0.998f)
{
    inverseMass = 1 / mass;
}


////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
DynamicEntity2D::DynamicEntity2D(const Vector2& position,
                           const Vector2& size,
                           float mass,
                           float maxSpeed,
                           float maxForce,
                           float maxTurnRate)
:   Entity2D(position, new Box(&transform, size.x, size.y)),
    maxSpeed(maxSpeed),
    maxForce(maxForce),
    maxTurnRate(maxTurnRate),
    velocity(0.0f, 0.0f),
    linearDamping(0.998f)
{
    inverseMass = 1 / mass;
}


////////////////////////////////////////////////////////////////////////////////
// Ctor from xml stettings, this need to point inside the element and
// not on it's root. This alows for the ctor to be used for settings and
// streaming at the same time.
////////////////////////////////////////////////////////////////////////////////
DynamicEntity2D::DynamicEntity2D(const XMLElement* settings)
:   Entity2D(settings),
    velocity(0,0),
    linearDamping(0.98f),
    maxTurnRate(Pi),
    maxForce(MAXFLOAT),
    maxSpeed(MAXFLOAT)
{
    // Max force
    const char* pMaxForce = settings->Attribute("maxForce");
    if(pMaxForce) maxForce = atof(pMaxForce);

    // Max speed
    const char* pMaxSpeed = settings->Attribute("maxSpeed");
    if(pMaxSpeed) maxSpeed = atof(pMaxSpeed);
    
    // Max turn rate
    const char* pMaxTurnRate = settings->Attribute("maxTurnRate");
    if(pMaxTurnRate) maxTurnRate = atof(pMaxTurnRate);
}


////////////////////////////////////////////////////////////////////////////////
// Super simple move
////////////////////////////////////////////////////////////////////////////////
void DynamicEntity2D::Update(float dt)
{
    // Trim max force
    force.Trim(maxForce);
    
    // gDebugDraw2D.AddLine(Position(), Position() + force, Color::Orange);
    
    // Acceleration = Force/Mass
    Vector2 acceleration = force * inverseMass;
    
    // update velocity
    velocity += acceleration * dt;
    
    // Add very fake linear vamping
    velocity *= linearDamping;
    //velocity -= velocity * (1-linearDamping);
    
    // make sure vehicle does not exceed maximum velocity
    velocity.Trim(maxSpeed);
    
    // clear the accumulator for the next frame
    force.Clear();
            
    //update the heading if the vehicle has a non zero velocity
    if(velocity.SquareMagnitude() > 0.1f)
    {
        // update the position
        Vector2 position = transform.Translation();
        position += velocity * dt;
        transform.SetTranslation(position);
        
        if(maxTurnRate > 0.0f)
        {
            // Set local coordinate frame
            Vector2 fwd = velocity.Unit();  
            Vector2 side = fwd.Perpendicular();
            //
            transform.SetOrientation(fwd, side);
        }
    }
    
    
    /*
    if (force.SquareMagnitude() > 0.001)
    {                
        // Acceleration = Force/Mass
        Vector2 acceleration = force * inverseMass;
        
        // update velocity
        velocity += acceleration * dt;
        
        // Add very fake linear vamping
        velocity *=linearDamping;
        
        // make sure vehicle does not exceed maximum velocity
        velocity.Trim(maxSpeed);
        
        // update the position
        Vector2 position = transform.Translation();
        position += velocity * dt;
        transform.SetTranslation(position);
        
        force.Clear();
    }
    else
    {
        Vector2 pos = Position();
        velocity *= linearDamping;
        pos += velocity * dt;
        transform.SetTranslation(pos);
        
        //update the heading if the vehicle has a non zero velocity
        if(velocity.SquareMagnitude() > 2.0f)
        {
            // Set local coordinate frame
            Vector2 fwd = velocity.Unit();  
            Vector2 side = fwd.Perpendicular();
            //
            transform.SetOrientation(fwd, side);        
        }
    }
     */
};

#ifdef DEBUG
void DynamicEntity2D::DebugRender(Color c)
{
    Entity2D::DebugRender(c);
    /*
    if(collisionShape == Disk)
    {
        Entity2D::DebugRender();
    }
    //else
    {
        
        // Create a transformation matrix
        Matrix33 transform;
        transform.SetIdentity();
        transform.SetOrientation(heading, side);
        transform.SetTranslation(position);
        
         
        // Create four vertices 
        Vector2 A(-dimensions.x, -dimensions.y);
        Vector2 B(dimensions.x, -dimensions.y);
        Vector2 C(dimensions.x, dimensions.y);
        Vector2 D(-dimensions.x, dimensions.y);
        
        // Transform vectors
        transform.TransformVector2(A);
        transform.TransformVector2(B);
        transform.TransformVector2(C);
        transform.TransformVector2(D);
        
        gDebugDraw2D.AddLine(A, B, Color::Red);
        gDebugDraw2D.AddLine(B, C, Color::Red);
        gDebugDraw2D.AddLine(C, D, Color::Red);
        gDebugDraw2D.AddLine(D, A, Color::Red);
    }
     */
}
#endif






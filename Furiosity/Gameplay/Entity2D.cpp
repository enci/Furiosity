////////////////////////////////////////////////////////////////////////////////
//  BaseGameEnity.cpp
//
//  Created by Bojan Endrovski on 26/09/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include <cassert>

#include "Entity2D.h"
#include "DebugDraw2D.h"

using namespace Furiosity;


////////////////////////////////////////////////////////////////////////////////
// Default ctor
////////////////////////////////////////////////////////////////////////////////
Entity2D::Entity2D()  :
    inverseMass(0.0f),
    Transformable(transform)
{
    transform.SetIdentity();
    //
    collisionShape = new CollisionNone(&transform);
}

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
Entity2D::Entity2D(uint ID) :
    Entity(ID),
    Transformable(transform)
{
    //    SetID(ID);
    transform.SetIdentity();
    // Always last
    collisionShape = new CollisionNone(&transform);
}


////////////////////////////////////////////////////////////////////////////////
// Disk ctor
////////////////////////////////////////////////////////////////////////////////
Entity2D::Entity2D(const Vector2& pos, float radius)  :
//    Entity(nextValidID),
    inverseMass(0.0f),
    Transformable(transform)
{
    transform.SetIdentity();
    transform.SetTranslation(pos);
    //
    // Always last
    if(radius > 0)
        collisionShape = new Disk(&transform, radius);
    else
        collisionShape = new CollisionNone(&transform);
}


////////////////////////////////////////////////////////////////////////////////
// Ctor from an explicit collision shape
////////////////////////////////////////////////////////////////////////////////
Entity2D::Entity2D(const Vector2& pos, CollisionShape* chape) :
    inverseMass(0.0f),
    Transformable(transform)
{
    transform.SetIdentity();
    transform.SetTranslation(pos);
    //
    // Always last
    collisionShape = chape;
    collisionShape->Transform = &transform;
}


////////////////////////////////////////////////////////////////////////////////
// Ctor from xml stettings, this need to point inside the element and
// not on it's root. This alows for the ctor to be used for settings and
// streaming at the same time.
////////////////////////////////////////////////////////////////////////////////
Entity2D::Entity2D(const XMLElement* settings) :
    Entity(settings),
    Transformable(transform)
{
    //                      Transform
    // Init transformation
    transform.SetIdentity();
    // Need this for loading levels
    const XMLElement* pTransformEl = settings->FirstChildElement("transform");
    if(pTransformEl)
        transform = Matrix33(pTransformEl);
    
    //                      Mass
    // Handle mass - init to a immovable object
    inverseMass = 0.0f;
    //
    // First try with mass attribute
    const char* pMass = settings->Attribute("mass");
    if(pMass) inverseMass = 1.0f / atof(pMass);
    //
    // Inverse mass can override this
    const char* pInvMass = settings->Attribute("inverseMass");
    // Set to infinite mass if not set
    if(pInvMass) inverseMass = atof(pInvMass);
    
    //                      Collision
    // Set collision shape
    const XMLElement* collisionEl = settings->FirstChildElement("collision");
    const char* pRadius = settings->Attribute("radius");
    //
    if(collisionEl)
        collisionShape = CollisionShape::Create(&transform, collisionEl);
    else if(pRadius)
        collisionShape = new Disk(&transform, atof(pRadius));
    else        
        collisionShape = new CollisionNone(&transform);
    
    /*
    //                      Render Layer
    const char* pLayer = settings->Attribute("layer");
    if (pLayer)
        renderLayer = atof(pLayer);
    else
        renderLayer = 0;
     */
}


////////////////////////////////////////////////////////////////////////////////
// BoundingRadius
////////////////////////////////////////////////////////////////////////////////
float Entity2D::BoundingRadius() const
{
    if (collisionShape) 
        return collisionShape->Radius;
    //
    return 0.0f;
}


#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
// Debug Render
////////////////////////////////////////////////////////////////////////////////
void Entity2D::DebugRender(Color c) const
{
    if(collisionShape)
        collisionShape->DebugRender(c);
}
#endif

// end // 
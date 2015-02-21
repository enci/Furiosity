////////////////////////////////////////////////////////////////////////////////
//  Collision3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/19/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Collision3D.h"

using namespace std;
using namespace Furiosity;

Collidable3D::Collidable3D(CollisionManager3D& manager,
                            Entity&             entity,
                            float radius) :
    radius(radius),
    entity(entity),
    manager(manager)
{
    
}


void CollisionManager3D::Detect()
{
    for (int i = 0; i < colliders.size(); i++)
    {
        for (int  j = i + 1; colliders.size(); j++)
        {
            Collidable3D& c0 = *colliders[i];
            Collidable3D& c1 = *colliders[j];
            float dist = (c0.Position() - c1.Position()).Magnitude();
            if( dist < (c0.BoundingRadius() + c1.BoundingRadius()) )
            {
                Vector3 pos = (c0.Position() + c1.Position()) * 0.5f;
                
                // if(c0.entity.)
                // handler.
                HandleCollision(c0.entity, c1.entity, pos);
            }
        }
    }
}

void CollisionManager3D::Add(Collidable3D *collider)
{
    colliders.push_back(collider);
}

void CollisionManager3D::Remove(Collidable3D *collider)
{
    vector<Collidable3D*>::iterator idx;
    for (idx = colliders.begin(); idx != colliders.end(); ++idx)
    {
        if (*idx == collider)
        {
            colliders.erase(idx);
            break;
        }
    }
}


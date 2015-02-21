////////////////////////////////////////////////////////////////////////////////
//  Collision3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/19/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "Vector3.h"
#include "Transformable3D.h"
#include "Entity.h"

namespace Furiosity
{
    /*
    /// A simple sphere in 3D
    struct Sphere
    {
        /// Sphere's center
        Vector3 Center;
        
        /// Sphere's radius
        float Raduis;
    };
    */
    
    class CollisionManager3D;
    
    /*
    class Collision3DHandler
    {
    public:
        virtual void HandleCollision(Entity& entity0, Entity& entity1, const Vector3& position) = 0;
    };
     */
    
    class Collidable3D : public Transformable3D
    {
        friend class CollisionManager3D;
        
    protected:
        CollisionManager3D& manager;
        
        Entity&             entity;
        
        float               radius;
        
    public:
        Collidable3D(CollisionManager3D& manager,
                     Entity&             entity,
                     float radius);
        
        ~Collidable3D();
        
        /// Gets the bounding radius
        float BoundingRadius() const { return radius; }
//        void SetBoundingRadius() const { return radius; }
    };
    
    
    class CollisionManager3D
    {
    protected:
        /// Single hanlder at this point
        // Collision3DHandler&         handler;
        
        std::vector<Collidable3D*> colliders;
        
    public:
        CollisionManager3D(/* Collision3DHandler& handler */) /*: handler(handler)*/ {}
        ~CollisionManager3D() {}
        
        void Detect();
        
        void Add(Collidable3D* collider);
        void Remove(Collidable3D* collider);
        
        virtual void HandleCollision(Entity& entity0, Entity& entity1, const Vector3& position) = 0;
    };
}

////////////////////////////////////////////////////////////////////////////////
//  CollisionShapes.h 
//  A collection of collision shapes to be used for collision checking.
//  They need to attached to a entity to make sense.
//
//  Created by Bojan Endrovski on 9/21/11.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Frmath.h"
#include "Color.h"
#include <cassert>

#include <vector>

namespace Furiosity
{
    enum CollisionShapeEnum
    {
        COLLISION_SHAPE_NONE,
        COLLISION_SHAPE_DISK,
        COLLISION_SHAPE_BOX,
        COLLISION_SHAPE_POLYLINE
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // A base class for all collision shapes, It also acts as a bouding volume
    // allowing for early outs.
    ////////////////////////////////////////////////////////////////////////////////
    class CollisionShape
    {
    public:
        /// 
        CollisionShapeEnum ShapeEnum;
        
        /// A reference to the transform of the body that owns the 
        const Matrix33*   Transform;
        
        /// A bouding radius of the shape
        float       Radius;
        
#ifdef DEBUG
        /// Mechanics to render this shape
        virtual void DebugRender(Color c = Color::Red) {}
#endif
        
    protected:
        /// Protected ctor making this class abstract
        CollisionShape(const Matrix33* transform, float r, CollisionShapeEnum shape)
            : Transform(transform), Radius(r), ShapeEnum(shape) {}
        
    public:
        /// Virtual destructor. This makes this class "polymorphic" and allows
        /// usage of dynamic_cast.
        virtual ~CollisionShape() {}
        
        /// Can create any of the shapes based on an Xml description
        /// Factory style
        static CollisionShape* Create(const Matrix33 *transform, const XMLElement* settings);
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Used for level geometry rather than entities
    ////////////////////////////////////////////////////////////////////////////////
    class LineSegment
    {
    public:
        Vector2 A;
        Vector2 B;
        
    public:
        LineSegment(const Vector2& a, const Vector2&b) : A(a), B(b) {}
        
        //        Vector2 Center() { return }
        
#ifdef DEBUG
        virtual void DebugRender(Color c = Color::Red);
#endif
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // A shape that might need to go away
    ////////////////////////////////////////////////////////////////////////////////
    class CollisionNone : public CollisionShape
    {
    public:
        CollisionNone(const Matrix33* transform)
            : CollisionShape(transform, 0, COLLISION_SHAPE_NONE) {}
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // The simplest shape is in fact the same as the bouding volume
    ////////////////////////////////////////////////////////////////////////////////
    class Disk : public CollisionShape
    {
    public:
        /// Ctor
        Disk(const Matrix33* transform, float r)
            : CollisionShape(transform, r, COLLISION_SHAPE_DISK) {}
        
#ifdef DEBUG
        virtual void DebugRender(Color c = Color::Red);
#endif        
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Place holder
    ////////////////////////////////////////////////////////////////////////////////
    class Box : public CollisionShape
    {
    public:
        float halfWidth;
        float halfHeight;
        
    public:
        Box(const Matrix33* transform, float width, float height);
        
#ifdef DEBUG
        virtual void DebugRender(Color c = Color::Red);
#endif
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    ////////////////////////////////////////////////////////////////////////////////
    class Polyline : public CollisionShape
    {
    public:
        std::vector<LineSegment> lines;
    
    public:
        Polyline(const Matrix33* transform, const std::vector<Vector2>& points);
                
#ifdef DEBUG
        virtual void DebugRender(Color c = Color::Red);
#endif
    };
}


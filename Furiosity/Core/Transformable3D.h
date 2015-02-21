////////////////////////////////////////////////////////////////////////////////
//  Transformable3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/16/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix44.h"

#include <vector>

namespace Furiosity
{
    /*
    class Transformable3D;
    
    
    ///
    /// Data - Used by the entities mostly via the
    ///
    struct Transform3D
    {
        friend Transformable3D;
        
    protected:
        
        Matrix44    local;
        
        Matrix44    world;
        
        Transform3D* parent = nullptr;
        
        std::vector<Transform3D*> childern;
        
    public:
        
        inline void UpdateTransform()
        {
            if(parent)
                world = local * parent->world;
            else
                world = local;
            
            for(auto c : childern)
                c->UpdateTransform();
        }
        
    };
    */
    
    /*
    
    ///
    ///
    ///
    class Transformable3D
    {
    protected:
        Matrix44    _trns;
        
        Vector3     _scale;
        
    public:
        Transformable3D() : _trns(), _scale(1, 1, 1) {}
        
        inline Matrix44 Transform() const { return _trns * Matrix44::CreateScale(_scale); }
        
        inline Matrix33 Orientation() const { return _trns.GetMatrix33(); }
        
        
        inline void SetTransform(const Matrix44& mat)
        {
            // Extract the orientation vectors
            Vector3 x(_trns.m00, _trns.m01, _trns.m03);
            Vector3 y(_trns.m10, _trns.m11, _trns.m13);
            Vector3 z(_trns.m20, _trns.m21, _trns.m23);
            
            // Get their scale
            _scale.x = x.Magnitude();
            _scale.y = y.Magnitude();
            _scale.z = z.Magnitude();
            
            // Normalize the orientation vectors
            x *= 1.0f / _scale.x;
            y *= 1.0f / _scale.y;
            z *= 1.0f / _scale.z;
            
            // Set the orientation and transaltion
            _trns.SetOrientation(x, y, z);
            _trns.SetTranslation(mat.Translation());
        }
        
        inline Vector3 Position() const { return _trns.Translation(); }
        
        inline void SetPosition(Vector3 position) { _trns.SetTranslation(position); }
        
        inline Vector3 Scale() const { return _scale; }
        
        inline void SetScale(Vector3 scale) { _scale = scale; }
        
        inline void SetOrientation(const Vector3& x,
                                   const Vector3& y,
                                   const Vector3& z)
        { _trns.SetOrientation(x, y, z); }
    };
    */
    
    /*
    
    class Transformable3D
    {
    protected:
        Transform3D transform;
        
        void SetParent(Transformable3D* parent)
        {
            transform.parent = parent->transform;
            transform.parent->childern.push_back(&transform);
        }
        
    public:
        ///
        /// Create a new transformable object.
        ///
        Transformable3D() {}
        
        ///
        ///
        ///
        inline Matrix44 Transform() const { return transform.world; }
        
        ///
        ///
        ///
        inline Matrix33 Orientation() const { return transform.world.GetMatrix33(); }
        
        ///
        ///
        ///
        inline void SetTransformation(const Matrix44& trans)
        {
            transform.local = trans;
            transform.UpdateTransform();
        }
        
        ///
        ///
        ///
        inline Vector3 Position() const             { return transform.world.Translation(); }
        
        ///
        ///
        ///
        inline void SetPosition(Vector3 position)
        {
            if(!transform.parent)
            {
                transform.local.SetTranslation(position);
            }
            else
            {
                Vector3 wPos = transform.world.Translation();
                Vector3 lPos = transform.local.Translation();
                transform.local.SetTranslation(position + (wPos - lPos));
            }
            
            transform.UpdateTransform();
        }
        
        ///
        ///
        ///
        inline Vector3 LocalPosition() const        { return transform.world.Translation(); }
        
        
        ///
        ///
        ///
        inline void SetLocalPosition(Vector3 position)
        {
            transform.local.SetTranslation(position);
            transform.UpdateTransform();
        }
        
        ///
        /// Gets the local scale. Might be non uniform.
        ///
        inline Vector3 LocalScale() const
        {
            // Extract the orientation vectors
            const Matrix44& trns = transform.local;
            Vector3 x(trns.m00, trns.m01, trns.m02);
            Vector3 y(trns.m10, trns.m11, trns.m12);
            Vector3 z(trns.m20, trns.m21, trns.m22);
            
            // Get their scale
            return Vector3(x.Magnitude(), y.Magnitude(), z.Magnitude());
        }
        
        ///
        /// Set the local scale. Might accumulate errors over time.
        ///
        inline void SetLocalScale(Vector3 scale)
        {
            Vector3 currentScale = LocalScale();
            Vector3 relativeScale(scale.x / currentScale.x,
                                  scale.y / currentScale.y,
                                  scale.z / currentScale.z);
            transform.local = transform.local * Matrix44::CreateScale(relativeScale);
            transform.UpdateTransform();
        }
        
        ///
        /// Set the local orientation. 
        ///
        inline void SetLocalOrientation(const Vector3& x,
                                        const Vector3& y,
                                        const Vector3& z)
        {
            Vector3 scale = LocalScale();
            transform.local.SetOrientation(x * scale.x,
                                           y * scale.y,
                                           z * scale.z);
            transform.UpdateTransform();
        }
    };
    */
}

////////////////////////////////////////////////////////////////////////////////
//  Entity3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/17/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Entity.h"
#include "Frmath.h"
#include "Xml.h"
//#include "Transformable3D.h"

namespace Furiosity
{
    class World3D;
    
    ///
    /// Entity3D - code and documentation are WIP on this
    ///
    class Entity3D : public Entity /*, public Transformable3D */
    {
    protected:
        
        //friend class World3D;
        
        float                   radius;
        
        Matrix44                localTrns;
        
        Matrix44                worldTrns;
        
        Entity3D*               parent      = nullptr;
        
        std::vector<Entity3D*>  childern;

        World3D*                world3D     = nullptr;
        
    public:
        Entity3D(World3D* world, Entity3D* parent, float radius);
        
        ~Entity3D() {}
        
        virtual void Update(float dt) override;
        
        virtual void LoadFromXml(const XMLElement& settings);
        
        /// Gets the bounding radius
        float BoundingRadius() const { return radius; }
        
        void SetBoundingRadius(float r) { radius = r; }
        
        /// Begin iterator for ranged loops
        decltype(childern.begin())  begin() { return childern.begin();  }
        
        /// End iterator for ranged loops
        decltype(childern.end())    end()   { return childern.end();    }
        
        /// Do debug rendering of this entity
        virtual void DebugRender();
        
        /// Update the trasform of the entity
        inline void UpdateTransform()
        {
            if(parent)
                //worldTrns = localTrns * parent->worldTrns;
                worldTrns =  parent->worldTrns * localTrns;
            else
                worldTrns = localTrns;
            
            for(auto c : childern)
                c->UpdateTransform();
        }
        
        /// Get the world transormation of this entity
        inline Matrix44 Transform() const { return worldTrns; }
        
        /// Get only the orientation part of the world matrix
        inline Matrix33 Orientation() const { return worldTrns.GetMatrix33(); }
        
        /// Get the world position
        inline Vector3 Position() const { return worldTrns.Translation(); }

        /// Set the local transformation
        inline void SetTransformation(const Matrix44& trans)
        {
            localTrns = trans;
            UpdateTransform();
        }
        
        ///
        ///
        ///
        inline void SetPosition(Vector3 position)
        {
            if(!parent)
            {
                localTrns.SetTranslation(position);
            }
            else
            {
                Vector3 wPos = worldTrns.Translation();
                Vector3 lPos = localTrns.Translation();
                localTrns.SetTranslation(position + (wPos - lPos));
            }
            
            UpdateTransform();
        }
        
        /// Get the local position
        inline Vector3 LocalPosition() const
        {
            return worldTrns.Translation();
        }
        
        
        ///
        inline void SetLocalPosition(Vector3 position)
        {
            localTrns.SetTranslation(position);
            UpdateTransform();
        }
        
        /// Gets the local scale. Might be non uniform.
        inline Vector3 LocalScale() const
        {
            // Extract the orientation vectors
            const Matrix44& trns = localTrns;
            Vector3 x(trns.m00, trns.m01, trns.m02);
            Vector3 y(trns.m10, trns.m11, trns.m12);
            Vector3 z(trns.m20, trns.m21, trns.m22);
            
            // Get their scale
            return Vector3(x.Magnitude(), y.Magnitude(), z.Magnitude());
        }
        
        /// Set the local scale. Might accumulate errors over time.
        inline void SetLocalScale(Vector3 scale)
        {
            Vector3 currentScale = LocalScale();
            Vector3 relativeScale(scale.x / currentScale.x,
                                  scale.y / currentScale.y,
                                  scale.z / currentScale.z);
            localTrns = localTrns * Matrix44::CreateScale(relativeScale);
            UpdateTransform();
        }
        
        /// Set the local orientation.
        inline void SetLocalOrientation(const Vector3& x,
                                        const Vector3& y,
                                        const Vector3& z)
        {
            Vector3 scale = LocalScale();
            localTrns.SetOrientation(x * scale.x,
                                     y * scale.y,
                                     z * scale.z);
            UpdateTransform();
        }
    };
}
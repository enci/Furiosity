////////////////////////////////////////////////////////////////////////////////
//  DynamicEntity3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/17/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Entity3D.h"

namespace Furiosity
{
    class DynamicEntity3D : public Entity3D
    {
    protected:
        
        /// Like mass, but then inverse
        float inverseMass;
        
        /// Imitates linear damping
        float linearDamping;
        
        /// Max speed for this entity
        float maxSpeed;
        
        /// In case we add some force stuff here
        float maxForce;
        
        /// The velicty of this entity
        Vector3 velocity;
        
        /// Force accumulator
        Vector3 force;
        
    public:
        
        DynamicEntity3D(World3D* world, Entity3D* parent = nullptr, float radius = -1.0f);
        
        virtual ~DynamicEntity3D() {}
        
        virtual void LoadFromXml(const XMLElement& settings) override;
        
        virtual void Update(float dt) override;
        
        /// Velocity
        Vector3  Velocity() const                       { return velocity;  }
        void     SetVelocity(const Vector3& vel)        { velocity = vel;   }
        float    Speed() const                          { return velocity.Magnitude(); }
        
        /// Linear damping
        float   LinearDaming() const            { return linearDamping;     }
        void    SetLinearDaming(float damping)  { linearDamping = damping;  }
        
        /// Max speed
        float   MaxSpeed() const                { return maxSpeed;  }
        void    SetMaxSpeed(float speed)        { maxSpeed = speed; }
        
        /// Max force
        float   MaxForce() const                { return maxForce;  }
        void    SetMaxForce(float force)        { maxForce = force; }
        
        /// Add force to the accumulator
        void    AddForce(const Vector3& f)      { force += f; }
        
        /// Mass access methods
        float   Mass() const                    { return 1 / inverseMass;       }
        void    SetMass(float m)                { inverseMass = 1 / m;          }
        //
        float   InverseMass() const             { return inverseMass;           }
        void    SetInverseMass(float m)         { inverseMass = m;              }
        //
        bool    HasInifitesMass()               { return inverseMass == 0.0f;   }
    };
}
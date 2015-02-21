////////////////////////////////////////////////////////////////////////////////
//  DynamicEntity2D.h
//
//  Created by Bojan Endrovski on 20/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma  once

#include "Entity2D.h"

namespace Furiosity
{
    ///
    /// Moving Entity
    /// Like an entity, but then it moves. Uses simple point mass physics
    ///
    class DynamicEntity2D : public Entity2D
    {
    protected:
        /// The velicty of this entity
        Vector2 velocity;
        
        /// Imitates linear damping
        float linearDamping;
        
        /// Max speed for this vehicle
        float maxSpeed;
        
        /// In case I add some force stuff here
        float maxForce;
        
        /// In case things take a turn
        float maxTurnRate;
        
        /// Force accumulator
        Vector2 force;
        
    public:
        
        /// Creates an empty DynamicEntity2D, maybe a streaming solution, NOT
        DynamicEntity2D();
        
        /// Crates a new dynamic entity with disk collision
        DynamicEntity2D(const Vector2& position,
                     float radius,
                     float mass         = 1.0f,
                     float maxSpeed     = FLT_MAX,
                     float maxForce     = FLT_MAX,
                     float maxTurnRate  = TwoPi);
        
        
        // Crates a new dynamic entity with box collision
        DynamicEntity2D(const Vector2& position,
                     const Vector2& size,
                     float mass,
                     float maxSpeed,
                     float maxForce,
                     float maxTurnRate);
        
        
        /// Ctor from xml stettings, this need to point inside the element and
        /// not on it's root. This alows for the ctor to be used for settings and
        /// streaming at the same time.
        DynamicEntity2D(const XMLElement* settings);
                 
        /// No dynamic memory or pointers to care about
        virtual ~DynamicEntity2D() { }
        
        /// Move it
        virtual void    Update(float dt);
        
#ifdef DEBUG        
		virtual void	DebugRender(Color c = Color::Red);
#endif	
        
        /// Velocity
        Vector2  Velocity() const                       { return velocity;  }
        void     SetVelocity(const Vector2& vel)        { velocity = vel;   }        
        float    Speed() const                          { return velocity.Magnitude(); }
        
        /// Read only property
        Vector2 Heading() const                 { return transform.Up();    }
        
        /// Read only property
        Vector2 Side() const                    { return transform.Right(); }

        /// Linear damping 
        float   LinearDaming() const            { return linearDamping;     }
        void    SetLinearDaming(float damping)  { linearDamping = damping;  }
        
        /// Max speed 
        float   MaxSpeed() const                { return maxSpeed;  }
        void    SetMaxSpeed(float speed)        { maxSpeed = speed; }
      
        /// Max force 
        float   MaxForce() const                { return maxForce;  }
        void    SetMaxForce(float force)        { maxForce = force; }      
        
        /// Max turn rate 
        float   MaxTurnRate() const             { return maxTurnRate;   }
        void    SetMaxTurnRate(float rate)      { maxTurnRate = rate;   }
        
        /// Add force to the accumulator
        void    AddForce(const Vector2& f)      { force += f; }
    };
}

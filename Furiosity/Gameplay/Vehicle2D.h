////////////////////////////////////////////////////////////////////////////////
//  Vehicle2D.h
//
//  Created by Bojan Endrovski on 20/03/2011.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector2.h"
#include "DynamicEntity2D.h"
#include "GameWorld.h"
#include "SteeringBehavior.h"
#include "Color.h"
#include "ValueSmoother.h"


namespace Furiosity
{
    // Forward declaration
    class SteeringBehavior;

    ///
    /// Vehicle2D
    /// Like moving entity but smart...ish
    ///
    class Vehicle2D : public DynamicEntity2D
    {
    public:

    protected:
        /// Pointer to the gameworld
        GameWorld* gameWorld;
        
        /// Pointer to the steering behavior
        SteeringBehavior* steering;
        
        /// Processor for the heading, smooting and such
        ValueSmoother<Vector2> headingProcessor;
                        
#ifdef DEBUG
    public:
        /// To indetify different vehicles
        Color dbgColor;
#endif
        
    public:
        /// Ctor with all the needed data, creates the behavior itself
        Vehicle2D(GameWorld* world,
                const Vector2& position,
                float radius,
                float mass              = 1.0f,
                float maxSpeed          = FLT_MAX,
                float maxForce          = FLT_MAX,
                float maxTurnRate       = TwoPi);
        
        /// Ctor from xml stettings, this need to point inside the element and
        /// not on it's root. This alows for the ctor to be used for settings and
        /// streaming at the same time.
        Vehicle2D(GameWorld* world, const XMLElement* settings);
        
        /// Dtor
        virtual ~Vehicle2D();
        
        /// Updates the steering and motion of the vehicle
        virtual void Update(float dt);
        
        /// Accessor for the steering
        SteeringBehavior* Steering()    { return steering; }    
        
        /// Accessor for the world
        GameWorld* World()              { return gameWorld; }
        
#ifdef DEBUG        
        virtual void DebugRender(Color c = Color::Red);
#endif
        
    };
    
}

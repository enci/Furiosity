////////////////////////////////////////////////////////////////////////////////
//  FXParticle.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/6/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "Color.h"
#include "FrMath.h"

namespace Furiosity
{
    ///
    /// Generic particle, base for both 2D and 3D particles
    ///
    template <class VectorT>
    class FXParticle
    {
    public:
        /// The position of this particle
        VectorT     Position;
        
        /// Size in pixels, implicitly relies on being right
        /// after the position.
        /// !!! Don't move it!   !!!
        float       Size;
        
        /// Start with this size
        float       BeginSize;
        
        /// End up with this size
        float       EndSize;
        
        
        /// Velocity (usually a constant per particle)
        VectorT     Velocity;
        
        /// Damping defines how fast the speed falls off
        /// faking dynamic friction. 0.98f suits fine
        float       Damping;
        
        /// Current color cached
        Color       CurrentColor;
        
        /// Start with this color
        Color       BeginColor;
        
        /// Interpolate to this color
        Color       EndColor;
        
        /// Normalized age in range [0,1]
        /// Used to interpolating values
        float       Age;
        
        /// Life span saved inverted to avod division
        float       OneOverLifeSpan;
        
    public:
        
        /// Creates a new default particle
        FXParticle() :
            Position(VectorT(FLT_MAX)),
            Velocity(VectorT()),
            Age(1.0f),                  // Born dead
            OneOverLifeSpan(1.0f),
            Damping(0.98f),
            //
            CurrentColor(Color::White),
            BeginColor(Color::White),
            EndColor(Color::White)
        {}
        
        /// Particle update
        /// This method is not virtual as that would not be good for speed.
        /// Also, particles are not expected to be accessed by pointer either.
        ///
        void Update(float dt)
        {
            Position += Velocity * dt;
            Velocity *= Damping;
            //
            Age += dt * OneOverLifeSpan;
            //
            CurrentColor.LerpColors(BeginColor, EndColor, Age);
            //
            Size = Lerp(BeginSize, EndSize, Age);
        }
        
        /// Check if this particle has expired
        bool        IsDead() const { return Age >= 1.0f; }
        
        /// Spawn a particle
        inline void Spawn(const VectorT& pos,
                          const VectorT& vel,
                          const Color& beginColor,
                          const Color& endColor,
                          float beginSize,
                          float endSize,
                          float lifeSpan)
        {
            Position        = pos;
            Velocity        = vel;
            //
            Age             = 0.0f;
            OneOverLifeSpan = 1.0f / lifeSpan;
            //
            BeginColor      = beginColor;
            EndColor        = endColor;
            CurrentColor    = beginColor;
            //
            BeginSize       = beginSize;
            EndSize         = endSize;
            Size            = beginSize;
        }
    };
}


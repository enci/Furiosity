////////////////////////////////////////////////////////////////////////////////
//  BaseFXParticles.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/6/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Color.h"

namespace Furiosity
{
    template <class ParticleT, class VectorT>
    class BaseFXParticleManager
    {
    protected:
        ushort          maxParticles;
        
        ParticleT*      particles       = nullptr;
        
        int*            freeSlots       = nullptr;
        
        int             nextFree;
        
        float           pixelScaling;
                
        BaseFXParticleManager(ushort max);
        
        virtual ~BaseFXParticleManager();
        
        void Init(ushort max);
        
    public:
        
        virtual void UpdateParticles(float dt);
        
        /// Add a new particle into this particle manager instance.
        virtual ushort AddParticle(const VectorT& position,
                                   const VectorT& velocity,
                                   const Color& beginColor,
                                   const Color& endColor,
                                   float beginSize,
                                   float endSize,
                                   float lifeSpan,
                                   float damping = 0.98);
        
        virtual void RemoveParticle(ushort index);
    };
}

// Templated implementation
#include "BaseFXParticles.hpp"

////////////////////////////////////////////////////////////////////////////////
//  BaseFXParticles.hpp - Templated implementation
//  Furiosity
//
//  Created by Bojan Endrovski on 10/6/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GeneralManager.h"

namespace Furiosity
{
    ////////////////////////////////////////////////////////////////////////////////
    // Ctor
    ////////////////////////////////////////////////////////////////////////////////
    template<class ParticleT, class VectorT>
    BaseFXParticleManager<ParticleT, VectorT>::BaseFXParticleManager(ushort max) :
        pixelScaling(gGeneralManager.GetDevice().GetPixelScale())
    {
        Init(max);
    }
    
    
    template<class ParticleT, class VectorT>
    void BaseFXParticleManager<ParticleT, VectorT>::Init(ushort max)
    {
        maxParticles = max;
        
        // Allocate particles store and slots
        if(particles) SafeDelete(particles);
        if(freeSlots) SafeDelete(freeSlots);
        
        // Allocate particles store and slots
        particles = new ParticleT[maxParticles];
        freeSlots = new int[maxParticles];
        
        // Init free slots to the next slot (i+1)
        for(ushort i = 0; i < maxParticles; i++)
        {
            freeSlots[i] = i+1;
            // particles[i].Position = VectorT(-FLT_MAX); // Put if off screen
        }
        
        // The last slot has no next free one
        freeSlots[maxParticles - 1] = -1;
        
        // Next free is 0, as there are no particles
        nextFree = 0;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Dtor
    ////////////////////////////////////////////////////////////////////////////////
    template<class ParticleT, class VectorT>
    BaseFXParticleManager<ParticleT, VectorT>::~BaseFXParticleManager()
    {
        SafeDeleteArray(particles);
        SafeDeleteArray(freeSlots);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Update
    ////////////////////////////////////////////////////////////////////////////////
    template<class ParticleT, class VectorT>
    void BaseFXParticleManager<ParticleT, VectorT>::UpdateParticles(float dt)
    {
        for (ushort i = 0; i < maxParticles; i++)
        {
            if (freeSlots[i] == -1) // Occupied, otherwise no valied particle there
            {
                if ( ! particles[i].IsDead())
                {
                    ParticleT& p = particles[i];
                    p.Update(dt);
                }
                else
                {
                    RemoveParticle(i);
                }
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // RemoveParticle
    ////////////////////////////////////////////////////////////////////////////////
    template<class ParticleT, class VectorT>
    void BaseFXParticleManager<ParticleT, VectorT>::RemoveParticle(ushort index)
    {
        particles[index].BeginSize  = 0;
        particles[index].EndSize    = 0;
        particles[index].Size       = 0;
        //
        particles[index].Position.x = -MAXFLOAT;
        particles[index].Position.y = -MAXFLOAT;
        //
        particles[index].BeginColor = Color::White;
        particles[index].EndColor = Color::White;
        particles[index].CurrentColor = Color::White;
        //
        // particles[index].Age        = 0;
        //
        freeSlots[index] = nextFree;    // Next free goes in new the empty slot
        nextFree = index;               // This index is immidiatelly free
    }

    ////////////////////////////////////////////////////////////////////////////////
    // AddParticle
    ////////////////////////////////////////////////////////////////////////////////
    template <class ParticleT, class VectorT>
    ushort BaseFXParticleManager<ParticleT, VectorT>::AddParticle(
        const VectorT& position,
        const VectorT& velocity,
        const Color& beginColor,
        const Color& endColor,
        float beginSize,
        float endSize,
        float lifeSpan,
        float damping)
    {
        // Check if there are empty slots
        if(nextFree < 0)
            return -1;
        //
        ushort idx = nextFree;                  // Get the next free slot
        nextFree = freeSlots[idx];              // Read what is the free slot after this
        freeSlots[idx] = -1;
        //
        particles[idx].Spawn(position,          // Pos
                             velocity,          // Vel
                             beginColor,        // Begin col
                             endColor,          // End col
                             beginSize,         // Begin size
                             endSize,           // End size
                             lifeSpan);         // Lifespan
        particles[idx].Damping  = damping;      // Set dampening
        //
        return idx;                             // Return index for book-keeping
    }
}
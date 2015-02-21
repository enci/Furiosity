////////////////////////////////////////////////////////////////////////////////
//  BaseAnimation.h
//  Furiosity
//
//  Created by Bojan Endrovski on 25/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Countable.h"
#include <memory>

namespace Furiosity
{
    ///
    /// This is a base for animation and it does not animate anything but provides
    /// timing functionality and few helper things that are shared among animations
    ///
    class Animation : public Countable<Animation>
    {
    protected:
        /// Duration of this animation in seconds
        float   duration;
        
        /// Current time of this animation in seconds
        float   time;
        
        /// Inverted durartion for faster updates
        float   invduration;
        
        /// A [0,1] normalized time interpolation value
        float   param;
        
        /// Flag for looping
        bool    loop;
        
    public:
        /// Create an empty animation. Can be used for waiting or more often
        /// to initialize the animation as a base class
        ///
        /// @param duration The duration of this animation
        Animation(float duration);
        
        ~Animation();
        
        /// Update the animation
        virtual void Update(float dt);
        
        /// Reset the animation
        virtual void Reset()        { time = 0.0f; }
        
        /// The duration of this animation
        float Duration() const      { return duration; }
        
        /// Returns true if this animation is done
        virtual bool IsDone() const { return time >= duration && !loop; }
        
        /// Set the value for looping
        void SetLoop(bool loop)     { this->loop = loop; }
        
        /// Return true if this is a looping animation
        bool Loop() const           { return loop; }
    };
    
    // Define a shared pointer to animations
    typedef std::shared_ptr<Animation> AnimationShrPtr;
}

////////////////////////////////////////////////////////////////////////////////
//  Animatable.h
//  Furiosity
//
//  Created by Bojan Endrovski on 27/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseAnimation.h"

// STL
#include <string>
#include <map>


namespace Furiosity
{
    ///
    /// Animatable gives a simple mechanisim to store and play any animations
    /// on any object that inherits the class. It can be instantiated too.
    ///
    class Animatable
    {
    protected:
        
        /// Collection of all the animations
        std::map<std::string, AnimationShrPtr>  animations;
        
        /// Current animation pointer
        AnimationShrPtr                         currAnimation;
        
        /// The name of the currently plying animation
        std::string                             currAnimationName;
        
    public:
        
        /// Create an invalid
        Animatable() : currAnimation(nullptr) {}
        
        /// Dtor - lets the shared pointers handle memory managing
        virtual ~Animatable() {}
        
        /// Add a new animation under this name. The name is used to play the
        /// the animation at a later time.
        virtual void AddAnimation(const std::string& name, AnimationShrPtr animation);
        
        /// Add a new animation under this name. The name is used to play the
        /// the animation at a later time. The object will take ownership of the animation
        virtual void AddAnimation(const std::string& name, Animation* animation);
        
        /// Get an animation with this name
        virtual AnimationShrPtr GetAnimation(const std::string& name);
        
        /// Remove an animation with this name. If the animamtion is playing
        /// it will not be stopped.
        virtual void RemoveAnimation(const std::string& name);
        
        /// Play an animation with this name. The current animation will be stopped.
        virtual void PlayAnimation(const std::string& name);
        
        /// Stop the currently playing animation.
        virtual void StopAnimation();
        
        /// Clear all added animations
        virtual void ClearAnimation();
        
        /// Returns true there is an animation with that name
        bool HasAnimation(const std::string& name) const;
        
        /// Returns true if there is a currently playing animation
        bool IsPlaying() const { return currAnimation.get() != 0; }
        
        /// Returns animation name
        std::string CurrentAnimationName() const { return currAnimationName; }
        
        /// Update the animation
        virtual void Update(float dt);
    };
}
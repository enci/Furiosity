////////////////////////////////////////////////////////////////////////////////
//  AnimationContainer.h
//  Furiosity
//
//  Created by Bojan Endrovski on 25/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <queue>
#include <string>
//#include <cstdarg>
#include <initializer_list>

#include "BaseAnimation.h"

using namespace std;

namespace Furiosity
{
    ///
    /// AnimationContainer is a generic abstract base class for collection of animations
    /// namely sequences and compositions.
    ///
    class AnimationContainer : public Animation
    {
    public:
        // Typedef
        typedef std::vector<AnimationShrPtr> AnimationColl;
        
    protected:
        
        AnimationColl animations;
        
    public:
        AnimationContainer() : Animation(-1) { duration = 0.0f; }
        
        AnimationContainer(initializer_list<Animation*> anims);
        
        virtual ~AnimationContainer() {}
        
        virtual void Update(float dt) override = 0;
        
        virtual void AddAnimation(AnimationShrPtr anim) { animations.push_back(anim); }
        
        virtual void Reset() override;
        
        virtual void Clear() { animations.clear(), duration = 0.0f; }
    };
    
    
    ///
    /// Animation sequence plays animation in a consecutive order
    ///
    class AnimationSequence : public AnimationContainer
    {
    protected:
        /// A queueu of animations to be played
        std::queue<AnimationShrPtr> animQueue;
        
    public:
        AnimationSequence() = default;
        
        AnimationSequence(initializer_list<Animation*> anims);
        
        virtual void Update(float dt) override;
        
        virtual void AddAnimation(AnimationShrPtr anim) override;
        
        /// Reset the animation sequence by playing from the beginning
        virtual void Reset() override;
        
        /// Returns when the last animation in the sequeces has finished playing
        virtual bool IsDone() const override { return animQueue.empty(); }
        
        /// Clear the animation sequence
        virtual void Clear() override { AnimationContainer::Clear(); animQueue = {}; }
    };
    
    
    ///
    /// Animation Composition plays animations simultaneously
    ///
    class AnimationComposition : public AnimationContainer
    {
    public:
        
        AnimationComposition() = default;
                
        AnimationComposition(initializer_list<Animation*> anims);
        
        virtual void Update(float dt);
        
        virtual void AddAnimation(AnimationShrPtr anim);
        
        /// Returns when the longest a animation in the composition has finished playing
        virtual bool IsDone() const;
    };
}
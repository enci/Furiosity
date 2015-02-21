////////////////////////////////////////////////////////////////////////////////
//  SpriteAnimator.h
//
//  Created by Bojan Endrovski on 10/29/11.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef SPRITEANIMATOR_H
#define SPRITEANIMATOR_H

#include "Frmath.h"

#include <vector>
#include <map>
#include <string>

using std::map;
using std::string;
using std::vector;

namespace Furiosity
{
    ///
    /// A very simple and modular approach to animating sprites. It does the
    /// animation calculations but none of the rendering, so it can used
    /// anywhere. A single object can have multiple Animators for different
    /// animated sprites.
    ///
    class SpriteAnimator
    {
    protected:
        
        // De
        struct Frame
        {
            Vector2 From;
            Vector2 To;
            Vector2 Size;
            Vector2 Offset;
        };
        
        typedef vector<Frame>       FramesSequence;
        
        string                      currentAnimationName;
        
        FramesSequence*             currentAnimation;
        
        Frame                       currentFrame;
        
        map<string, FramesSequence> animations;
        
        /// Frames per second
        int     fps;
        
        /// How long does a single frame last
        float   timePerFrame;
        
        /// The current time
        float   time;
        
        /// Current frame in the animation
        int     frameIndex;
  
        // State vars
        // int     currentAnimation;
        
        
        /// A flag for looping the animation
        bool    loop;
        
        /// A flag wheter this animation is done
        bool    isDone;        
        
    public:
        /// Default ctor, see cpp for defult values
        SpriteAnimator();
        
        /// Ctor, see cpp for defult values
        SpriteAnimator(const string& spriteSheet,
                       const string& animationSheet = "");
        
        /// Ctor, see cpp for defult values
        SpriteAnimator(const XMLElement& spriteSheet);

        
        /// Ctor, see cpp for defult values
        // SpriteAnimator(const XMLElement& spriteSheet,
        //               const XMLElement& animationSheet);
        
        // Dtor - nothing to do
        virtual ~SpriteAnimator() {}
        
        void InitWithGrid(int rows, int columns);
        
        /// Get FPS for this animation
        int     FramesPerSecond() const { return fps; }
        
        /// Set FPS for this animation
        void    SetFramesPerSecond(int fps);
        
        /// Animation
        //FramesSequence& CurrentAnimation() const     { return *currentAnimation; }
        
        // TODO: Remove if a better solution is found
        /// Set a named animation
        void    SetAnimation(const string& animationName, vector<int> frames);
        
        void    PlayAnimation(string name, bool loop = false);
        
        /// Check if the animation has been done
        bool    IsDone() const { return isDone; }
        
        /// Gets "from" UV coordinaes to be used for rendering
        const Vector2& UVFrom() const    { return currentFrame.From; }
        
        /// Gets "to" UV coordinaes to be used for rendering
        const Vector2& UVTo() const      { return currentFrame.To;   }
        
        /// Gets the relative size of the frame to the original image to be rendered
        const Vector2& Size() const      { return currentFrame.Size;   }
        
        
        /// Gets the relative offset of the frame to the original image to be rendered
        const Vector2& Offset() const      { return currentFrame.Offset;   }
        
        // As all game components
        virtual void Update(float dt);
        
    protected:
        
        void InitDefaults();
        
        void LoadSpriteSheet(const XMLElement& settings);
        
        //LoadAnimation();
    };
}

#endif

////////////////////////////////////////////////////////////////////////////////
//  Animation.h
//  Furiosity
//
//  Created by Bojan Endrovski on 2/4/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

#include "Defines.h"
#include "Transformable.h"
#include "Transformable3d.h"
#include "Entity3D.h"
#include "Colorable.h"

#include "BaseAnimation.h"
#include "ParameterAnimation.h"
#include "ValueAnimation.h"
#include "AnimationContainer.h"
#include "Animatable.h"

namespace Furiosity
{
    ///
    /// Event Animation, plays an event
    ///
    class EventAnimation : public Animation
    {
    private:
        function<void(void)>    event;
        
        bool                    fired;
        
    public:        
        EventAnimation(function<void(void)> event);
        
        virtual void Update(float dt);
        
        virtual void Reset()        { fired = false; }
        
        virtual bool IsDone() const { return fired; }
    };    
    
    
    ///
    /// Position Animation moves a transformable object
    ///
    class PositionAnimation : public ParameterAnimation<Transformable, Vector2>
    {
    public:
        PositionAnimation(Transformable* obj,
                          Vector2 from,
                          Vector2 to,
                          float duration,
                          InterpolationType interpolation = INTERPOLATION_LERP);
        
        PositionAnimation(Transformable* obj,
                          Vector2 to,
                          float duration,
                          InterpolationType interpolation = INTERPOLATION_LERP);
        
        virtual ~PositionAnimation() {}
    };
    
/*
    ///
    /// Scale Animation scales a transformable object
    ///
    class ScaleAnimation : public ParameterAnimation<Transformable, Vector2>
    {
    public:
        ScaleAnimation(Transformable* obj,
                       Vector2 from,
                       Vector2 to,
                       float duration,
                       InterpolationType interpolation = INTERPOLATION_LERP);
        
        ScaleAnimation(Transformable* obj,
                       Vector2 to,
                       float duration,
                       InterpolationType interpolation = INTERPOLATION_LERP);
        
        ~ScaleAnimation() {}
    };
*/
    
    
    ///
    /// Rotation Animation rotates a transformable object
    ///
    class RotationAnimation : public ParameterAnimation<Transformable, float>
    {
    public:
        RotationAnimation(Transformable* obj,
                          float from,
                          float to,
                          float duration,
                          InterpolationType interpolation = INTERPOLATION_LERP);
        
        RotationAnimation(Transformable* obj,
                          float to,
                          float duration,
                          InterpolationType interpolation = INTERPOLATION_LERP);
        
        virtual ~RotationAnimation() {}
    };
    
    
    ///
    /// Shake Animation it shakes an object for a bit
    ///
    class ShakeAnimation : public Animation
    {
    protected:
        /// An object to shake
        Transformable*  obj;
        
        /// Position to base the shake on
        Vector2         originalPos;
        
        /// Current intensity (distance)
        float           intensity;
        
        /// Intensity (distance) to shake this object with
        float           originalIntensity;
        
    public:
        
        ShakeAnimation(Transformable*   obj,
                       float            intensity,
                       float            duration);
        
        virtual void Update(float dt);
        
        virtual void Reset();
    };
    
    
    ///
    /// Play Animation is an animation that when played it plays (hopefully) another
    /// animation on an Animatable object.
    ///
    class PlayAnimation : public Animation
    {
        
    protected:
        std::string     animation;
        Animatable&     object;        
        bool            fired;
        
    public:
        PlayAnimation(Animatable& object, const std::string& animation);
        virtual void Update(float dt);
        virtual void Reset() { fired = false; }
        virtual bool IsDone() const { return fired; }
    };


    /*
    ///
    /// PlaySound plays a sound once
    ///
    class SoundResource;
    class PlaySound : public Animation
    {
    protected:
        /// Pointer to sound buffer data
        SoundResource *    sound;
        
        /// Flag wheter this event has fired
        bool                fired;
        
    public:
        /// Constructor with sound filename and optonal gain parameter
        PlaySound(const std::string& soundName, float gain = -1.0f);
        virtual ~PlaySound();
        
        virtual void Update(float dt);
        virtual void Reset() { fired = false; }
        virtual bool IsDone() const { return fired; }
    };
    */
    
    /*
    ///
    /// Used to animate the position of any Transformable3D object
    ///
    class Scale3DAnimation : public ParameterAnimation<Transformable3D, Vector3>
    {
    public:
        Scale3DAnimation(Transformable3D* obj,
                         Vector3 from,
                         Vector3 to,
                         float duration,
                         InterpolationType interpolation = INTERPOLATION_LERP);
        
        Scale3DAnimation(Transformable* obj,
                         Vector3 to,
                         float duration,
                         InterpolationType interpolation = INTERPOLATION_LERP);
        
        virtual ~Scale3DAnimation() {}
    };

    ///
    /// Used to animate the position of any Transformable3D object
    ///
    class Position3DAnimation : public ParameterAnimation<Transformable3D, Vector3>
    {
    public:
        Position3DAnimation(Transformable3D* obj,
                            Vector3 from,
                            Vector3 to,
                            float duration,
                            InterpolationType interpolation = INTERPOLATION_LERP);
        
        Position3DAnimation(Transformable3D* obj,
                            Vector3 to,
                            float duration,
                            InterpolationType interpolation = INTERPOLATION_LERP);
        
        virtual ~Position3DAnimation() {}
    };
    */
    
    ///
    /// Used to animate the position of any Transformable3D object
    ///
    class Scale3DAnimation : public ParameterAnimation<Entity3D, Vector3>
    {
    public:
        Scale3DAnimation(Entity3D* obj,
                         Vector3 from,
                         Vector3 to,
                         float duration,
                         InterpolationType interpolation = INTERPOLATION_LERP);
        
        Scale3DAnimation(Entity3D* obj,
                         Vector3 to,
                         float duration,
                         InterpolationType interpolation = INTERPOLATION_LERP);
        
        virtual ~Scale3DAnimation() {}
    };
    
    ///
    /// Used to animate the position of any Transformable3D object
    ///
    class Position3DAnimation : public ParameterAnimation<Entity3D, Vector3>
    {
    public:
        Position3DAnimation(Entity3D* obj,
                            Vector3 from,
                            Vector3 to,
                            float duration,
                            InterpolationType interpolation = INTERPOLATION_LERP);
        
        Position3DAnimation(Entity3D* obj,
                            Vector3 to,
                            float duration,
                            InterpolationType interpolation = INTERPOLATION_LERP);
        
        virtual ~Position3DAnimation() {}
    };

    ///
    /// Color Animation is used to animate the color of any Colorable object
    ///
    class ColorAnimation : public ParameterAnimation<Colorable, Color>
    {
    public:
        ColorAnimation(Colorable* el,
                       Color from,
                       Color to,
                       float duration,
                       InterpolationType interpolation = INTERPOLATION_LERP);
        
        ColorAnimation(Colorable* el,
                       Color to,
                       float duration,
                       InterpolationType interpolation = INTERPOLATION_LERP);
    };

}

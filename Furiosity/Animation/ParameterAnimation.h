////////////////////////////////////////////////////////////////////////////////
//  ParameterAnimation.h
//  Furiosity
//
//  Created by Bojan Endrovski on 25/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseAnimation.h"
#include "Frmath.h"


namespace Furiosity
{
    ///
    /// A templated generic animation for manipulating a object usig a setter
    /// a getter methods. Base class for many of the animations.
    ///
    template<class BaseT, class ValueT>
    class ParameterAnimation : public Animation
    {
        // Typedef a pointer to static member method
        typedef ValueT (BaseT::* Getter)(void) const;
        typedef void (BaseT::* Setter)(ValueT val);
        
    protected:
        /// Object being animated
        BaseT* baseObject;
        
        /// The method that will be used to get the intial value
        Getter get;
        
        /// The method that will be used to set the values
        Setter set;
        
        /// The interpolation start from this value
        ValueT base;
        
        /// The interpolation goes to this value
        ValueT target;
        
        /// Interpolation scheme
        InterpolationType interpolation;
        
    public:
        
        /// Create new animation with explicit "from" and "to" values
        ///
        /// @param object Object to animate
        /// @param setter Pointer to a setter method for the parameter on object to animate
        /// @param from Value to interpolate from
        /// @param to Value to interploate to
        /// @param duration Duration of this animation
        /// @param interpolation Interpolation method for the animation
        ParameterAnimation(BaseT* object,
                           Setter setter,
                           ValueT from,
                           ValueT to,
                           float duration,
                           InterpolationType interpolation = INTERPOLATION_LERP) :
            Animation(duration),
            baseObject(object),
            get(0),
            set(setter),
            base(from),
            target(to),
            interpolation(interpolation) {}
        
        /// Create new animation animation with explicit "from" and current "to",
        /// at the moment of starting the animation
        ///
        /// @param object Object to animate
        /// @param setter Pointer to a setter method for the parameter on object to animate
        /// @param getter Pointer to a getter method for the parameter on object to animate
        /// @param to Value to interploate to
        /// @param duration Duration of this animation
        /// @param interpolation Interpolation method for the animation
        ParameterAnimation(BaseT* object,
                           Getter getter,
                           Setter setter,
                           ValueT to,
                           float duration,
                           InterpolationType interpolation = INTERPOLATION_LERP) :
            Animation(duration),
            baseObject(object),
            get(getter),
            set(setter),
            base( CALL_MEMBER_FN(*baseObject, get)() ),
            target(to),
            interpolation(interpolation) {}
        
        /// Dtor
        virtual ~ParameterAnimation() {}
        
        /// Update the animation
        virtual void Update(float dt)
        {
            // Check first update (or reset)
            if (get && time <= 0.0f)
                base = CALL_MEMBER_FN(*baseObject, get)();
            
            // Update generic interpolation params
            Animation::Update(dt);

            // Manipulate the actual interpolation paremeter, this moves the
            // interpolation speed to time domain, rather than the value itself
            float t;
            switch (interpolation)
            {
                case INTERPOLATION_SPRING:
                case INTERPOLATION_LERP:
                    t = param;
                    break;
                    
                case INTERPOLATION_EASE_IN:
                    t = HermiteInterpolation(0.0f, 1.0f, param, 0.0f, 1.0f);
                    break;
                    
                case INTERPOLATION_EASE_OUT:
                    t = HermiteInterpolation(0.0f, 1.0f, param, 1.0f, 0.0f);
                    break;
                    
                case INTERPOLATION_EASE_IN_OUT:
                    t = HermiteInterpolation(0.0f, 1.0f, param, 0.0f, 0.0f);
                    break;
                    
                case INTERPOLATION_EASE_MID:
                    t = HermiteInterpolation(0.0f, 1.0f, param, 1.0f, 1.0f);
                    break;
            }
            
            // Use the adjusted parameter and set the value
            ValueT val = Lerp(base, target, t);
            CALL_MEMBER_FN(*baseObject, set) (val);
        }
        
        /// Reset the with an explicit values
        ///
        /// @param from From value
        /// @param to To value
        /// @param duration Duration of the animation, if not set it will use the original
        void Reset(ValueT from, ValueT to, float duration = -1.0f)
        {
            if(duration != -1) invduration = 1.0f / duration;
            base = from;
            target = to;
            time = 0;
        }
        
        /// Reset the with an explicit "from" value and current "to" value
        ///
        /// @param from From value
        /// @param duration Duration of the animation, if not set it will use the original
        void Reset(ValueT to, float duration = -1.0f)
        {
            if(get) base = CALL_MEMBER_FN(*baseObject, get)();
            if(duration != -1) invduration = 1.0f / duration;
            target = to;
            time = 0;
        }
    };
}
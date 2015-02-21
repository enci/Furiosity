////////////////////////////////////////////////////////////////////////////////
//  ValueAnimation.h
//  Furiosity
//
//  Created by Bojan Endrovski on 25/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseAnimation.h"


namespace Furiosity
{
    ///
    /// A templated generic animation for directly manipulating a value on an object
    ///
    template< class ValueT>
    class ValueAnimation : public Animation
    {
    protected:
        /// The interpolation start from this value
        ValueT& val;
        
        /// The interpolation start from this value
        ValueT base;
        
        /// The interpolation goes to this value
        ValueT target;
        
        /// Interpolation scheme
        InterpolationType interpolation;
        
        /// Flag wheter the base value should be init with the current value
        bool init;
        
    public:
        
        /// Create new animation with explicit "from" and "to" values
        ///
        /// @param object Object to animate
        /// @param from Value to interpolate from
        /// @param to Value to interploate to
        /// @param duration Duration of this animation
        /// @param interpolation Interpolation method for the animation
        ValueAnimation(ValueT&  val,
                       ValueT   from,
                       ValueT   to,
                       float    duration,
                       InterpolationType interpolation = INTERPOLATION_LERP) :
            Animation(duration),
            val(val),
            base(from),
            target(to),
            interpolation(interpolation),
            init(false) {}
        
        /// Create new animation animation with explicit "from" and current "to",
        /// at the moment of starting the animation
        ///
        /// @param object Object to animate
        /// @param to Value to interpolate to
        /// @param duration Duration of this animation
        /// @param interpolation Interpolation method for the animation
        ValueAnimation(ValueT&  val,
                       ValueT   to,
                       float    duration,
                       InterpolationType interpolation = INTERPOLATION_LERP) :
            Animation(duration),
            val(val),
            base(val),
            target(to),
            interpolation(interpolation),
            init(true) {}
        
        /// Dtor
        virtual ~ValueAnimation() {}
        
        virtual void Update(float dt)
        {
            // Check first update (or reset)
            if(init)
            {
                base = val;
                init = false;
            }
            
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
            ValueT v = Lerp(base, target, t);
            val = v;
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
            init = true;
            if(duration != -1) invduration = 1.0f / duration;
            target = to;
            time = 0;
        }
    };
}




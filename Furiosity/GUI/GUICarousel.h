////////////////////////////////////////////////////////////////////////////////
//  Carousel.h
//
//  Created by Bojan Endrovski on 11/1/11.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef CAROUSEL_H
#define CAROUSEL_H

#include "GUIContainer.h"
#include "Input.h"
#include "ValueSmoother.h"

namespace Furiosity
{
    class GUICarousel : public GUIContainer
    {
    public:
        enum Orientation
        {
            ORIENTATION_HORIZONTAL,
            ORIENTATION_VERTICAL,
        };
        
    protected:
        
        // Current touch
        Touch*                  touch;
        
        // Used to fake input when tapped
        Touch                   fakeTouch;
        
        // Current dragging velocity
        Vector2                 draggVelocity;
        
        // Current position
        Vector2                 currPos;
        
        // Previous position
        Vector2                 prevPos;
        
        Vector2                 moveaccum;
        
        /// Accumulates the distance over a frames (per touch and any direction)
        float                   distance;
        
        ///
        float                   dirDistance;
        
        /// Orientation
        Orientation             orientation;
        
        /// Current offset, this is how much the carousel has moved
        float                   offset;

        // Wrap arround if set to positive
        float                   wrap;
        
        // Slow down
        float                   damping;

        // Smooth out the velocity
        ValueSmoother<Vector2>  smoother;
        
        /// Spacing, used for snapped navigation
        float                   spacing;
        
        /// Slides, used for snapped navigation
        int                     slides;
        
        vector<GUIElement*>     pageIndicators;
        
        int                     index;
        
        GUIContainer*           pages;
        
        int                     targetIndex;
        
        bool                    skipSnapping;
        
        // Ticks
        SoundResource *        tickSnd;
        float                   tickDist;
        float                   tickTravel;
        
    public:
        // Ctor
        GUICarousel(Vector2     origin,
                    Vector2     size,
                    Orientation orientation = ORIENTATION_HORIZONTAL,
                    int         slides      = -1,
                    float       spacing     = -1.0f);
               
        virtual void HandleTouch(Touch* touch);
                
        virtual void Update(float dt);
        
        int Index() const                   { return index; }
        void SelectIndex(int idx, bool instantly);
        
        void Reset();
        
        float Wrap() const                  { return wrap; }
        void SetWrap(float w)               { wrap = w; }
        
        float Damping() const               { return damping; }
        void SetDamping(float d)            { damping = d; }
        
        Vector2 DraggVelocity() const       { return draggVelocity; }
        void SetDraggVelocity(Vector2 dv)   { draggVelocity = dv; }
        
        
        
        float ScrollOffset() const          { return offset; }
        void SetScrollOffset(float off);
        
        void SetTickSound(const string& sound, float travel, float gain = -1.0f);        
        
        int IndicatorCount() const          { return (int)pageIndicators.size(); }
        
        void SetPageIndicatorColor(int i, Color c);
        
        float                               MinOffset;
        
        float                               MaxOffset;

    };
}

#endif

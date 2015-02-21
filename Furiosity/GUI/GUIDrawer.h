////////////////////////////////////////////////////////////////////////////////
//  GUIDrawer.h
//
//  Created by Bojan Endrovski on 3/21/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GUIContainer.h"
#include "Draggable.h"

namespace Furiosity
{
    class GUIDrawer : public GUIContainer
    {
    public:
        enum Orientation
        {
            ORIENTATION_HORIZONTAL,
            ORIENTATION_VERTICAL,
        };
        
        enum State
        {
            OPENED,
            CLOSED
        };
        
    protected:
        // Camera ref used to know the where the touch is in
        // const Camera2D&     camera;
        
        // A transform matrix for this object to move
        // Matrix33&           trans;
        
        // Current touch
        Touch*              touch;
        
        // Current dragging velocity
        Vector2             draggVelocity;
        
        // Current position
        Vector2             currPos;
        
        // Previous position
        Vector2             prevPos;
        
        Orientation         orientation;
        
        Vector2             base;
        
        float               travel;
        
        State               state;
        
        float               min;
        float               max;
        
    public:
        GUIDrawer(Vector2 origin, Vector2 size,
                  float min, float max,
                  Orientation orientation = ORIENTATION_HORIZONTAL);
        
        // Virtual update call
        virtual void Update(float dt);

        // Handle a touch
        virtual void HandleTouch(Touch* touch);
    };
}

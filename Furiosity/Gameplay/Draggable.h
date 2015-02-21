////////////////////////////////////////////////////////////////////////////////
//  Draggable.h
//
//  Created by Bojan Endrovski on 01/06/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include "Camera2D.h"
#include "Matrix33.h"
#include "Input.h"

namespace Furiosity
{
    
    class Draggable : public TouchHandler
    {
    protected:
        // Camera ref used to know the where the touch is in
        const Camera2D&     camera;
        
        // A transform matrix for this object to move
        Matrix33&           trans;
        
        // Current touch
        Touch*              touch;
        
        // Current dragging velocity
        Vector2             draggVelocity;
        
        // Current position
        Vector2             currPos;
        
        // Previous position
        Vector2             prevPos;
        
        // Radius it collects touches in
        float               radius;
        
    public:        
        // Ctor
        Draggable(const Camera2D& camera, Matrix33& transform, float radius);
        
        // Dtor
        virtual ~Draggable() {}
        
        // From TouchHandler
        virtual void HandleTouch(Touch& touch);

        // From TouchHandler        
        virtual void DropTouches() { touch = 0; }
        
        // All the acction happens in the update
        virtual void Update(float dt);
        
        // This object is being dragged as we querry
        bool IsDragged() { return touch != 0; }
        
        Vector2 DraggVelocity() const { return draggVelocity; }
        
        // Drop it
        void Drop();
        
        // Respond to dragging begin
        virtual void DraggingBegin() {}
        
        // Respond to dragging end 
        virtual void DraggingEnd() {}
    };
}

#endif

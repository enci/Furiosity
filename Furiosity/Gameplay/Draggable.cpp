////////////////////////////////////////////////////////////////////////////////
//  Draggable.h
//  Furiosity
//
//  Created by Bojan Endrovski on 01/06/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Draggable.h"

using namespace Furiosity;

Draggable::Draggable(const Camera2D& camera, Matrix33& transform, float radius) :
    camera(camera),
    trans(transform),
    touch(0),
    radius(radius),
    draggVelocity() // [0,0]
{}


void Draggable::HandleTouch(Touch& t)
{
    if(!touch)
    {
        if(t.Phase != TOUCH_PHASE_CANCELLED   &&
           t.Phase != TOUCH_PHASE_INVALID     &&
           t.Phase != TOUCH_PHASE_ENDED       &&
           !t.Handled())
        {
            float d = (camera.Unproject(t.Location) - trans.Translation()).Magnitude();
            if (d < radius)
            {
                touch = &t;
                t.Handle(this);
                DraggingBegin();
                //
                currPos = camera.Unproject(touch->Location);
                prevPos = currPos;
            }
        }
    }
    else if(touch == &t)
    {
        if(t.Phase == TOUCH_PHASE_CANCELLED   ||
           t.Phase == TOUCH_PHASE_INVALID     ||
           t.Phase == TOUCH_PHASE_ENDED )
        {
            touch = 0;
            DraggingEnd();
        }
        else
        {
            currPos = camera.Unproject(touch->Location);
            trans.SetTranslation(currPos);
        }
    }
}

void Draggable::Update(float dt)
{
    if(touch)
    {
        draggVelocity = (currPos - prevPos) * (1/dt);
        prevPos = currPos;
        trans.SetTranslation(camera.Unproject(touch->Location));
    }
    else
    {
        draggVelocity.Clear();
    }
}

void Draggable::Drop()
{
    if(touch)
    {
        touch->Unhandle();
        touch->Phase = TOUCH_PHASE_INVALID;
        touch = 0;
    }
    
    draggVelocity.Clear();
} 


// eof
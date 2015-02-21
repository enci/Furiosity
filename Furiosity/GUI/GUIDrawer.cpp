////////////////////////////////////////////////////////////////////////////////
//  GUIDrawer.cpp
//
//  Created by Bojan Endrovski on 3/21/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIDrawer.h"
#include "GUIManager.h"

using namespace Furiosity;

GUIDrawer::GUIDrawer(Vector2 origin, Vector2 size,
                     float min, float max,
                     Orientation orientation) :
    GUIContainer(origin, size),
    touch(0),
    currPos(origin),
    prevPos(origin),
    orientation(orientation),
//    travel(travel),
    state(CLOSED),
    min(min),
    max(max)
{
}

void GUIDrawer::Update(float dt)
{
    GUIContainer::Update(dt);
    
    if(touch && !gInputManager.IsTouchValid(*touch))
    {
        parent->PlayAnimation("Hide");
        touch = 0;
        state = CLOSED;
    }
//    Draggable::Update(dt);
}

void GUIDrawer::HandleTouch(Furiosity::Touch *t)
{
    const Camera2D& camera = gGUIManager.Camera();
    GUIContainer::HandleTouch(t);
    
    if(orientation == ORIENTATION_HORIZONTAL)
    {
        if(Position().x > max)
            SetPosition( Vector2(max, Position().y));
        if(Position().x < min)
            SetPosition( Vector2(min, Position().y));

    } else
    {
        /* TODO
        if(Position().y > travel)
            SetPosition( Vector2(travel, Position().y));
         */
    }
    
    if(!touch)
    {
        if(t->Phase == TOUCH_PHASE_BEGAN   &&
           !t->Handled()                   &&
           InRegion(t))
        {
            touch = t;
            t->Handle(this);
            base = Position();
            
            /*
            //
            currPos = camera.Unproject(touch->Location);
            prevPos = currPos;
             */
        }
    }
    else if(touch == t)
    {
        if(t->Phase == TOUCH_PHASE_CANCELLED   ||
           t->Phase == TOUCH_PHASE_INVALID     ||
           t->Phase == TOUCH_PHASE_ENDED )
        {            
            touch = 0;
            if(state == CLOSED)
            {
                parent->PlayAnimation("Hide");
            } else {
                parent->PlayAnimation("Show");
            }
        }
        else
        {
            Vector2 move =  camera.Unproject(touch->Location) -
                            camera.Unproject(touch->PreviousLocation);
            
            if(orientation == ORIENTATION_HORIZONTAL)
                move.y = 0.0f;
            else
                move.x = 0.0f;
            
            Translate(move);
            
            if ( (Position() - base).Magnitude() > 300)
            {
                if(state == CLOSED)
                {
                    parent->PlayAnimation("Show");
                    touch = 0;
                    state = OPENED;
                }
                else
                {
                    parent->PlayAnimation("Hide");
                    touch = 0;
                    state = CLOSED;
                }
            }
            
            /*
            currPos = camera.Unproject(touch->Location);
            trans.SetTranslation(currPos);
             */
        }
    }
}

/*
void Draggable::Update(float dt)
{
    if(touch)
    {
        //Vector2 pos = camera.Unproject(touch->Location);
        //trans.SetTranslation(pos);
        //
        //Vector2 prev = camera.Unproject(touch->PreviousLocation);
        //
        draggVelocity = (currPos - prevPos) * (1 / dt);
        prevPos = currPos;
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
*/


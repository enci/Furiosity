////////////////////////////////////////////////////////////////////////////////
//  Carousel.xpp
//
//  Created by Bojan Endrovski on 11/1/11.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUICarousel.h"

#include "Input.h"
#include "AudioManager.h"
#include "GUIManager.h"
#include "GUIImage.h"
#include "GUIAnimations.h"

#include <assert.h>

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
GUICarousel::GUICarousel(Vector2        origin,
                         Vector2        size,
                         Orientation    orientation,
                         int            slides,
                         float          spacing) :
    GUIContainer(origin, size),
    touch(0),
    wrap(-1.0f),
    smoother(1),  // A velocity smooter
    moveaccum(),
    orientation(orientation),
    damping(0.8f),
    spacing(spacing),
    slides(slides),
    offset(0.0f),
    targetIndex(-1),
    index(-1),
    distance(0.0f),
    dirDistance(0.0f),
    MaxOffset(FLT_MAX),
    MinOffset(-FLT_MAX),
    skipSnapping(false)
{
    masking = true;
    
    if (slides > 1)
    {
        pages = new GUIContainer(Vector2(0, Size().y * -0.5 + 30), Vector2(Size().x * 0.5f, 30));
        AddElement(pages, INT_MAX);
        
        pageIndicators.resize(slides);
        float dx = pages->Size().x / slides;
        float x = -0.5f * dx * (slides - 1);
        for (int i = 0; i < slides; i++)
        {
            auto img = new GUIImage(Vector2(x, 0), Vector2(12, 12), "/SharedResources/Textures/circle.png", true);
            pages->AddElement(img);
            pageIndicators[i] = img;
            x += dx;
        }        
    }
}

////////////////////////////////////////////////////////////////////////////////
// Handle touch
////////////////////////////////////////////////////////////////////////////////
void GUICarousel::HandleTouch(Touch* t)
{
    if(!Enabled())
        return;
    
    const Camera2D& camera = gGUIManager.Camera();

    if(!touch)
    {
        if(t->Phase == TOUCH_PHASE_BEGAN &&
           !t->Handled())
        {
            if(InRegion(t))
            {
                touch = t;
                t->Handle(this);
                distance    = 0;
                dirDistance = 0;
                smoother.Clear();
                //
                currPos = camera.Unproject(touch->Location);
                prevPos = currPos;
                
                // TODO: Test this!
                if(!IsPlaying()) StopAnimation();
            }
        }
    }
    else if(touch == t)
    {
        if(t->Phase == TOUCH_PHASE_CANCELLED   ||
           t->Phase == TOUCH_PHASE_INVALID     ||
           t->Phase == TOUCH_PHASE_ENDED )
        {
            touch = 0;
            
            // Snapping
            if(slides > 0 && spacing > 1.0f)
            {
                float min = 0.08f;
                //
                if(dirDistance > spacing * min  &&
                   dirDistance < spacing * 0.5f &&
                   index > 0)
                {
                    targetIndex = index - 1;
                }
                else if(dirDistance < -spacing * min    &&
                        dirDistance > -spacing * 0.5f   &&
                        index < slides - 1)
                {
                    targetIndex = index + 1;
                }
                dirDistance = 0;
            }
        }
        else
        {
            currPos = camera.Unproject(touch->Location);
        }
    }

    // Pass the input down
    if(InRegion(t))
    {
        fakeTouch = *t;
        fakeTouch.Unhandle();
        if(distance > 50) fakeTouch.Phase = TOUCH_PHASE_CANCELLED;
        //
        for(int i = 0; i < elements.size(); i++)
            elements[i]->HandleTouch(&fakeTouch);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////////////////////
void GUICarousel::Update(float dt)
{
    // Get movement from touch
    Vector2 move;
    if(touch)
    {
        move = currPos - prevPos;
        moveaccum += move;
        Vector2 dv = (currPos - prevPos) / dt;  // Get velocity
        draggVelocity = smoother.Update(dv);    // Smooth out the input if needed
        prevPos = currPos;                      // Clear movement
        distance += move.Magnitude();
        //
        if(!gInputManager.IsTouchValid(*touch))
            touch = 0;
    }
    else
    {
        draggVelocity *= damping;
    }
    // The final result is velocity
    float velocity = (orientation == ORIENTATION_HORIZONTAL) ? draggVelocity.x : draggVelocity.y;
    float springTarget            = offset;
    
    ////////////////////////////////////////////////////////////////////////////////
    // Apply snapping to snapping carousels
    if(slides > 0 && spacing > 1.0f && pageIndicators.size() > 0)
    {
        int idx = (int) roundf(-offset / spacing);
        idx = Clamp(idx, 0, slides - 1);
        
        if(idx != index)
        {
            auto curr = pageIndicators[idx];
            curr->AddAnimation("Enlarge", new GUISizeAnimation(curr,
                                                               Vector2(25, 25),
                                                               0.25f,
                                                               INTERPOLATION_EASE_IN));
            curr->PlayAnimation("Enlarge");
            //
            if(index >= 0)
            {
                auto prev = pageIndicators[index];
                prev->AddAnimation("Reduce",
                                   new GUISizeAnimation(prev,
                                                        Vector2(12, 12),
                                                        0.25f,
                                                        INTERPOLATION_EASE_IN));
                prev->PlayAnimation("Reduce");
            }
            index = idx;
        }
        
        //LOG("Offeset=%2.2f     IDX=%i\n", offset, idx);
        //
        if(touch == 0)
        {
            // Check if it's needed to move to
            if(targetIndex != -1)
            {
                if(idx == targetIndex)
                    targetIndex = -1;
                else
                    idx = targetIndex;
            }
            
            springTarget      = -idx * spacing;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Non-snapping carousels
    else
    {
        if(touch == 0)
        {
            if(springTarget > MaxOffset)
                springTarget = MaxOffset;
            else if(springTarget < MinOffset)
                springTarget = MinOffset;
        }
    }
    
    // Use spring for snapping or limiting
    if(touch == 0 && offset != springTarget && !skipSnapping)
    {
        float springDistance    = springTarget - offset;
        float springConstant    = 700.0f;
        float springForce       = springConstant * springDistance;
        //
        float totalForce        = springForce;
        //
        float mass              = 1.0f;
        // Acceleration = Force/Mass
        float acceleration      = totalForce / mass;
        // Update velocity
        velocity                += acceleration * dt;
    }
    else
    {
        // Limit dragging speed on edges
        if(offset > MaxOffset || offset < MinOffset)
            velocity *= 0.3f;
    }
    
    float delta = velocity * dt;
    offset      += delta;
    dirDistance += delta;
    
    // Update positions of all elements
    for (int i = 0; i < elements.size(); ++i)
    {
        GUIElement* el = elements[i];
        
        if(el == pages)
            continue;
        
        float pos = orientation == ORIENTATION_HORIZONTAL ? el->Position().x : el->Position().y;
        
        if(wrap != -1.0f)
        {
            if(pos > wrap * 0.5f)
                pos -= wrap;
            else if(pos < wrap * -0.5f)
                pos += wrap;
        }
        
        pos += delta;
        if(orientation == ORIENTATION_HORIZONTAL)
            el->SetPosition( Vector2(pos, el->Position().y) );
        else
            el->SetPosition( Vector2(el->Position().x, pos) );
    }
    
    GUIContainer::Update(dt);
    skipSnapping = false;
}

////////////////////////////////////////////////////////////////////////////////
// Reset
////////////////////////////////////////////////////////////////////////////////
void GUICarousel::Reset()
{
    /*
    state = CarouselStateIdle;
    scrool = 0;
    scroolBase = 0;
    touch = 0;
    currentIndex = 0;
    currentIterpolationParam = 0.0f;
    fakeTimer = 0;
    velocity = 0;
    targetVelocity = 0;
    distance = 0;    
    */
}


void GUICarousel::SelectIndex(int idx, bool instantly)
{
    if(instantly)
    {
        SetScrollOffset(-idx * spacing);
    }
    else
        targetIndex = idx;
}


void GUICarousel::SetScrollOffset(float off)
{
    float diff = off - offset;
    skipSnapping = true;
    
    // float velocity = (orientation == ORIENTATION_HORIZONTAL) ? draggVelocity.x : draggVelocity.y;
    
    //    draggVelocity.Clear();
    
    //float vel = diff / (-1.0f / 60.0f);
    //draggVelocity = orientation == ORIENTATION_HORIZONTAL ? Vector2(vel, 0) : Vector2(0, vel);
    draggVelocity.Clear();
    
    // Update positions of all elements
    for (int i = 0; i < elements.size(); ++i)
    {
        GUIElement* el = elements[i];
        
        if(el == pages)
            continue;
        
        float pos = orientation == ORIENTATION_HORIZONTAL ? el->Position().x : el->Position().y;
        pos += diff;
        
        if(wrap != -1.0f)
        {
            if(pos > wrap * 0.5f)
                pos -= wrap;
            else if(pos < wrap * -0.5f)
                pos += wrap;
        }
        
        if(orientation == ORIENTATION_HORIZONTAL)
            el->SetPosition( Vector2(pos, el->Position().y) );
        else
            el->SetPosition( Vector2(el->Position().x, pos) );
    }

    offset = off;
}


void GUICarousel::SetPageIndicatorColor(int i, Furiosity::Color c)
{
    ASSERT(i >= 0);
    ASSERT(i < pageIndicators.size());
    pageIndicators[i]->SetColor(c);
}




// end

////////////////////////////////////////////////////////////////////////////////
//  GUIButton.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIButton.h"

// Local
#include "GUIManager.h"
#include "AudioManager.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                              - BUTTON -
//
////////////////////////////////////////////////////////////////////////////////

// Creates a new button with size and path to the texutre
GUIButton::GUIButton(Vector2        origin,
                     Vector2        size,
                     const string& texutrename,
                     bool           isToggle,
                     bool           mipmap)
:   GUIElement(origin, size),
    pressed(false),
    touch(NULL),
    isToggle(isToggle),
    toggled(false),
    sound(NULL),
    cooldownTime(0.1f),
    cooldown(cooldownTime),
    texture(NULL),
    tolerance(1.0f),
    event(nullptr),
    toggleEvent(nullptr)
{
    if(!texutrename.empty())
    {
        texture = gResourceManager.LoadTexture(texutrename, mipmap);
        assert(texture);
    }
}

void GUIButton::SetSound(const string& soundname)
{
    if(sound)
        SafeDelete(sound);
    
    sound = new Sound(soundname);
}

// Dtor
GUIButton::~GUIButton()
{
    gResourceManager.ReleaseResource(texture);
    SafeDelete(sound);
}

// Update button state
void GUIButton::Update(float dt)
{
    GUIElement::Update(dt);
    
    UpdateTransform();
    if (cooldown > 0.0f)
        cooldown -= dt;
}


void GUIButton::Hide()
{
    pressed = false;
    touch = NULL;
    //
    GUIElement::Hide();
}

void GUIButton::SetTexture(const string& texutrename, bool mipmap)
{
    gResourceManager.ReleaseResource(texture);
    //
    if(!texutrename.empty())
        texture = gResourceManager.LoadTexture(texutrename, mipmap);
}

/*
 void GUIButton::SetEnabled(bool e)
 {
 enabled = e;
 //
 if(!enabled)
 {
 pressed = false;
 touch = NULL;
 }
 }
 */


void GUIButton::HandleTouch(Touch* t)
{
    if (!Enabled() /* || InTransition() */ )
        return;
    
    if(touch)
    {
        if(!gInputManager.IsTouchValid(*touch))
        {
            pressed = false;
            touch = NULL;
        }
    }
    
    if(!pressed && !t->Handled())
    {
        // Screen to local position
        // Vector2 loc = LocalCoordinates(t->Location);
        
        if((t->Phase == TOUCH_PHASE_BEGAN ||
            t->Phase == TOUCH_PHASE_STATIONARY) &&   // Only touches that being there
           InRegion(t, tolerance))                  // Check if in region
        {
            touch = t;
            touch->Handle(this);
            pressed = true;
        }
    }
    else if(pressed && touch == t)
    {
        // Cancel press if finger is lost
        if(touch->Phase == TOUCH_PHASE_CANCELLED ||
           touch->Phase == TOUCH_PHASE_INVALID)
        {
            pressed = false;
            touch = NULL;
            return;
        }
        
        // Cancel if finger gets out of the button
        if( !InRegion(t, tolerance + 0.2f))      // Check if in region
        {
            pressed = false;
            touch = NULL;
            return;
        }
        
        // Finally raise event on lifted finger
        if (touch->Phase == TOUCH_PHASE_ENDED)
        {
            pressed = false;
            touch = NULL;
            
            if (cooldown <= 0.0f)
            {
                toggled = !toggled;
                
                // Call event handeler
                if(toggleEvent)
                    toggleEvent(toggled);
                else if(event)
                    event();
                else
                    LOG("Button has no callback! \n");
                
                // Play sound if set
                if(sound)
                    gAudioManager.PlaySound(sound);
                else if (gGUIManager.DefaultButtonSound())
                    gAudioManager.PlaySound(gGUIManager.DefaultButtonSound());
                
                cooldown = cooldownTime;
            } // if touch ended
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Render
////////////////////////////////////////////////////////////////////////////////
void GUIButton::Render(SpriteRender& rend)
{
    if(!texture || !Visible() || tint.a == 0)
        return;
    
    UpdateTransform();
    
    float cut = isToggle ? 0.5f : 1.0f;
    
    // Update the rendering shit
    if (pressed)
        texOffset = Vector2(0.5f, 0.0f);
    else
        texOffset = Vector2(0.0f, 0.0f);
    
    if(isToggle && toggled)
        texOffset += Vector2(0.0f, 0.5f);
    
    Vector2 from = texOffset;
    Vector2 to   = Vector2(0.5f, cut) + texOffset;
    
    rend.DrawQuad(transform,
                  size.x,
                  size.y,
                  texture,
                  offset,
                  tint,
                  from,
                  to);
}

#ifdef DEBUG
void GUIButton::DebugDraw(DebugDraw2D& debugDraw)
{
	GUIElement::DebugDraw( debugDraw );
    
    if(!Enabled() || !Visible())
        return;
    
    // Create four vertices
    Vector2 s = size * 0.5f + Vector2(tolerance, tolerance);
    Vector2 A(-s.x, -s.y);
    Vector2 B(s.x, -s.y);
    Vector2 C(s.x, s.y);
    Vector2 D(-s.x, s.y);
    
    // Transform vectors
    transform.TransformVector2(A);
    transform.TransformVector2(B);
    transform.TransformVector2(C);
    transform.TransformVector2(D);
    
    debugDraw.AddLine(A, B, Color::Green);
    debugDraw.AddLine(B, C, Color::Green);
    debugDraw.AddLine(C, D, Color::Green);
    debugDraw.AddLine(D, A, Color::Green);
}
#endif


////////////////////////////////////////////////////////////////////////////////
// GUI Shrink Button
////////////////////////////////////////////////////////////////////////////////
GUIShrinkButton::GUIShrinkButton(Vector2 origin,
                                 Vector2 size,
                                 const string& texutrename,
                                 float minSize,
                                 bool isToggle,
                                 bool mipmap) :
    GUIButton(origin, size, texutrename, isToggle, mipmap),
    minSize(minSize),
    sizeFix(1.0f)
{}

void GUIShrinkButton::Render(SpriteRender& rend)
{
    float m = pressed ? minSize : 1.0f;
    
    Vector2 fromUV(0, 0);
    Vector2 toUV(1,1);
    
    if(isToggle)
    {
        toUV.x = 0.5f;
        if(toggled)
        {
            fromUV.x    += 0.5;
            toUV.x      += 0.5;
        }
    }
    
    
    rend.DrawQuad(transform,
                  size.x * m * sizeFix,
                  size.y * m * sizeFix,
                  texture,
                  offset,
                  tint,
                  fromUV,
                  toUV);
}



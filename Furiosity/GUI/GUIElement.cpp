////////////////////////////////////////////////////////////////////////////////
//  GUIElement.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIElement.h"

#include "GUI.h"

using namespace Furiosity;

GUIElement::GUIElement(Vector2 postition, Vector2 size):
    Transformable(local),
    TouchHandler(false, 0),
    Colorable(Color::White),
    size(size),
    parent(0),
    visible(true),
    state(GUIElementState::ELEMENT_STATE_ENABLED),
    capture(false),
    offset(Vector2()),
    anchoring(ANCHORING_NONE),
    anchoredPosition()
{
    tint = Color::White;
    //
    local.SetIdentity();
    local.SetTranslation(postition);
    //
    UpdateTransform();
}


void GUIElement::AnchorInParent()
{
    // Early out
    if(!parent)
        return;
    //
    if(anchoring == ANCHORING_NONE)
        return;
    
    Vector2 sz = parent->Size();
    Vector2 pos; //  = local.Translation();
    
    if(anchoring & ANCHORING_TOP)
        pos.y = sz.y * 0.5f;
    else if(anchoring & ANCHORING_BOTTOM)
        pos.y = sz.y * -0.5f;
    
    if(anchoring & ANCHORING_LEFT)
        pos.x = sz.x * -0.5f;
    else if(anchoring & ANCHORING_RIGHT)
        pos.x = sz.x * 0.5f;
    
    pos += anchoredPosition;
    
    local.SetTranslation(pos);
}


void GUIElement::Update(float dt)
{    
    Animatable::Update(dt);
    
    // Update the transform
    AnchorInParent();
    UpdateTransform();
    UpdateColor();
}

void GUIElement::UpdateTransform()
{
    if(parent != 0)
    {
        transform = parent->transform;
        transform.Multiply(local);
    } else {
        transform = local;
    }
}

void GUIElement::UpdateColor()
{
    if(parent)
        tint = color * parent->Tint();
    else
        tint = color;
}

void GUIElement::PlayAnimation(const string &name)
{
    Animatable::PlayAnimation(name);
    
    Animatable::Update(0.0f);
    
    // Update the transforms and color
    AnchorInParent();
    UpdateTransform();
    UpdateColor();
}

void GUIElement::Show()
{
    // Make sure it's visible
    SetVisible(true);
    Enable();
}

void GUIElement::Hide()
{
    SetVisible(false);
    Disable();
}

Vector2 GUIElement::LocalCoordinates(const Vector2 &vec)
{
    // Get (GUI) world coordinates
    Vector2 loc = gGUIManager.Local(vec);
    
    // Transform to local element coordinates
    Matrix33 inv = transform.Inverse();
    inv.TransformVector2(loc);
    //
    return loc;
}

bool GUIElement::InRegion(const Touch* touch, float tolerance)
{
    // Screen to local position
    Vector2 loc = LocalCoordinates(touch->Location);
    //
    float hx = size.x * 0.5f + tolerance;
    float hy = size.y * 0.5f + tolerance;
    //
    if(loc.x >= -hx && loc.x <= hx &&     // Check if in region
       loc.y >= -hy && loc.y <= hy)       // -//-
        return true;
    //
    return false;
}

void GUIElement::SetAnchoring(uint a, Vector2 offsetFromAnchor)
{
    anchoring = a;
    anchoredPosition = offsetFromAnchor;
    
    if(anchoring == ANCHORING_NONE)
        SetPosition(anchoredPosition);
}

void GUIElement::SetAnchoring(uint a)
{
    Vector2 sz = Size();
    Vector2 offset;
    
    // Vertical
    if(a & ANCHORING_TOP)
        offset.y = sz.y * -0.5f;
    else if(a & ANCHORING_BOTTOM)
        offset.y = sz.y * 0.5f;
    
    // Horizontal
    if(a & ANCHORING_LEFT)
        offset.x = sz.x * 0.5f;
    else if(a & ANCHORING_RIGHT)
        offset.x = sz.x * -0.5f;
    
    // Save
    anchoring           = a;
    anchoredPosition    = offset;
}


Vector2 GUIElement::AnchoredPosition(uint a, Furiosity::Vector2 offsetFromAnchor) const
{
    // Early out
    if(!parent)
        return Vector2();
    //
    if(a == ANCHORING_NONE)
        return Vector2();
    
    Vector2 sz = parent->Size();
    Vector2 pos  = local.Translation();
    
    if(a & ANCHORING_TOP)
        pos.y = sz.y * 0.5f;
    else if(a & ANCHORING_BOTTOM)
        pos.y = sz.y * -0.5f;
    
    if(a & ANCHORING_LEFT)
        pos.x = sz.x * -0.5f;
    else if(a & ANCHORING_RIGHT)
        pos.x = sz.x * 0.5f;
    
    pos += offsetFromAnchor;
    
    return pos;
}


#ifdef DEBUG
void GUIElement::DebugDraw(DebugDraw2D& debugDraw)
{
    Color color = Color::Red;
    
    if(!Enabled() || !Visible())
        return;
        
    if(tint.a == 0)
        color = Color::Blue;
    
    // Create four vertices
    Vector2 s = size * 0.5f;
    Vector2 A(-s.x, -s.y);
    Vector2 B(s.x, -s.y);
    Vector2 C(s.x, s.y);
    Vector2 D(-s.x, s.y);
    
    // Transform vectors
    transform.TransformVector2(A);
    transform.TransformVector2(B);
    transform.TransformVector2(C);
    transform.TransformVector2(D);
    
    debugDraw.AddLine(A, B, color);
    debugDraw.AddLine(B, C, color);
    debugDraw.AddLine(C, D, color);
    debugDraw.AddLine(D, A, color);
}
#endif


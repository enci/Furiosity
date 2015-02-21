////////////////////////////////////////////////////////////////////////////////
//  GUILabeledButton.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 11/09/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUILabeledButton.h"

using namespace Furiosity;


GUILabeledButton::GUILabeledButton(Vector2 position, GUILabel* label, GUIShrinkButton* button)
    : GUIContainer(position, button->Size())
    , label(label)
    , button(button)
{
    AddElement(button);
    AddElement(label);
}


GUILabeledButton::GUILabeledButton(Vector2 position,
                                   const string& text,
                                   const string&  fontName,
                                   uint           fontSize)
    : GUIContainer(position, Vector2(0, 0))
{
    // Make label
    label = new GUILabel(Vector2(0, 0), text, fontName, fontSize);
    label->SetColor(Color::Black);
    
    // Choose image
    const string& img =    (Width() / Height() > 2.5f) ?
    "/SharedResources/Textures/ButtonWide.png" :
    "/SharedResources/Textures/Button2.png";
    
    // Make button
    button = new GUIShrinkButton(Vector2(), label->Size() + Vector2(fontSize, fontSize), img);
    
    // Add to container
    AddElement(button);
    AddElement(label);
    SetSize(button->Size());
}

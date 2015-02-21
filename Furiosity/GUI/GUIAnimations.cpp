////////////////////////////////////////////////////////////////////////////////
//  GUIAnimations.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/6/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIAnimations.h"

using namespace Furiosity;


////////////////////////////////////////////////////////////////////////////////
//
//                       - GUI Size  Animation -
//
////////////////////////////////////////////////////////////////////////////////

GUISizeAnimation::GUISizeAnimation(GUIElement* el,
                                   Vector2 from,
                                   Vector2 to,
                                   float duration,
                                   InterpolationType interpolation) :
    ParameterAnimation(el,
                       &GUIElement::SetSize,
                       from,
                       to,
                       duration,
                       interpolation) {}

GUISizeAnimation::GUISizeAnimation(GUIElement* el,
                                   Vector2 to,
                                   float duration,
                                   InterpolationType interpolation) :
    ParameterAnimation(el,
                       &GUIElement::Size,
                       &GUIElement::SetSize,
                       to,
                       duration,
                       interpolation) {}

// end
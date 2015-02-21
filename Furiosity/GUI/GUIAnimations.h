////////////////////////////////////////////////////////////////////////////////
//  GUIAnimations.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/6/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GUIElement.h"


namespace Furiosity
{    
    ////////////////////////////////////////////////////////////////////////////////
    // GUI Show
    ////////////////////////////////////////////////////////////////////////////////
    class GUIShow : public Animation
    {
    private:
        GUIElement& el;
        
    public:
        GUIShow(GUIElement& el) : Animation(-1), el(el) {}
        virtual void Update(float dt) { el.Show(); }
        virtual bool IsDone() { return el.Visible(); }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // GUI Hide
    ////////////////////////////////////////////////////////////////////////////////
    class GUIHide : public Animation
    {
    private:
        GUIElement& el;
        
    public:
        GUIHide(GUIElement& el) : Animation(-1), el(el) {}
        virtual void Update(float dt) { el.Hide(); }
        virtual bool IsDone() { return !el.Visible(); }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // GUI Size Animation
    ////////////////////////////////////////////////////////////////////////////////
    class GUISizeAnimation : public ParameterAnimation<GUIElement, Vector2>
    {
    public:
        GUISizeAnimation(GUIElement* el,
                         Vector2 from,
                         Vector2 to,
                         float duration,
                         InterpolationType interpolation = INTERPOLATION_LERP);
        
        GUISizeAnimation(GUIElement* el,
                         Vector2 to,
                         float duration,
                         InterpolationType interpolation = INTERPOLATION_LERP);
    };

}
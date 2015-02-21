////////////////////////////////////////////////////////////////////////////////
//  GUILabeledButton.h
//  Furiosity
//
//  Created by Bojan Endrovski on 11/09/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GUI.h"
#include "GUIContainer.h"
#include "GUILabel.h"
#include "GUIButton.h"

namespace Furiosity
{
    ///
    /// A quick way to create and manipulate a button and a label
    ///
    class GUILabeledButton: public GUIContainer
    {
    public:
        
        GUILabeledButton(Vector2 position,
                         const string& text,
                         const string&  fontName    = "/SharedResources/Fonts/Roboto-Regular-subset.ttf",
                         uint           fontSize    = 30);
        
        GUILabel*           Label() { return label; }
        
        GUIShrinkButton*    Button() { return button; }
        
    protected:
        
        GUILabel*           label;
        
        GUIShrinkButton*    button;
        
        GUILabeledButton(Vector2 position, GUILabel* label, GUIShrinkButton* button);
    };

}


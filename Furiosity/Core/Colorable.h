////////////////////////////////////////////////////////////////////////////////
//  Colorable.h
//  Furiosity
//
//  Created by Bojan Endrovski on 26/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Color.h"


namespace Furiosity
{
    ///
    /// A simple interface for anything that can have a color
    ///
    class Colorable
    {
    protected:
        
        /// Store the color as Color
        Color color;
        
    public:
        
        /// A constructor with a color, forcing init to a certain color
        Colorable(const Color& c) : color(c) {}
        
        /// Gets the color
        Color GetColor() const { return color;  }
        
        /// Set the color
        void SetColor(Color c) { color = c;     }
    };
}
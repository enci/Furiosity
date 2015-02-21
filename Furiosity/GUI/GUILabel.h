////////////////////////////////////////////////////////////////////////////////
//  GUILabel.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GUIImage.h"
#include "Label.h"

namespace Furiosity
{
    ///
    /// Label - the only element capable of rendering strings
    ///
    class GUILabel : public GUIImage
    {
    protected:
        // Text to be drawn
        //std::string                 text;
        
        // Internal label, most of the magic is inside
        Label* label;
        
        void Reloaded(const Label& res);
        
        void SetNewSize(Vector2 newsize);
        
    public:
        // Ctor
        GUILabel(Vector2        origin,
                 const string&  labelText,
                 const string&  fontName     = "/SharedResources/Fonts/Roboto-Regular-subset.ttf",
                 uint           fontSize     = 40,
                 float          maxWidth     = FLT_MAX);
        
        // Dtor
        virtual ~GUILabel();
        
        //virtual void Update(float dt) override;
        
        /// Change the text of this label
        /// Note: Internally this completely recreates the texture
        void SetText(const std::string& text);

        /// Change the font of this label
        /// Note: Internally this completely recreates the texture
        void SetFont( const std::string& fontName );
        
        /// Change the size of this label
        /// Note: Internally this completely recreates the texture
        void SetFontSize( const uint& fontSize );
        
        /// Change this label
        /// Note: Internally this completely recreates the texture
        void SetLabel(const std::string& text,
                      const std::string& fontName,
                      const uint& fontSize,
                      float maxWidth = FLT_MAX);
        //
        const string& Text() const { return label->OriginalText(); }
        
        void SetParent(GUIElement* p) { parent = p;}                
    };
}

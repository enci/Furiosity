////////////////////////////////////////////////////////////////////////////////
//  Font.h
//  Furiosity
//
//  Created by Gerard Meier on 4/15/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef FONT_H
#define FONT_H

#include "ResourceManager.h"
#include "Resource.h"

#include "stb_truetype.h"

// #include <ft2build.h>
// #include FT_FREETYPE_H


namespace Furiosity
{
    class Font : public Resource
    {
        friend class ResourceManager;
        
        
    private:
        std::string fontdata; // Why is this kept?
        
        stbtt_fontinfo fontinfo;
        
        // FT_Face face;
        
        // Library instance is shared among all labels. It is instantiated
        // the first time any label is generated.
        // static FT_Library library;
        
    private:
        Font(const std::string& filename);
        
        virtual ~Font();
        
    public:
        std::string& GetData()
        {
            return fontdata;
        }
        
        const stbtt_fontinfo& Fontinfo() const
        {
            return fontinfo;
        }
        
        /*i
        FT_Face GetFace()
        {
            return face;
        }
        
        FT_Library GetLibrary()
        {
            return Font::library;
        }
        */
    };
}

#endif

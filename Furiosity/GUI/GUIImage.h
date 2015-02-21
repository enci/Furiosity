////////////////////////////////////////////////////////////////////////////////
//  GUIImage.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GUIElement.h"

namespace Furiosity
{
    ///
    /// ImagePane A simple static image
    ///
    class GUIImage : public GUIElement
    {
    // protected:
        
    public: // TODO: Un-hack this
        // A texutre for the button
        Texture*     texture;
        
    protected:
        
        Vector2      fromUV;
        
        Vector2      toUV;
        
        // Creates a non defiend image, used for class that might want to handle
        // stuff them selves
        GUIImage(Vector2 origin,
                     Vector2 size) : GUIElement(origin, size) {}
        
    public:
        // Creates a new image with size and a RELATIVE path to the texutre
        GUIImage(Vector2 origin,
                     Vector2 size,
                     const string& texutrename,
                     bool mipmap    = false,
                     Vector2 from   = Vector2(0,0),
                     Vector2 to     = Vector2(1,1)) :
            GUIElement(origin, size),
            fromUV(from),
            toUV(to)
        {
            texture = gResourceManager.LoadTexture(texutrename, mipmap);
        }
        
        // Dtor
        virtual ~GUIImage()
        {
            gResourceManager.ReleaseResource(texture);
        }        
        
        // Draw simple image
        virtual void Render(SpriteRender& rend) override;
        
        virtual void HandleTouch(Touch* touch) override {}
        
        virtual void SetTexture(const string& texturename,
                                bool mipmap = false);
    };
    
    
    ///
    /// Animated Image  A simple animated image
    ///
    class GUIAnimatedImage : public GUIImage, public SpriteAnimator
    {
    public:
        GUIAnimatedImage(Vector2 origin,
                         Vector2 size,
                         const string& texutreName,
                         const string& spriteSheet,
                         bool mipmap    = false);
        
        GUIAnimatedImage(Vector2 origin,
                         Vector2 size,
                         const string& texutreName,
                         bool mipmap    = false);

        virtual ~GUIAnimatedImage() {}

        // Virtual update call
        virtual void Update(float dt) override;
        
        virtual void Render(SpriteRender& rend) override;
        
    };
}


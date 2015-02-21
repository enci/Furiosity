////////////////////////////////////////////////////////////////////////////////
//  GUIImage.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIImage.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                            - Image  -
//
////////////////////////////////////////////////////////////////////////////////
void GUIImage::Render(SpriteRender& rend)
{
    UpdateTransform();
    
    if(!Visible() || tint.a == 0)
        return;
    
    rend.DrawQuad(transform,
                  size.x,
                  size.y,
                  texture,
                  offset,
                  tint,
                  fromUV,
                  toUV);
}

void GUIImage::SetTexture(const string& texturename, bool mipmap)
{
    if(!texturename.empty())
    {
    	// Only load the image if it's a new image.
    	if(texture->Path() != texturename)
    	{
            gResourceManager.ReleaseResource(texture);
            texture = gResourceManager.LoadTexture(texturename, mipmap);
    	}
    }
}



////////////////////////////////////////////////////////////////////////////////
//
//                            - Animated Image -
//
////////////////////////////////////////////////////////////////////////////////
GUIAnimatedImage::GUIAnimatedImage(Vector2 origin,
                                   Vector2 size,
                                   const string& texutreName,
                                   const string& spriteSheet,
                                   bool mipmap) :
    GUIImage(origin,
             size,
             texutreName,
             mipmap,
             Vector2(),
             Vector2(1, 1)),
    SpriteAnimator(spriteSheet)
{}

GUIAnimatedImage::GUIAnimatedImage(Vector2 origin,
                                   Vector2 size,
                                   const string& texutreName,
                                   bool mipmap) :
    GUIImage(origin,
             size,
             texutreName,
             mipmap,
             Vector2(),
             Vector2(1, 1)),
    SpriteAnimator()
{}


void GUIAnimatedImage::Update(float dt)
{
    SpriteAnimator::Update(dt);
    GUIElement::Update(dt);
    //
    fromUV = SpriteAnimator::UVFrom();
    toUV  = SpriteAnimator::UVTo();
}

void GUIAnimatedImage::Render(SpriteRender& rend)
{
    UpdateTransform();
    
    if(!Visible() || tint.a == 0)
        return;
    
    Vector2 texoffset = SpriteAnimator::Offset().ComponentProduct(GUIElement::Size());
    rend.DrawQuad(transform,
                  size.x * SpriteAnimator::Size().x * 2,
                  size.y * SpriteAnimator::Size().y * 2,
                  texture,
                  texoffset,
                  tint,
                  fromUV,
                  toUV);
}

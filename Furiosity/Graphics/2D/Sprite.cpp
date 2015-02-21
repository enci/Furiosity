////////////////////////////////////////////////////////////////////////////////
//  Sprite.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 24/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Sprite.h"
#include "Entity2D.h"

using namespace Furiosity;


////////////////////////////////////////////////////////////////////////////////
//
//                              Sprite
//
////////////////////////////////////////////////////////////////////////////////
// Ctor
Sprite::Sprite(Transformable& parent,
               float width,
               float height,
               const string& filename,
               Vector2 offset,
               bool mipmap,
               Vector2 from,
               Vector2 to) :
    Colorable(Color::White),
    matrix( parent.Transform() ),
    width(width),
    height(height),
    offset(offset),
    uvFrom(from),
    uvTo(to),
    texture(0)
{
    texture = gResourceManager.LoadTexture(filename, mipmap);
}

// Ctor
Sprite::Sprite(Entity2D & entity,
               const string& filename,
               Vector2 offset,
               bool mipmap,
               Vector2 from,
               Vector2 to) :
    Colorable(Color::White),
    matrix( entity.Transform() ),
    offset(offset),
    texture(0)
{
    texture = gResourceManager.LoadTexture(filename, mipmap);
    //
    if(entity.GetCollisionShape()->ShapeEnum == COLLISION_SHAPE_BOX)
    {
        Box* box    = static_cast<Box*>(entity.GetCollisionShape());
        width       = box->halfWidth;
        height      = box->halfHeight;
    }
    else
    {
        width       = entity.BoundingRadius();
        height      = entity.BoundingRadius();
    }
    
    uvFrom = from;
    uvTo   = to;
}

// Ctor
Sprite::Sprite(Entity2D& parent, const XMLElement& settings) :
    Colorable(Color::White),
    Renderable(settings),
    matrix( parent.Transform() ),
    texture(0),
    height(0.0f),
    width(0.0f),
    uvFrom(0, 0),
    uvTo(1, 1)
{
    bool mipmap = false;
    const char* pMipmap = settings.Attribute("mipmap");
    if(pMipmap)
        mipmap = (*pMipmap == 't') ? true : false;
    
    // Get texture
    const char* pTexture = settings.Attribute("texture");
    assert(pTexture);
    texture = gResourceManager.LoadTexture(pTexture, mipmap);
    assert(texture);
    
    // Color for tinting
    const char* pColor = settings.Attribute("color");
    if(pColor) color = Color(pColor);
    
    // Render scale to unifromly scale the renderable
    float renderScale = 1.0f;
    const char* pRenderScale = settings.Attribute("renderScale");
    if(pRenderScale)
        renderScale = atof(pRenderScale);
    
    // Renderable height and width form XML
    const char* pHeight = settings.Attribute("height");
    const char* pWidth = settings.Attribute("width");
    //
    if(pWidth && pHeight)
    {
        height = atof(pHeight) * renderScale * 2;
        width = atof(pWidth) * renderScale * 2;
    }
    else
    {
        float radius = -1;
        settings.QueryAttribute("radius", &radius);
        if(radius == -1)
        {
            height = parent.BoundingRadius() * renderScale * 2;
            width = parent.BoundingRadius() * renderScale * 2;
        }
        else
        {
            height = radius * renderScale * 2;
            width = radius * renderScale * 2;
        }
    }
    
    const XMLElement* pFromUV = settings.FirstChildElement("fromUV");
    if(pFromUV)
        uvFrom = Vector2(pFromUV);
    
    const XMLElement* pToUV = settings.FirstChildElement("toUV");
    if(pToUV)
        uvTo = Vector2(pToUV);
}


// Dtor
Sprite::~Sprite()
{
    gResourceManager.ReleaseResource(texture);
}


void Sprite::Render(SpriteRender* render)
{
    render->DrawQuad(matrix,
                     width, height,
                     texture,
                     offset,
                     color,
                     uvFrom,
                     uvTo);
}

void Sprite::SetTexture(const string& texturename, bool mipmap)
{
    if(!texturename.empty())
    {
        gResourceManager.ReleaseResource(texture);
        texture = gResourceManager.LoadTexture(texturename, mipmap);
    }
}

/*
////////////////////////////////////////////////////////////////////////////////
//
//                             - Animated Sprite -
//
////////////////////////////////////////////////////////////////////////////////
AnimatedSprite::AnimatedSprite(Entity2D* entity,
                               const XMLElement* settings) :
Sprite(entity, settings)
{
    animator = new SpriteAnimator();
    
    // Grid height and width form XML
    const char* pGHeight = settings->Attribute("gridHeight");
    const char* pGWidth = settings->Attribute("gridWidth");
    //
    if(pGWidth && pGHeight)
    {
        uint gwidth = atof(pGWidth);
        uint gheight = atof(pGHeight);
        //
        animator->SetGridWidth(gwidth);
        animator->SetGridHeight(gheight);
    }
    
    
    // Animations from XML
    const XMLElement* pAnimEl = settings->FirstChildElement("animation");
    int i = 0;
    //
    while (pAnimEl)
    {
        // Get frame
        const char* pFrNum = pAnimEl->Attribute("framesNumber");
        assert(pFrNum);
        uint frnum = atoi(pFrNum);
        
        // Get frame
        // const char* pName = pAnimEl->Attribute("name"); If we decide for named animation
        // pName = 0;      // with the name now, doesn't mean you can leave it out
        assert(pFrNum); // Just because I don't know what to do
        
        
        animator->SetAnimationFramesNumber(i, frnum);
        
        //++
        pAnimEl = pAnimEl->NextSiblingElement("animation");
        i++;
    }
    
    animator->SetAnimation(0, true);
}

void AnimatedSprite::Update(float dt)
{
    animator->Update(dt);
    //
    uvFrom.x = animator->U0;
    uvFrom.y = animator->V0;
    //
    uvTo.x = animator->U1;
    uvTo.y = animator->V1;
    
}
*/


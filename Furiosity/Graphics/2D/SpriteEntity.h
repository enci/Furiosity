////////////////////////////////////////////////////////////////////////////////
//  SpriteEntity.h
//  Furiosity
//
//  Created by Bojan Endrovski on 02/08/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef SpriteEntity_h
#define SpriteEntity_h


#include "Entity2D.h"
#include "SpriteRender.h"
#include "Sprite.h"

namespace Furiosity
{
    class SpriteEntity : public Entity2D, public Sprite
    {
    protected:
        // Renderable*     sprite;
        // float           width;
        // float           height;
        
    public:
        SpriteEntity(Vector2 position,
                     float width,
                     float height,
                     const string& filename,
                     bool mipmap = false,
                     Vector2 from = Vector2(0, 0),
                     Vector2 to = Vector2(1, 1));
        
        SpriteEntity(const XMLElement& settings) :
            Entity2D(&settings),
            Sprite(*this, settings) {}
        
        virtual void Render(SpriteRender* render) { Sprite::Render(render); }
    };
    

    /*
    class AnimatedSpriteEntity : public Entity2D, public AnimatedSprite
    {
    public:
        AnimatedSpriteEntity(const XMLElement* settings) :
        Entity2D(settings),
        AnimatedSprite(this, settings) {}
        
        virtual void Render(SpriteRender* render) { AnimatedSprite::Render(render); }
        
        virtual void Update(float dt) { AnimatedSprite::Update(dt); }
    };
    */

}

#endif

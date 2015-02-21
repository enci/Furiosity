////////////////////////////////////////////////////////////////////////////////
//  Sprite.h
//  Furiosity
//
//  Created by Bojan Endrovski on 24/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SpriteRender.h"
#include "Transformable.h"
#include "Colorable.h"

namespace Furiosity
{
    ///
    /// A simple interface to do some rendering with
    ///
    class Sprite : public Renderable, public Colorable
    {
    private:
        // A matrix, defining where and how to draw stuff
        const Matrix33&     matrix;
        
    protected:
        
        // Rendering texture
        Texture*            texture;
                
        // Rendering width
        float               width;
        
        // Rendering height
        float               height;
        
        Vector2             offset;
        
        Vector2             uvFrom;
        
        Vector2             uvTo;
        
    public:
        Sprite(Transformable& parent,
               float width,
               float height,
               const string& filename,
               Vector2 offset   = Vector2(0, 0),
               bool mipmap      = false,
               Vector2 from     = Vector2(0, 0),
               Vector2 to       = Vector2(1, 1));
        
        Sprite(Entity2D & entity,
               const string& filename,
               Vector2 offset   = Vector2(0, 0),
               bool mipmap      = false,
               Vector2 from     = Vector2(0, 0),
               Vector2 to       = Vector2(1, 1));
        
        Sprite(Entity2D & entity, const XMLElement& settings);
        
        virtual ~Sprite();
        
        float   Width() const           { return width; }
        void    SetWidth(float w)       { width = w;    }
        
        float   Height() const          { return height;}
        void    SetHeight(float h)      { height = h;   }
        
        virtual void Render(SpriteRender* render) override;
        
        virtual void SetTexture(const string& texturename,
                                bool mipmap = false);
    };
    
    
    /*    
    ////////////////////////////////////////////////////////////////////////////////
    // Same as above but animated
    ////////////////////////////////////////////////////////////////////////////////
    class AnimatedSprite : public Sprite
    {
    protected:
        SpriteAnimator*     animator;
        
    public:
        AnimatedSprite(Entity2D* entity,
                       const XMLElement* settings);
        
        virtual ~AnimatedSprite() { SafeDelete(animator); }
        
        virtual void Update(float dt);
        
        void PlayAnimation(int idx, bool loop) { animator->SetAnimation(idx, loop); }
        
    };
    */
}
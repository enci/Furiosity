////////////////////////////////////////////////////////////////////////////////
//  SpriteEntity.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 8/2/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "SpriteEntity.h"

using namespace Furiosity;

SpriteEntity::SpriteEntity(Vector2 position,
                           float width,
                           float height,
                           const string& filename,
                           bool mipmap ,
                           Vector2 from,
                           Vector2 to) :
    Entity2D(),
    Sprite(*this,
           width,
           height,
           filename,
           Vector2(),
           mipmap,
           from,
           to)
{
    this->SetPosition(position);
}

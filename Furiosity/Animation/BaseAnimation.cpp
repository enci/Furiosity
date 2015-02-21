////////////////////////////////////////////////////////////////////////////////
//  BaseAnimation.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 25/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "BaseAnimation.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//                              Animation
////////////////////////////////////////////////////////////////////////////////
Animation::Animation(float duration) :
    duration(duration),
    time(0.0f),
    param(0.0f),
    loop(false)
{
    invduration = 1.0f / duration;
}

Animation::~Animation()
{}

void Animation::Update(float dt)
{
    if (time <= duration)
    {
        time += dt;
        param = time * invduration;
    }
    else if (loop)
    {
        time    = 0.0f;
        param   = time;
    }
}
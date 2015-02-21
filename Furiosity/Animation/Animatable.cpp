////////////////////////////////////////////////////////////////////////////////
//  Animatable.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 27/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Animatable.h"

using namespace std;
using namespace Furiosity;


void Animatable::AddAnimation(const string& name, AnimationShrPtr animation)
{
    animations[name] = animation;
}

void Animatable::AddAnimation(const string& name, Animation* animation)
{
    animations[name] = AnimationShrPtr(animation);
}


void Animatable::PlayAnimation(const string &name)
{
    if(animations.find(name) == animations.end()) return;
    
    auto anim = animations[name];
    assert(anim);
    
    currAnimationName = name;
    currAnimation = anim;
    currAnimation->Reset();
    // this->Update(0.0f);
}


AnimationShrPtr Animatable::GetAnimation(const string &name)
{
    if(animations.find(name) == animations.end())
        return AnimationShrPtr();
    return animations[name];
}

void Animatable::RemoveAnimation(const string &name)
{
    if(animations.find(name) != animations.end())
        animations.erase(name);
}

void Animatable::StopAnimation()
{
    currAnimationName.clear();
    currAnimation.reset();
}


void Animatable::Update(float dt)
{
    if(currAnimation.get() == 0) return;
    
    if(currAnimation->IsDone())
    {
        currAnimation.reset();
        currAnimationName.clear();
    }
    else
    {
        currAnimation->Update(dt);
    }
}


void Animatable::ClearAnimation()
{
    animations.clear();
    currAnimation.reset();
    currAnimationName.clear();
}


bool Animatable::HasAnimation(const std::string& name) const
{
    return animations.find(name) != animations.end();
}


// end
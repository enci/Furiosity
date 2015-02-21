////////////////////////////////////////////////////////////////////////////////
//  AnimationContainer.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 25/02/14.
//  Copyright (c) 2014 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AnimationContainer.h"

using namespace std;
using namespace Furiosity;



////////////////////////////////////////////////////////////////////////////////
//                          Animation Container
////////////////////////////////////////////////////////////////////////////////
AnimationContainer::AnimationContainer(initializer_list<Animation*> anims) : Animation(-1)
{
    for (auto a : anims)
        AddAnimation(AnimationShrPtr(a));
}


////////////////////////////////////////////////////////////////////////////////
//                              Reset
////////////////////////////////////////////////////////////////////////////////
void AnimationContainer::Reset()
{
    Animation::Reset();
    
    for (auto i = animations.begin(); i != animations.end(); ++i)
        (*i)->Reset();
}


////////////////////////////////////////////////////////////////////////////////
//                          Animation sequence
////////////////////////////////////////////////////////////////////////////////
AnimationSequence::AnimationSequence(initializer_list<Animation*> anims) :
    AnimationContainer(anims)
{
    duration = 0.0f;
    for (auto a : anims)
        duration += a->Duration();
}

void AnimationSequence::Update(float dt)
{
    if(animQueue.empty())
        return;
    
    AnimationShrPtr anim = animQueue.front();
    anim->Update(dt);
    
    // Current animation is done
    if(anim->IsDone())
    {
        animQueue.pop();                // Move to the next one
        if(animQueue.size() != 0)       // If there is one
        {
            anim = animQueue.front();   // Reset it if so
            anim->Reset();
            //            anim->Update(dt);
        }
        else if(loop)
            Reset();          // In last one and loop then reset
    }
}

void AnimationSequence::AddAnimation(AnimationShrPtr anim)
{
    AnimationContainer::AddAnimation(anim);
    animQueue.push(anim);
    
    duration += anim->Duration();
}

void AnimationSequence::Reset()
{
    AnimationContainer::Reset();
    //
    while(!animQueue.empty()) animQueue.pop();
    for (auto i = animations.begin(); i != animations.end(); ++i)
        animQueue.push((*i));
}


////////////////////////////////////////////////////////////////////////////////
//                      Animation Composition
////////////////////////////////////////////////////////////////////////////////
AnimationComposition::AnimationComposition(initializer_list<Animation*> anims) :
    AnimationContainer(anims)
{
    for (auto a : anims)
        if(a->Duration() > duration)
            duration = a->Duration();
}

void AnimationComposition::Update(float dt)
{
    Animation::Update(dt);
    
    for (AnimationShrPtr a : animations)
        a->Update(dt);
    
    
    if(IsDone() && loop)
        Reset();
}

void AnimationComposition::AddAnimation(AnimationShrPtr anim)
{
    AnimationContainer::AddAnimation(anim);
    //
    if(anim->Duration() > duration)
        duration = anim->Duration();
}

bool AnimationComposition::IsDone() const
{
    bool isDone = true;
    for (AnimationShrPtr a : animations)
        if (!a->IsDone())
            isDone = false;
    
    return isDone;
}

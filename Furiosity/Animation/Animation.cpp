////////////////////////////////////////////////////////////////////////////////
//  Animation.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 2/4/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Animation.h"

#include "AudioManager.h"
#include "SoundResource.h"
#include "ResourceManager.h"

using namespace std;
using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Event Animation
////////////////////////////////////////////////////////////////////////////////
EventAnimation::EventAnimation(function<void(void)> event) :
    Animation(0.016),
    fired(false),
    event(event)
{}

void EventAnimation::Update(float dt)
{
    // Prevent recursive firing if the update of this animaton
    // is called from the event
    bool f = fired;
    
    if(!loop)
        fired = true;
    
    if(!f)
         event();
}


////////////////////////////////////////////////////////////////////////////////
//                          Position Animation
////////////////////////////////////////////////////////////////////////////////
PositionAnimation::PositionAnimation(Transformable* obj,
                                     Vector2 from,
                                     Vector2 to,
                                     float duration,
                                     InterpolationType interpolation) :
    ParameterAnimation(obj,
                       &Transformable::SetPosition,
                       from,
                       to,
                       duration,
                       interpolation)
{}


PositionAnimation::PositionAnimation(Transformable* obj,
                                     Vector2 to,
                                     float duration,
                                     InterpolationType interpolation) :
    ParameterAnimation(obj,
                       &Transformable::Position,
                       &Transformable::SetPosition,
                       to,
                       duration,
                       interpolation)
{}


/*
////////////////////////////////////////////////////////////////////////////////
//                          Scale Animation
////////////////////////////////////////////////////////////////////////////////
ScaleAnimation::ScaleAnimation(Transformable* obj,
                               Vector2 from,
                               Vector2 to,
                               float duration,
                               InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Transformable::SetScale,
                   from,
                   to,
                   duration,
                   interpolation)
{}


ScaleAnimation::ScaleAnimation(Transformable* obj,
                               Vector2 to,
                               float duration,
                               InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Transformable::Scale,
                   &Transformable::SetScale,
                   to,
                   duration,
                   interpolation)
{}
 */


////////////////////////////////////////////////////////////////////////////////
//                          Rotation Animation
////////////////////////////////////////////////////////////////////////////////
RotationAnimation::RotationAnimation(Transformable* obj,
                                     float from,
                                     float to,
                                     float duration,
                                     InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Transformable::SetRotation,
                   from,
                   to,
                   duration,
                   interpolation)
{}


RotationAnimation::RotationAnimation(Transformable* obj,
                                     float to,
                                     float duration,
                                     InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Transformable::Rotation,
                   &Transformable::SetRotation,
                   to,
                   duration,
                   interpolation)
{}



////////////////////////////////////////////////////////////////////////////////
//                              Shake Animation
////////////////////////////////////////////////////////////////////////////////
ShakeAnimation::ShakeAnimation(Transformable*   obj,
                               float            intensity,
                               float            duration) :
    Animation(duration),
    obj(obj),
    intensity(intensity),
    originalPos(obj->Position()),
    originalIntensity(intensity)
{}

void ShakeAnimation::Update(float dt)
{
    Animation::Update(dt);
    if (IsDone())
        return;
    
    obj->SetPosition(originalPos + RandomOnUnitDisk() * intensity);
    //
    intensity *= 0.9f;
}

void ShakeAnimation::Reset()
{
    Animation::Reset();
    intensity = originalIntensity;
}

////////////////////////////////////////////////////////////////////////////////
//                              Play Animation
////////////////////////////////////////////////////////////////////////////////
PlayAnimation::PlayAnimation(Animatable& object, const string& animation) :
    Animation(0.016f),
    fired(false),
    animation(animation),
    object(object)
{}

void PlayAnimation::Update(float dt)
{
    if(!fired)
        object.PlayAnimation(animation);
    
    if(!loop)
        fired = true;
}


/*
////////////////////////////////////////////////////////////////////////////////
//                          Play Sound
////////////////////////////////////////////////////////////////////////////////
PlaySound::PlaySound(const std::string& soundName, float gain) :
    Animation(0.016f),
    fired(false),
    sound( gResourceManager.LoadSoundData(soundName) )
{
    if(gain > 0)
        sound->SetGain(gain);
}

PlaySound::~PlaySound() { gResourceManager.ReleaseResource(sound); }


void PlaySound::Update(float dt)
{
    // TODO: Fix this
    // if(!fired)
    //    gAudioManager.PlayBuffer(sound);
    
    if(!loop)
        fired = true;
}
*/


/*

////////////////////////////////////////////////////////////////////////////////
//                          Scale3D Animation
////////////////////////////////////////////////////////////////////////////////
Scale3DAnimation::Scale3DAnimation(Transformable3D* obj,
                                   Vector3 from,
                                   Vector3 to,
                                   float duration,
                                   InterpolationType interpolation) :
    ParameterAnimation(obj,
                       &Transformable3D::SetLocalScale,
                       from,
                       to,
                       duration,
                       interpolation)
{}


Scale3DAnimation::Scale3DAnimation(Transformable3D* obj,
                                   Vector3 to,
                                   float duration,
                                   InterpolationType interpolation) :
    ParameterAnimation(obj,
                       &Transformable3D::LocalScale,
                       &Transformable3D::SetLocalScale,
                       to,
                       duration,
                       interpolation)
{}



////////////////////////////////////////////////////////////////////////////////
//                          Position3D Animation
////////////////////////////////////////////////////////////////////////////////
Position3DAnimation::Position3DAnimation(Transformable3D* obj,
                                   Vector3 from,
                                   Vector3 to,
                                   float duration,
                                   InterpolationType interpolation) :
    ParameterAnimation(obj,
                       &Transformable3D::SetPosition,
                       from,
                       to,
                       duration,
                       interpolation)
    {}


Position3DAnimation::Position3DAnimation(Transformable3D* obj,
                                         Vector3 to,
                                         float duration,
                                         InterpolationType interpolation) :
    ParameterAnimation(obj,
                       &Transformable3D::Position,
                       &Transformable3D::SetPosition,
                       to,
                       duration,
                       interpolation)
{}
*/

////////////////////////////////////////////////////////////////////////////////
//                          Scale3D Animation
////////////////////////////////////////////////////////////////////////////////
Scale3DAnimation::Scale3DAnimation(Entity3D* obj,
                                   Vector3 from,
                                   Vector3 to,
                                   float duration,
                                   InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Entity3D::SetLocalScale,
                   from,
                   to,
                   duration,
                   interpolation)
{}


Scale3DAnimation::Scale3DAnimation(Entity3D* obj,
                                   Vector3 to,
                                   float duration,
                                   InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Entity3D::LocalScale,
                   &Entity3D::SetLocalScale,
                   to,
                   duration,
                   interpolation)
{}



////////////////////////////////////////////////////////////////////////////////
//                          Position3D Animation
////////////////////////////////////////////////////////////////////////////////
Position3DAnimation::Position3DAnimation(Entity3D* obj,
                                         Vector3 from,
                                         Vector3 to,
                                         float duration,
                                         InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Entity3D::SetPosition,
                   from,
                   to,
                   duration,
                   interpolation)
{}


Position3DAnimation::Position3DAnimation(Entity3D* obj,
                                         Vector3 to,
                                         float duration,
                                         InterpolationType interpolation) :
ParameterAnimation(obj,
                   &Entity3D::Position,
                   &Entity3D::SetPosition,
                   to,
                   duration,
                   interpolation)
{}




////////////////////////////////////////////////////////////////////////////////
//                  ColorAnimation
////////////////////////////////////////////////////////////////////////////////
ColorAnimation::ColorAnimation(Colorable* el,
                               Color from,
                               Color to,
                               float duration,
                               InterpolationType interpolation):
    ParameterAnimation(el,
                       &Colorable::SetColor,
                       from,
                       to,
                       duration,
                       interpolation) {}

ColorAnimation::ColorAnimation(Colorable* el,
                               Color to,
                               float duration,
                               InterpolationType interpolation):
    ParameterAnimation(el,
                       &Colorable::GetColor,
                       &Colorable::SetColor,
                       to,
                       duration,
                       interpolation) {}









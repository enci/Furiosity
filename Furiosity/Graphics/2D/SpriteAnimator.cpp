////////////////////////////////////////////////////////////////////////////////
//  SpriteAnimator.cpp
//
//  Created by Bojan Endrovski on 10/29/11.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "SpriteAnimator.h"

#include <assert.h>

// Fr
#include "Xml.h"
#include "ResourceManager.h"


using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Ctors
////////////////////////////////////////////////////////////////////////////////
SpriteAnimator::SpriteAnimator()
{ InitDefaults(); }

SpriteAnimator::SpriteAnimator(const string& spriteSheet,
                               const string& animationSheet)
{
    InitDefaults();
    
    Xml spriteSheetXml(spriteSheet);
    LoadSpriteSheet(spriteSheetXml->RootElement());
    
    // TODO: Load animations
}

SpriteAnimator::SpriteAnimator(const XMLElement& spriteSheet)
{
    InitDefaults();
    LoadSpriteSheet(spriteSheet);
}


// Default ctor, see cpp for defult values
//SpriteAnimator(const XMLElement& settings);


////////////////////////////////////////////////////////////////////////////////
// SetFramesPerSecond
////////////////////////////////////////////////////////////////////////////////
void SpriteAnimator::SetFramesPerSecond(int fps)
{
    this->fps = fps;
    timePerFrame = 1.0f / fps;
}

////////////////////////////////////////////////////////////////////////////////
// Set the current animation
////////////////////////////////////////////////////////////////////////////////
void SpriteAnimator::PlayAnimation(string name, bool loop)
{
    // Do a check first
    if(animations.find(name) == animations.end())
    {
        LOG("Animation \"%s\" not found.", name.c_str());
        return;
    }
    
    // Find animation
    currentAnimation = &animations[name];
    currentAnimationName = name;
    
    // Set params
    this->loop          = loop;
    this->isDone        = false;
    this->time          = 0;
    this->frameIndex    = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////////////////////
void SpriteAnimator::Update(float dt)
{
    if(!currentAnimation || isDone) 
        return;
    
    // Advance time
    time += dt;

    // Calculate frame in animation
    if(time > timePerFrame)
    {
        time = 0.0f;
        frameIndex++;
        
        // Loop animation
        // TODO: This a nice place for queueing animations
        if(frameIndex >= currentAnimation->size() - 1)
        {
            if(loop)
            {
                frameIndex = 0;
            }
            else
            {
                //frame = animationFrameLenghts[currentAnimation] - 1;
                frameIndex--;
                isDone = true;
            }
        }
    }
    
    currentFrame = currentAnimation->at(frameIndex);
}


////////////////////////////////////////////////////////////////////////////////
// LoadSpriteSheet
////////////////////////////////////////////////////////////////////////////////
void SpriteAnimator::LoadSpriteSheet(const XMLElement& settings)
{
    FramesSequence defaultAnimation;
    
    float width = settings.FloatAttribute("width");
    float height = settings.FloatAttribute("height");
    
    FOREACH_XML_ELEMENT_NAME(el, settings, "SubTexture")
    {
        // Basic elements
        float x = el->FloatAttribute("x");
        float y = el->FloatAttribute("y");
        float w = el->FloatAttribute("width");
        float h = el->FloatAttribute("height");

        // Cropping
        float xoffset  = 0;
        float yoffset = 0;
        float originalW = w;
        float originalH = h;
        //
        el->QueryFloatAttribute("xoffset", &xoffset);
        el->QueryFloatAttribute("yoffset", &yoffset);
        el->QueryFloatAttribute("originalWidth", &originalW);
        el->QueryFloatAttribute("originalHeight", &originalH);
        
        // Frame info
        Frame fr;
        fr.From = Vector2(x / width, y / height);
        Vector2 dimesions(w / width, h / height);
        fr.To   = fr.From + dimesions;
        //
        fr.Size.x = w / originalW;
        fr.Size.y = h / originalH;
        //
        float dX = xoffset - (originalW - w);
        float dY = yoffset - (originalH - h);
        fr.Offset.x = (dX * 0.5f) / originalW;
        fr.Offset.y = (dY * 0.5f) / originalH;
        
        // Add this frame to the default animation
        defaultAnimation.push_back(fr);
    }
    
    animations["default"] = defaultAnimation;
}


void SpriteAnimator::InitWithGrid(int rows, int columns)
{
    FramesSequence defaultAnimation;
    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // Frame info
            Frame fr;
            //
            //fr.From = Vector2((float)i / rows, (float)j / columns);
            //fr.To = Vector2((i+1.0f) / rows, (j+1.0f) / columns);
            
            fr.From = Vector2((float)j / rows, (float)i / columns);
            fr.To = Vector2((j+1.0f) / rows, (i+1.0f) / columns);
            //
            fr.Size.x = 1.0f / columns;
            fr.Size.y = 1.0f / rows;
            //
            fr.Offset = Vector2();
            
            // Add this frame to the default animation
            defaultAnimation.push_back(fr);
        }
    }
    animations["default"] = defaultAnimation;
}

void SpriteAnimator::SetAnimation(const string &animationName, vector<int> frames)
{
    const FramesSequence& defaultAnim = animations["default"];
    FramesSequence seq;
    
    for (int i : frames)
        seq.push_back(defaultAnim[i]);
    
    animations[animationName] = seq;
}

////////////////////////////////////////////////////////////////////////////////
// InitDefaults
////////////////////////////////////////////////////////////////////////////////
void SpriteAnimator::InitDefaults()
{
    fps                 = 15;
    timePerFrame        = 1.0f / fps;
    time                = 0;
    frameIndex          = 0;
    currentAnimation    = nullptr;
    loop                = false;
    isDone              = false;
}


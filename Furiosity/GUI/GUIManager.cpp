////////////////////////////////////////////////////////////////////////////////
//  GUIManager.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIManager.h"
#include "SoundResource.h"
#include "Defines.h"

using namespace Furiosity;

// Actualy creates a new UI Manager, but the real work is in the Init not the ctor
// This serves as a singletone UI Manager
GUIManager Furiosity::gGUIManager;


////////////////////////////////////////////////////////////////////////////////
//
//                           - GUIManager -
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Object invalid util intialized!
// Screen size set to 0x0!
////////////////////////////////////////////////////////////////////////////////
GUIManager::GUIManager() :
    GUIContainer(Vector2(), Vector2()),
    defaultButtonSnd(0)
// ,TouchHandler(false) // Static init is non deterministic, don't ask for input
{}

GUIManager::~GUIManager()
{
    //    gResourceManager.ReleaseResource(re)
}

////////////////////////////////////////////////////////////////////////////////
// HandleTouch from TouchHandler, just forwards to container
// TODO: Look for a smoother way to do this if possible
////////////////////////////////////////////////////////////////////////////////
void GUIManager::HandleTouch(Touch& touch)
{
    GUIContainer::HandleTouch(&touch);
}

////////////////////////////////////////////////////////////////////////////////
// Initialize with screen size
////////////////////////////////////////////////////////////////////////////////
void GUIManager::Initialize(float screenWidth, float screenHeight)
{
    UpdateTransform();
    
    if(!mask) mask = gResourceManager.LoadTexture("/SharedResources/Textures/square.png");
    
    
    // TODO: Consider this fixed positioning scheme
    camera = new Camera2D(screenWidth,      screenHeight,
                          0,                0,
                          true);
    
    size = camera->Window();
    
    renderer = new SpriteRender(camera);
    
#ifdef DEBUG
    debugDraw.Initialize(*camera);
#endif
    
    // Ask for input now
    gInputManager.Unsubscribe(camera);
    gInputManager.Subscribe(this);
    
}

////////////////////////////////////////////////////////////////////////////////
// Shutdown
////////////////////////////////////////////////////////////////////////////////
void GUIManager::Shutdown()
{
    gInputManager.Unsubscribe(this);
    //
    SafeDelete(defaultButtonSnd);
    SafeDelete(renderer);
    SafeDelete(camera);
}

////////////////////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////////////////////
void GUIManager::Update(float dt)
{
    GUIContainer::Update(dt);
    
    if(camera)
        camera->Update(dt);
}

////////////////////////////////////////////////////////////////////////////////
// Set Rotation - here to handle orientation crap
////////////////////////////////////////////////////////////////////////////////
void GUIManager::SetRotation(float rot)
{
    local.SetRotation(rot);
    if(rot < 0)
        local.SetTranslation(Vector2(size.x, 0));
    else
        local.SetTranslation(Vector2(0, size.y));
    
    UpdateTransform();
}

////////////////////////////////////////////////////////////////////////////////
// Render
// Calls the render for its container with the member renderer
////////////////////////////////////////////////////////////////////////////////
void GUIManager::Render()
{
    if(renderer != 0)
        GUIContainer::Render(*renderer);
}

////////////////////////////////////////////////////////////////////////////////
// SetDefaultButtonSound
////////////////////////////////////////////////////////////////////////////////
void GUIManager::SetDefaultButtonSound(const string &defaultButtonSound)
{
    if(defaultButtonSnd)
        SafeDelete(defaultButtonSnd);
    defaultButtonSnd = new Sound(defaultButtonSound);
}


#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
// Dbg draw
////////////////////////////////////////////////////////////////////////////////
void GUIManager::DebugDraw()
{
    debugDraw.Clear();
    GUIContainer::DebugDraw(debugDraw);
    debugDraw.Draw();
}
#endif

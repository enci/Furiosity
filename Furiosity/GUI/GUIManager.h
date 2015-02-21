////////////////////////////////////////////////////////////////////////////////
//  GUIManager.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// STD

//
#include "GUIContainer.h"
#include "SoundResource.h"

namespace Furiosity
{
    /////////////////////////////////////////////////////////////////////////////
    // A manger callass for all the UI on screen
    // Also handles a bit of the shader stuff
    /////////////////////////////////////////////////////////////////////////////
    class GUIManager : public GUIContainer /*, public TouchHandler*/
    {
    protected:
        // A sprite font to draw text with
        //        SpriteFont* defaultFont;
        
        Camera2D*           camera;
        
        SpriteRender*       renderer;
        
        //SoundResource*    defaultButtonSnd;
        Sound*                defaultButtonSnd = nullptr;
        
    public:
        // Do nothing ctor, static allocation only
        // Object invalid util intialized!
        GUIManager();
        virtual ~GUIManager();
        
        // Loads shaders for rendering and such
        void Initialize(float screenWidth,
                        float screenHeight);
        
        // Acts as dtor
        void Shutdown();
        
        // Update
        void Update(float dt) override;
        
        // Sets the rotation
        void SetRotation(float rot);
                
        // Do nothing default render method
        void Render();
        
        const Camera2D& Camera() const { return *camera; }
        
        // From TouchHandler
        virtual void HandleTouch(Touch& touch) override;
        virtual void DropTouches() override {}
        
        // Width
        float GetScreenWidth() const { return size.x; }
        void SetScreenWidth(float width) { size.x = width; }
        
        // Height
        float GetScreenHeight() const { return size.y; }
        void SetScreenHeight(float height) { size.y = height; }
        
        // Calculates a position in local coordinates
        Vector2     Local(const Vector2& vec) { return camera->Unproject(vec); }
        
        /// Sets a default sound for all buttons
        void SetDefaultButtonSound(const string& defaultButtonSound);
        
        Sound* DefaultButtonSound() const { return defaultButtonSnd; }
        
#ifdef DEBUG
        DebugDraw2D debugDraw;
        void DebugDraw();
#endif
    };
    
    // Global forward declaration
    extern GUIManager gGUIManager;
}
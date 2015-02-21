////////////////////////////////////////////////////////////////////////////////
//  GeneralManager.h
//
//  Created by Bojan Endrovski on 12/30/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "Device.h"

#ifdef IOS
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#endif

//class GLKViewController;

namespace Furiosity
{
    ////////////////////////////////////////////////////////////////////////////////
    // The General Manager (like in real life) doesn't do much, it merely calls
    // the other managers in the right order.
    // It also has a vague idea on what's happening, like screen resolution and
    // stuff like that. It's also the only singleton in town.    
    ////////////////////////////////////////////////////////////////////////////////
    class GeneralManager
    {
    private:
        float screenWidth;
        float screenHeight;
        
        bool fitToScreen;
        
        static GeneralManager* instance;

        Device device;

    public:
        // Dummy ctor and dtor
        GeneralManager() {}
        ~GeneralManager() {}
        
        // Actual dtor like call
        void Initialize(const std::string& resourcesPath,
                        const std::string& localPath,
                        const Device& device,
                        float GUISafeWidth,
                        float GUISafeHeight,
                        bool  fitToScreen   = true,
                        const std::string &remotePath = "");
        
        // Update all the managers
        void Update(float dt);
        
        // Shutdown the managers
        void Shutdown();
        
        float ScreenWidth() const { return screenWidth; };
        float ScreenHeight() const { return screenHeight; };
        
        const Device& GetDevice() { return device; }
        
        static GeneralManager* Instance();
        
        
#ifdef IOS
    protected:
        GLKViewController* view;
        
    public:
        void SetIOSView(GLKViewController* view);
        
        void ShowPopUp();

#endif
    };
    
    extern GeneralManager& gGeneralManager;
}

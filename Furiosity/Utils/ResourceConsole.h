////////////////////////////////////////////////////////////////////////////////
//  ResourceConsole.h
//  Furiosity
//
//  Created by Bojan Endrovski on 11/6/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Defines.h"

#ifdef IOS

#include "GUI.h"
#include "GUICarousel.h"

namespace Furiosity
{
    ///
    /// Data entry into the table of stuff to be reloaded
    ///
    struct ReloadData
    {
        int IsDone;
        bool AutoReload;
        
        ReloadData() { IsDone = 0, AutoReload = false; }
        ReloadData(int done, bool reload)
        { IsDone = done, AutoReload = reload; }
    };
    
    ///
    /// A GUI planel that lets you reload resources
    ///
    class ResourceConsole : public GUIContainer
    {
    private:
        
        GUIShrinkButton* openBtn;
        
        GUIContainer* console;
        
        map<Resource*, ReloadData> reloads;
        
        void ReloadResource(Resource* res, bool autoreload);
        
        float fontSize;
        
    public:
        ResourceConsole(Vector2 offset, float fontSize);
        virtual ~ResourceConsole(){}
        
        void Reload();
        
        void Update(float dt) override;
        
#ifdef DEBUG
        virtual void DebugDraw(DebugDraw2D& debugDraw) override {}
#endif

        
    private:
        
        void Load();
        
        void ReloadResource(Resource* res);
        
        void AutoReloadResource(Resource* res, bool isOn);
        
        static void ReloadedCallback(void* sender, void* data);
    };
}

#endif

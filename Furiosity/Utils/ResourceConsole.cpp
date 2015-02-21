////////////////////////////////////////////////////////////////////////////////
//  ResourceConsole.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 11/6/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "ResourceConsole.h"

#ifdef IOS

#include "Font.h"
#include "BackgroundDownloader.h"


using namespace Furiosity;

string fontfile = "SharedResources/Fonts/Roboto-Regular-subset.ttf";

bool CompareResources(Resource* lhs, Resource* rhs)
{
    return lhs->Path() < rhs->Path();
}

ResourceConsole::ResourceConsole(Vector2 offset, float fontSize) :
    GUIContainer(Vector2(), gGUIManager.Size()),
    console(0),
    fontSize(fontSize)
{
    // Cache font and never release it
    gResourceManager.LoadFont("/SharedResources/Fonts/Roboto-Regular-subset.ttf");
    
    
    // Open button
    openBtn = new GUIShrinkButton(Vector2(),
                                   Vector2(fontSize, fontSize),
                                   "/SharedResources/Textures/Icons/Window.png");
    openBtn->SetAnchoring(ANCHORING_TOP | ANCHORING_LEFT,
                          Vector2(fontSize * 0.5f + offset.x, fontSize * -0.5f + offset.y));
    //openBtn->SetCallback(this, ResourceConsole::LoadEvent, 0);
    openBtn->SetEvent( [this](){ Load(); } );
    openBtn->SetColor(Color(255,255,255,128));
    AddElement(openBtn);
    
    
}

void ResourceConsole::Reload()
{
    if(console)
    {
        RemoveAndDeleteElement(console);
        Update(0.0f);        
    }
    
    console = new GUIContainer(Vector2(), Size());
    AddElement(console);
    
    
    // Background
    auto bckg = new GUIImage(Vector2(), console->Size(), "SharedResources/Textures/square.png");
    bckg->SetColor(Color(0, 0, 0, 128));
    console->AddElement(bckg);
    
    
    // Refresh List button
    auto refreshListBtn = new GUIShrinkButton(Vector2(),
                                              Vector2(fontSize * 1.5f, fontSize * 1.5f),
                                              "SharedResources/Textures/Icons/Refresh.png");
//    refreshListBtn->SetCallback(this, ResourceConsole::ReloadEvent, 0);
    refreshListBtn->SetEvent( [this](){ Reload(); } );
    refreshListBtn->SetAnchoring(ANCHORING_TOP|ANCHORING_LEFT, Vector2(fontSize * 0.8f, fontSize * -0.8f));
    console->AddElement(refreshListBtn);
    
    
    //close button
    auto closeBtn = new GUIShrinkButton(Vector2(),
                                              Vector2(fontSize * 1.5f, fontSize * 1.5f),
                                              "SharedResources/Textures/Icons/Discard.png");
//    closeBtn->SetEvent(this, ResourceConsole::LoadEvent, 0);
    closeBtn->SetEvent( [this](){ Load(); } );
    closeBtn->SetAnchoring(ANCHORING_TOP|ANCHORING_RIGHT, Vector2(fontSize * -0.8f, fontSize * -0.8f));
    console->AddElement(closeBtn);
    
    //
    float buttonSize = fontSize * 1.5f;
    
    auto car = new GUICarousel(Vector2(0, buttonSize * -0.5f),
                               Vector2(Size().x, Size().y - buttonSize),
                               GUICarousel::ORIENTATION_VERTICAL);
    console->AddElement(car);
    
    float y = car->Size().y * 0.5f - buttonSize;
    int i = 0;
    //
    auto resources = gResourceManager.GetResources();
    stable_sort(resources.begin(), resources.end(), CompareResources);
    //
    for (auto res : resources)
    {
        i++;
        //
        auto bckg = new GUIImage(Vector2( 0, y ),
                                     Vector2( Width(), buttonSize),
                                     "SharedResources/Textures/square.png");
        bckg->SetColor((i%2==0) ? Color(0, 0, 0, 128) : Color("a77a3299"));
        car->AddElement(bckg);
        
        
        float x = Width() * -0.5f + buttonSize * 0.5f;
        
        if(res->Type() == RESOURCE_TYPE_LABEL ||
           res->Type() == RESOURCE_TYPE_RESOURCEPACK)
            continue;
        
        // Get path
        const char* cpath = res->Path().c_str();
        // Paths may contain the local prefix, which makes them unreadable.
        if(StringStartsWith(res->Path(), gResourceManager.GetPath("")))
        {
            // Cheap version of substring. Offset with the prefix.
            cpath += gResourceManager.GetPath("").length();
        }
        string path(cpath);
        
    
        // Reload button
        auto relbtn = new  GUIShrinkButton(Vector2(x, y),
                                           Vector2(buttonSize, buttonSize),
                                           "SharedResources/Textures/Icons/Save.png");
        //relbtn->SetCallback(this, ResourceConsole::ReloadResourceEvent, res);
        relbtn->SetEvent( [this, res](){ ReloadResource(res); } );
        car->AddElement(relbtn);
        
        x += buttonSize + 10;
        
        // Auto reload button
        auto autorelbtn = new GUIShrinkButton(Vector2(x, y),
                                              Vector2(buttonSize, buttonSize),
                                              "SharedResources/Textures/Icons/RefreshToggle.png",
                                              0.9f,
                                              true);
        // autorelbtn->SetCallback(this, ResourceConsole::AutoReloadResourceEvent, res);
        autorelbtn->SetEvent( [this, res](bool toggle){ AutoReloadResource(res, toggle); } );
        car->AddElement(autorelbtn);
        
        x += buttonSize * 0.8f;
        
        // If the resource is present, make sure the buttons reflects this  
        if(reloads.find(res) != reloads.end())
        {
            auto reldata = reloads[res];
            autorelbtn->SetToggled(reldata.AutoReload);
        }
        
        
        
        // Add label
        auto lbl = new GUILabel(Vector2(x, y),
                                path,
                                fontfile,
                                fontSize * 0.5f);
        lbl->Translate(Vector2(lbl->Size().x * 0.5f, 0));
        //lbl->SetColor( (i%2==0) ? textColor0 : textColor1 );
        car->AddElement(lbl);
        
        // Move to next row
        y -= buttonSize;
    }
    
    //car->SetWrap(x + sz);
}

void ResourceConsole::Update(float dt)
{
    GUIContainer::Update(dt);
    
    map<Resource*, ReloadData> reloadscopy = reloads;
    
    for (auto itr : reloadscopy)
    {
        Resource* res = itr.first;
        if(itr.second.IsDone == 1)
        {
        	// Release currently held (OpenGL) resources. This only
        	// works because on iOS resources are *always* valid.
        	res->Invalidate();

        	res->Reload(true);

            if(itr.second.AutoReload)
            {
                AutoReloadResource(res, true);
            }
            else
            {
                reloads.erase(res);
            }
        }
    }
}

void ResourceConsole::Load()
{
    //auto console = static_cast<ResourceConsole*>(sender);
    //
    if(console)
    {
        if(console->Visible())
        {
            console->Hide();
            openBtn->Show();
        }
        else
        {
            console->Show();
            openBtn->Hide();
        }
    }
    else
    {
        Reload();
        openBtn->Hide();
    }
}



void ResourceConsole::ReloadResource(Resource* res)
{
    const string& path = res->Path();
    
    DownloadInBackground(gResourceManager.GetRemotePath(path),
                         gResourceManager.GetCachedPath(path),
                         this,
                         ResourceConsole::ReloadedCallback,
                         res);
    
    reloads[res] = ReloadData(0, false);
}

void ResourceConsole::AutoReloadResource(Resource* res, bool isOn)
{
    if(isOn)
    {
        if(res->Type() != RESOURCE_TYPE_SHADER)
        {
            const string& path = res->Path();
        
            DownloadInBackground(gResourceManager.GetRemotePath(path),
                                 gResourceManager.GetCachedPath(path),
                                 this,
                                 ResourceConsole::ReloadedCallback,
                                 res);
        
            reloads[res] = ReloadData(0, true);
        }
        else
        {
            Shader* shader = dynamic_cast<Shader*>(res);
            const string& vpath = shader->VertexShaderPath();
            const string& fpath = shader->FragmentShaderPath();
            
            // Post reload for vertex shader
            DownloadInBackground(gResourceManager.GetRemotePath(vpath),
                                 gResourceManager.GetCachedPath(vpath),
                                 this,
                                 ResourceConsole::ReloadedCallback,
                                 res);

            // Post reload for fragment shader
            DownloadInBackground(gResourceManager.GetRemotePath(fpath),
                                 gResourceManager.GetCachedPath(fpath),
                                 this,
                                 ResourceConsole::ReloadedCallback,
                                 res);
            
            // Set the count to -1
            reloads[res] = ReloadData(-1, true);
        }
    } else {
        reloads[res] = ReloadData(0, false);
    }
}

void ResourceConsole::ReloadedCallback(void *sender, void *data)
{
    auto res = static_cast<Resource*>(data);
    auto console = static_cast<ResourceConsole*>(sender);
    //
    // res->Reload();
    console->reloads[res].IsDone += 1;
}


#endif

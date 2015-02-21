////////////////////////////////////////////////////////////////////////////////
//  World3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/16/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EntityContainer.h"
#include "Entity3D.h"
#include "Contact.h"
#include "Light3D.h"
#include "Camera3D.h"
#include "Renderer3D.h"

namespace Furiosity
{
    class World3D;
    
    ///
    /// A bit cleaner than having all the data in the world
    /// TODO: consider making weak pointers in this
    ///
    class RenderManager3D
    {
        friend class World3D;
        
    public:
        vector<Light3D*>        lights;
        
        vector<Camera3D*>       cameras;
        
        vector<Renderable3D*>   renderables;
        
        Camera3D*               activeCamera    = nullptr;
        
        int                     renderPass      = 0;
    };
    
    class World3D : public EntityContainer<Entity3D>
    {
    public:
        
        /// Adds and entity to the collection
        virtual void AddEntity(Entity3D* e) override;
        
        virtual void RemoveEntity(Entity3D* e) override;
        
        /// @see EntityContainer::Render
        virtual void Render();
        
        /// Updates the entities and handles collisions
        void virtual Update(float dt);
        
        /// This function must be overriden be each game
        virtual void HandleCollision(Contact3D& contact) = 0;
        
        /// Sets the currently active camera
        void SetActiveCamera(Camera3D* camera);
        
#ifdef DEBUG
        /// DebugDraws all the entities in the world
        virtual void DebugDraw();
#endif
        
    protected:
        
        RenderManager3D renderInfo;
        
        virtual void PrepareToRender();
        
        virtual void RenderPass();

        
    };
}
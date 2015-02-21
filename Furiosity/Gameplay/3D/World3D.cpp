////////////////////////////////////////////////////////////////////////////////
//  World3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/16/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "logging.h"
#include "World3D.h"
#include "DebugDraw3D.h"

using namespace Furiosity;
using namespace std;

void World3D::AddEntity(Entity3D* e)
{
//    e->world = this;
    EntityContainer::AddEntity(e);
}

void World3D::RemoveEntity(Entity3D* e)
{
    // Remove entity
    EntityContainer::RemoveEntity(e);
    
    // Also remove all childern
    for(auto c : *e)
        RemoveEntity(c);
}

void World3D::Render()
{
    PrepareToRender();
    RenderPass();
}

void World3D::Update(float dt)
{
    EntityContainer::Update(dt);
    
    vector<Entity3D*> colliders;
    
    for(Entity3D* e : entities)
        if (e->BoundingRadius() > 0 )
            colliders.push_back(e);    
    
    for (int i = 0; i < colliders.size() - 1; i++)
    {
        for (int  j = i + 1; j < colliders.size(); j++)
        {
            // Get the entities
            Entity3D* e0 = colliders[i];
            Entity3D* e1 = colliders[j];
            
            // Always order them in a way that is predicable for the
            // event handling. Smaller type goes first
            if(e0->EntityType() > e1->EntityType())
                swap(e0, e1);
            
            Vector3 normal = e0->Position() - e1->Position();
            float dist = normal.Magnitude();
            float radiussum = e0->BoundingRadius() + e1->BoundingRadius();
            if(dist < radiussum)
            {
                
                // Vector3 pos = (c0->Position() + c1->Position()) * 0.5f;
                normal.Normalize();
                float diff = radiussum - dist;
                Contact3D contact(e0, e1, 0.0f, normal, diff	);
                
                //LOG("Collision! \n");
                
                HandleCollision(contact);
                
                // Very, very, very crude collision resolving
                if(!contact.Resolved)
                {
                    Entity3D& first  = *contact.FirstBody;
                    Entity3D& second = *contact.SecondBody;
                
                    // The movement of each object is based on inverse mass, so total that
                    float totalRadius = first.BoundingRadius() + second.BoundingRadius();
                    
                    Vector3 move = contact.ContactNormal * contact.Penetration;
                    
                    Vector3 moveFst = move * (-first.BoundingRadius() / totalRadius);
                    Vector3 moveSec = move * (second.BoundingRadius() / totalRadius);
                    
                    first.SetPosition( first.Position() - moveFst );
                    second.SetPosition( second.Position() - moveSec );
                }
            }
        }
    }
}

void World3D::SetActiveCamera(Camera3D *camera)
{
    renderInfo.activeCamera = camera;
}

void World3D::PrepareToRender()
{
    renderInfo.lights.clear();
    renderInfo.cameras.clear();
    renderInfo.renderables.clear();
    
    for(Entity3D* e : entities)
    {
        // Collect all light
        Light3D* light = dynamic_cast<Light3D*>(e);
        if(light)
            renderInfo.lights.push_back(light);
        
        // Collect all cameras
        Camera3D* camera = dynamic_cast<Camera3D*>(e);
        if(camera)
            renderInfo.cameras.push_back(camera);
        
        // Collect all renderable objects
        Renderable3D* renderable = dynamic_cast<Renderable3D*>(e);
        if(renderable)
            renderInfo.renderables.push_back(renderable);
    }
    
    if(renderInfo.activeCamera == nullptr && renderInfo.cameras.size() > 0)
        renderInfo.activeCamera = renderInfo.cameras[0];
}

void World3D::RenderPass()
{
    for (Renderable3D* r : renderInfo.renderables)
    {
        if(r->TestLayer(renderInfo.activeCamera->RenderLayer()))
           r->Render(renderInfo);
    }
}



#ifdef DEBUG
void World3D::DebugDraw()
{
    for (Entity3D* e :  entities)
        e->DebugRender();    
}
#endif


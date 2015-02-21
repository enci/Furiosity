////////////////////////////////////////////////////////////////////////////////
//  GameWorld.cpp
//
//  Created by Bojan Endrovski on 26/09/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GameWorld.h"
#include "Furiosity.h"

#include <algorithm>

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Make a new gameworld
////////////////////////////////////////////////////////////////////////////////
GameWorld::GameWorld() : isRunning(true)
{
    manageCollisions = false;
    collisionManager = new CollisionManager(this, 300);
}

////////////////////////////////////////////////////////////////////////////////
// Destroy the world (muahahaha)
////////////////////////////////////////////////////////////////////////////////
GameWorld::~GameWorld()
{
    Clear();
    delete collisionManager;
}

////////////////////////////////////////////////////////////////////////////////
// Queues adding an entity
////////////////////////////////////////////////////////////////////////////////
void GameWorld::AddEntity(Entity2D* e)
{
    addQueue.push_back(e);
}

////////////////////////////////////////////////////////////////////////////////
// Queues adding an entity
////////////////////////////////////////////////////////////////////////////////
void GameWorld::RemoveEntity(Entity2D* e)
{
    // No double removes
    for (auto bge : removeQueue)
        if(bge == e)
            return;
    //
    removeQueue.insert(e);
    
    // Let the other entities know
    // EntityDeletedMessage msg(*e);
    // BroadcastMessage(msg);
}

////////////////////////////////////////////////////////////////////////////////
// Select entity (null otherwise)
////////////////////////////////////////////////////////////////////////////////
Entity2D* GameWorld::SelectClosestEntity(const Vector2& position)
{
	float mindist = MAXFLOAT;
	Entity2D* closest = NULL;
    for (auto bge : entities)
	{
		float dist = (bge->Position() - position).Magnitude();
		if (dist < bge->BoundingRadius() && dist < mindist)
		{
			mindist = dist;
			closest = bge;
		}
	}
	
	return closest;
}

////////////////////////////////////////////////////////////////////////////////
// SelectClosestEntityOfType (null otherwise)
////////////////////////////////////////////////////////////////////////////////
Entity2D* GameWorld::SelectClosestEntityOfType(const Vector2& position, int type)
{
	float mindist = MAXFLOAT;
	Entity2D* closest = NULL;
	
    for(auto bge : entities)
	{
		float dist = (bge->Position() - position).Magnitude();
		if (bge->EntityType() == type /* && dist < bge->BoundingRadius() */ && dist < mindist)
		{
			mindist = dist;
			closest = bge;
		}
	}
	
	return closest;
}

////////////////////////////////////////////////////////////////////////////////
// TagEntitiesWithinRange
////////////////////////////////////////////////////////////////////////////////
void GameWorld::TagEntitiesWithinRange(Entity2D*entity, float range)
{
    for(auto bge : entities)
	{
        // Skip self
        if(entity == bge)
            continue;
        
        // Clear all tags
        bge->UnTag();
        //
		float dist = (bge->Position() - entity->Position()).Magnitude();;
		if (dist < range)
            bge->Tag();
	}
}

/*
////////////////////////////////////////////////////////////////////////////////
// TagEntitiesWithinRange
////////////////////////////////////////////////////////////////////////////////
void GameWorld::TagEntitiesWithinRange(Entity2D *entity, float range)
{
    EntityIterator i = entities.begin();
	Entity2D* bge;
	//
	for (; i != entities.end(); ++i)
	{
        bge = *i;
        
        // Skip self
        if(entity == bge)
            continue;
        
        // Clear all tags
        bge->UnTag();
        //
		float dist = (bge->Position() - entity->Position()).Magnitude();;
		if (dist < range)
            bge->Tag();
*/      

////////////////////////////////////////////////////////////////////////////////
// GetEntityByID
////////////////////////////////////////////////////////////////////////////////
Entity2D* GameWorld::GetEntityByID(int id)
{
    for(auto bge : entities)
		if (bge->GetID() == id)
            return bge;
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// GetEntityByName
////////////////////////////////////////////////////////////////////////////////
Entity2D* GameWorld::GetEntityByName(string name)
{
    for (auto bge : entities)
		if (bge->Name().compare(name) == 0)
            return bge;
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////////////////////
void GameWorld::Update(float dt)
{
    if(!isRunning)
        return;
    
    // Add entities
    for (auto bge : addQueue)
        entities.push_back(bge);
    
    addQueue.clear();
        
    // Update entites
    for (auto bge : entities)
		bge->Update(dt);
    
    if(manageCollisions)
    {
        collisionManager->Clear();
        collisionManager->AccumulateContacts(entities);
        collisionManager->AccumulateContacts(entities, walls);
        collisionManager->RaiseContactEvents();
        collisionManager->ResolveContacts();
        collisionManager->ResolveVelocity();
    }
    
    // Remove after update so that no new entities have been added entites
    for(auto bge : removeQueue)
    {
        entities.remove(bge);           // Derefence j as it holds an pointer
        SafeDelete(bge);                // THIS is not a solution!!
    }
    removeQueue.clear();
}

void GameWorld::Clear()
{
    for(auto bge : entities)
		SafeDelete(bge);
    
    entities.clear();
    addQueue.clear();
    removeQueue.clear();
    walls.clear();

    Entity2D::ResetNextValidID();
}

////////////////////////////////////////////////////////////////////////////////
// Render
////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
void GameWorld::DebugRender()
{
    for (auto bge : entities)
		bge->DebugRender();
    
    for (auto w : walls)
        w.DebugRender();
}
#endif


void GameWorld::BroadcastMessage(const Message& message) const
{
    for (auto bge : entities)
		bge->HandleMessage(message);
}









// end


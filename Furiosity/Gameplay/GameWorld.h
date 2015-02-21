////////////////////////////////////////////////////////////////////////////////
//  GameWorld.h
//
//  Created by Bojan Endrovski on 26/09/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <vector>
#include <list>
#include <set>

#include "Entity2D.h"
#include "CollisionShapes.h"
#include "CollisionManager.h"
#include "Frmath.h"
#include "Contact.h"
#include "Messaging.h"

namespace Furiosity
{
    ///
    /// A container class for all entities in the game. Also there are walls for
    /// the level geometry.
    /// The world also has a mechanism for handling collision and collision events.
    /// Games using the engine should inherit this class for their levels.
    ///
    class GameWorld
	{
	public:
        typedef std::list<Entity2D*>      EntityContainer;
        
        typedef std::set<Entity2D*>       EntityRemoveQueue;
//        typedef EntityRemoveQueue::iterator     EntityRmQueueIterator;
        
        // All the entities in this world
        EntityContainer                 entities;
        
        // Queue so that adding can be done form the game loop itself
		std::vector<Entity2D*>    addQueue;

        // Queue so that removing can be done form the game loop itself
        EntityRemoveQueue               removeQueue;
        
         // Level geometry
        std::vector<LineSegment>        walls;
        
        // Ability to stop time in this world
        bool                            isRunning;
    
    public:
        
        CollisionManager*   collisionManager;
        
        bool                manageCollisions;
        
	public:
        // Ctor
		GameWorld();
        
        // Dtor
		virtual ~GameWorld();
		
		Entity2D* SelectClosestEntity(const Vector2& position);
        
		Entity2D* SelectClosestEntityOfType(const Vector2& position, int type);
        
        void TagEntitiesWithinRange(Entity2D* entity, float range);
        
        vector<Entity2D*> GetEntitiesWithinRange(Entity2D* entity, float range);
		
		void AddEntity(Entity2D* e);
        
		void RemoveEntity(Entity2D* e);
        
        Entity2D* GetEntityByID(int id);
        
        Entity2D* GetEntityByName(string name);
        
        //
        void AddWall(LineSegment wall) { walls.push_back(wall); }
        //
        const std::list<Entity2D*>& Entites() const { return entities; }
		
        // Gameplay goes here
		void virtual Update(float dt);
        
        CollisionManager* GetCollisionManager() const { return collisionManager; }
        
        // Gameplay goes here as well
        void virtual HandleContactEvent(Contact& contact) {}
        
        //bool virtual FilterCollision(Contact& contact) { return false; }
        
#ifdef DEBUG        
        // Eyes to know what is actually happening in this world
		void virtual DebugRender();
#endif
        
        // Mechanism to pause and resume play
        void Pause()        { isRunning = false; }
        void Play()         { isRunning = true;  }
        
        void Clear();
        
    protected:
        
        void BroadcastMessage(const Message& message) const;        
	};
}


#endif

////////////////////////////////////////////////////////////////////////////////
//  CollisionManager.h
//
//  Created by Boyan Endrovski on 7/27/11.
//  Copyright 2011 Boyan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <map>
#include <list>

// Local
#include "Contact.h"
#include "Entity2D.h"

using std::list;

namespace Furiosity 
{
    typedef unsigned long long uint64;
    
    class Grid
    {
    public:
        Grid(const list<Entity2D*>& entities);
                
        vector<Entity2D*> GetNeighbours(Entity2D& entity);
        
#ifdef DEBUG
        void virtual DebugRender();
#endif
        
    protected:
        int gridx;
        int gridy;
        
        //float xSize;
        //float ySize;
        
        //Vector2 center;
        Vector2 min;
        Vector2 max;
        
        vector<vector<vector<Entity2D*>>> grid;
    };
    
    // Fwd
    class GameWorld;
    
    ////////////////////////////////////////////////////////////////////////////////
    // Collision Manager
    // In a game things are bound to collide, this manager detects collision
    // resolves them in terms of velocity and penetration. The user of this class is
    // the world, so it can be used to go over multiple collections. The using class
    // can subscribe to event when entites collide.
    ////////////////////////////////////////////////////////////////////////////////
    class CollisionManager
    {        
    protected:        
        // Collection of contacts, these are shared and reused
        std::vector<Contact> contacts;
        
        // The next slot in the contact vector
        int next;
        
        // This is the maximum number of contacts supported
        // above this number collisions are ignored
        int maxContacts;
        
        // A vector of collision types to ignore
        std::vector<int> ignore;
        
        std::vector<uint64> ignorePairs;
        
//        int64
        
        // Pointer to the class that is handling the events
        GameWorld* gameWorld;
    
    protected:
        bool Ignore(int type);
        
        uint64 CalcPairID(uint id0, uint id1);
        
        bool Ignore(uint id0, uint id1);
        
    public:
        // Ctor
        CollisionManager(GameWorld* gameWorld, int maxContacts)
        : gameWorld(gameWorld),
          maxContacts(maxContacts),
          contacts(maxContacts),
          next(0)
          //ignore(16)
        {}
        
        // Dtor - do nothing as no dynamic memory is used
        ~CollisionManager() {}
        
        void Clear() { next = 0; }
        
        void IgnoreByType(int ignoreType) { ignore.push_back(ignoreType); }
                
        void IgnoreByIDs(uint id0, uint id1);
                
        void AccumulateContacts(const std::list<Entity2D*>& entities);
        
        void AccumulateContacts(const std::list<Entity2D*>& entities,
                                const std::vector<LineSegment>&     walls);

        void ResolveContacts();
        
        void ResolveVelocity();
            
        void RaiseContactEvents();
    };
}
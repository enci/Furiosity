////////////////////////////////////////////////////////////////////////////////
//  EntityContainer.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/16/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// STL
#include <vector>
#include <list>
#include <set>
#include <functional>

// FR
#include "Entity.h"


namespace Furiosity
{
    /// A container class for all entities in the game.
    /// Base class for all game worlds (and maybe GUI)
    template<class T>
    class EntityContainer
    {
    protected:
        
        typedef std::list<T*>      EntityInnerContainer;
        typedef std::set<T*>       EntityRemoveQueue;
        typedef std::vector<T*>    EntityAddQueue;
        
        /// All the entities in this world
        EntityInnerContainer            entities;
        
        /// Queue so that adding can be done form the game loop itself
        EntityAddQueue                  addQueue;
        
        /// Queue so that removing can be done form the game loop itself
        EntityRemoveQueue               removeQueue;
        
    public:
        // Ctor
        EntityContainer() {}
        
        // Dtor
        virtual ~EntityContainer();
        
        /// Adds and entity to the collection
        virtual void AddEntity(T* e);
        
        /// Removes an entity from this collection
        virtual void RemoveEntity(T* e);
        
        /// Updates the entities
        virtual void Update(float dt);
        
        /// Commits changes (add and remove) to the entities
        virtual void Commit();
        
        /// Removes and deletes all entities in this container
        void Clear();
        
        /// Get a single entity with an ID
        T* GetEntityByID(int id);
        
        /// Get the first entity with a name (names might not be unique)
        T* GetEntityByName(std::string name);
        
        /// Select entities that pass the selection filter
        std::vector<T*> SelectEntities(std::function<bool(T*)>);

        
        /// Select and cast entities that pass the selection filter
        template<class C>
        std::vector<C*> SelectAndCastEntities(std::function<bool(T*)>);

    protected:
        
        ///
        const EntityInnerContainer& Entites() const { return entities; }
        
        /// Send a message to all entities in the container
        virtual void BroadcastMessage(const Message& message) const;
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////
    //
    //                            - Implemetation -
    //
    ////////////////////////////////////////////////////////////////////////////////
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Destructor
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    EntityContainer<T>::~EntityContainer()
    {
        Clear();
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Queues adding an entity
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void EntityContainer<T>::AddEntity(T* e)
    {
        addQueue.push_back(e);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Queues adding an entity
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void EntityContainer<T>::RemoveEntity(T* e)
    {
        // No double removes
        for (auto entity : removeQueue)
            if(entity == e)
                return;
        //
        removeQueue.insert(e);
        
        // Let the other entities know
        // EntityDeletedMessage msg(*e);
        // BroadcastMessage(msg);
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // GetEntityByID
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    T* EntityContainer<T>::GetEntityByID(int id)
    {
        for(auto entity : entities)
            if (entity->GetID() == id)
                return entity;
        
        return 0;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // GetEntityByName
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    T* EntityContainer<T>::GetEntityByName(std::string name)
    {
        for (auto entity : entities)
            if (entity->Name().compare(name) == 0)
                return entity;
        
        return 0;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Select entities
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    std::vector<T*> EntityContainer<T>::SelectEntities(std::function<bool(T*)> filter)
    {
        std::vector<T*> selection;
        for (auto entity : entities)
            if (filter(entity))
                selection.push_back(entity);
        
        return selection;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Select and cast entities
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    template<class C>
    std::vector<C*> EntityContainer<T>::SelectAndCastEntities(std::function<bool(T*)> filter)
    {
        std::vector<C*> selection;
        for (auto entity : entities)
            if (filter(entity))
                selection.push_back(static_cast<C*>(entity));
        
        return selection;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Update
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void EntityContainer<T>::Update(float dt)
    {
        // Add entities
        for (auto e : addQueue)
        {
            entities.push_back(e);
            e->Added();
        }
        addQueue.clear();
        
        // Update entites
        for (auto entity : entities)
            entity->Update(dt);
        
        // Remove after update so that no new entities have been added
        for(auto entity : removeQueue)
        {
            entities.remove(entity);        // Derefence j as it holds an pointer
            SafeDelete(entity);             // THIS is (not) a solution
        }
        removeQueue.clear();
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Commit
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void EntityContainer<T>::Commit()
    {
        // Add entities
        for (auto e : addQueue)
        {
            entities.push_back(e);
            e->Added();
        }
        addQueue.clear();
        
        // Remove after update so that no new entities have been added
        for(auto entity : removeQueue)
        {
            entities.remove(entity);        // Derefence j as it holds an pointer
            SafeDelete(entity);             // THIS is (not) a solution
        }
        removeQueue.clear();
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Clear
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void EntityContainer<T>::Clear()
    {
        // Clear current entities
        for(auto e : entities)
            SafeDelete(e);
        entities.clear();
        
        // Clear enities that were to be added
        for(auto e : addQueue)
            SafeDelete(e);
        addQueue.clear();
        
        // Clear remove queue
        for(auto e : removeQueue)
            SafeDelete(e);
        removeQueue.clear();
        
        //Entity::ResetNextValidID();
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // BroadcastMessage
    ////////////////////////////////////////////////////////////////////////////////
    template<class T>
    void EntityContainer<T>::BroadcastMessage(const Message& message) const
    {
        for (auto bge : entities)
            bge->HandleMessage(message);
    }

}




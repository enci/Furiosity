////////////////////////////////////////////////////////////////////////////////
//  Resource.h
//
//  Created by Bojan Endrovski on 6/24/11.
//  Copyright 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <map>
#include <functional>

#include "Countable.h"
#include "ResourceManager.h"

namespace Furiosity
{
    class ResourceManager;
    
    /// All supported resource types
    enum ResourceType
    {
        RESOURCE_TYPE_TEXTURE      = 1 << 0,
        RESOURCE_TYPE_SOUND        = 1 << 1,
        RESOURCE_TYPE_XML          = 1 << 2,
        RESOURCE_TYPE_LABEL        = 1 << 3,
        RESOURCE_TYPE_SHADER       = 1 << 4,
        RESOURCE_TYPE_FONT         = 1 << 5,
        RESOURCE_TYPE_RESOURCEPACK = 1 << 6,
        RESOURCE_TYPE_SVG          = 1 << 7,
        RESOURCE_TYPE_MESH         = 1 << 8,
    };
    
    ///
    /// Base class to all resources, gives book keeping option to the manager
    ///
    /// template <typename T>
    class Resource : public Countable<Resource>
    {
        friend class ResourceManager;
        
    protected:
        
        /// This is how the manager tracks the resources
        unsigned long               resourceID;
        
        /// Saves the path for runtime reloading of assets
        std::string                 resourcePath;
        
        /// Size in memory (not on disk) in bytes
        unsigned int                size;
        
        /// The type of this resource, as set by the constructor.
        ResourceType                type;
        
        /// Type for the resource reload callback
        typedef std::function<void(const Resource& res)> Callback;
        
        /// Collection of callbacks for reloading the resource
        std::map<void*, Callback>   callbacks;
        
    protected:
        
        /// Protected ctor, as resources are handled by the ResourceManager.
        ///
        /// @param type the resource type of this resource.
        ///
        Resource(ResourceType type) : type(type), resourceID(0), size(0) { }
        
        /// Protected dtor, as resources are handled by the ResourceManager.
        virtual ~Resource() {}
        
    public:
        
        /// Get the id for times where is better to use it instead of a
        /// pointer to the resource.
        ///
        unsigned long ResourceID() const            { return resourceID; }
        
        /// Retrieve the type of this resource, as set by constructor.
        ///
        /// @returns the resource type.
        ///
        ResourceType Type() const                   { return type; }
        
        /// Reload the inner workings of this resource. If the resource
        /// is still valid, any data will be overriden causing a memory
        /// leak. Always call this from the subclasses to call all reload
        /// subscribers.
        ///
        virtual void Reload(bool cached = false)
        // { for(auto p : callbacks) p.second( static_cast<const T&>(*this)); }
        { for(auto p : callbacks) p.second(*this); }
        
        /// Determine whether or not this resource is valid. E.g., a GL context
        /// was lost, and this resource requires resending the texture data
        /// to the GPU.
        ///
        virtual bool IsValid()                      { /* Feel free to override. */ return true; }
        
        /// Forcibily invalidate this resource. E.g., when reloading labels
        /// to reflect text changes. It is common to follow an Invalidate call
        /// by a Reload call.
        ///
        /// @see Reload
        /// @see IsValid
        /// 
        virtual void Invalidate() { /* Feel free to override */ }
        
        /// Returns the path to this resource.
        ///
        /// @returns the resource path.
        ///
        const std::string& Path() const             { return resourcePath; }
        
        /// Adds a callback event for realoading the resource
        ///
        /// @param sender Handler for this callback and a lookup for the callback map
        /// @param lambda Callback function
        ///
        void AddReloadEvent(void* sender, Callback lambda)
        { callbacks[sender] = lambda; }
        
        /// Every AddReloadEvent must be matched with a RemoveReloadEvent
        /// otherwise there will be a dangling pointer in the callback list.
        ///
        /// @param sender Handler for this callback and a lookup for the callback map
        ///
        void RemoveReloadEvent(void* sender)
        { callbacks.erase(sender); }
    };
    
    ///
    /// Base class to all resources, gives book keeping option to the manager
    ///
    template <typename T>
    class ResourceHandle
    {
    protected:
        
        /// Type for the resource reload callback
        typedef std::function<void(const T& res)> ReloadCallback;
        
        /// Pointer to resource
        T* resource;
        
        //ReloadCallback callback;
        
        /// Can't copy a handle
        ResourceHandle(const ResourceHandle&) = delete;
        
        /// Can't copy a handle
        ResourceHandle& operator=(const ResourceHandle&) = delete;
        
        /// Can't just create a handle. This is basically a virtual class
        ResourceHandle(T* resource);
        
    public:
        
        /// Destroy
        virtual ~ResourceHandle();
        
        /// Set a callback event for reloading the resource
        ///
        /// @param lambda Callback function
        ///
        void SetReloadEvent(ReloadCallback reloadCallback);
        
        /// Allow for the usage of resource handles as resource pointers
        const T* operator->() const { return resource; }
        
        /// Allow for the usage of resource handles as resource pointers
        T* operator->() { return resource; }
    };
    
    template<typename T>
    ResourceHandle<T>::ResourceHandle(T* resource)
    {
        this->resource = resource;
    }
    
    template<typename T>
    ResourceHandle<T>::~ResourceHandle()
    {
        resource->RemoveReloadEvent(this);
        gResourceManager.ReleaseResource(resource);
    }
    
    template<typename T>
    void ResourceHandle<T>::SetReloadEvent(ReloadCallback reloadCallback)
    {
        resource->AddReloadEvent(this, [this, reloadCallback](const Resource& res)
        {
            reloadCallback(static_cast<const T&>(res));
        });
    }
}

#endif

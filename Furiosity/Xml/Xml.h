////////////////////////////////////////////////////////////////////////////////
//  XmlResource.h
//
//  Created by Bojan Endrovski on 12/11/11.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>

#include "Resource.h"
#include "XmlMacros.h"

using namespace tinyxml2;

namespace Furiosity
{
    class ResourceManager;
    
    ///
    /// A generic xml resource, so that settings are loaded only once and
    /// then cached for later use.
    ///
    class XmlResource : public Resource
    {
        typedef void (*Callback)(void* sender, XmlResource& data);
        
        std::map<void*, Callback> callbacks;
        
        /// Resource's lifetime is handeled by the resource manager
        friend class ResourceManager;
        
    protected:
        
        /// The document that this resource is connected with
        XMLDocument*        doc;
        
        /// A quick handle to the root
        const XMLElement*   root;
        
    protected:
        /// Default ctor, prepares this XmlResource for data injection later on. 
        XmlResource();
    
        /// Ctor - loads the DOM in memory and that's about it
        XmlResource(const std::string& filename);
        
        /// Releases the handle to the DOM, file is released in ctor
        ~XmlResource() { delete doc; }
        
        XmlResource* LoadData(const std::string& data);
        
    public:
        /// Accessor for the document
        const XMLDocument* Document() const { return doc; }
        
        /// Accessor for the element
        const XMLElement&  RootElement() const { return *root; }
        
        /// Reload and call subscribers
        virtual void Reload(bool cached = false) override;
    };
    
    
    ///
    /// A convinient handle for the the XmlResource with automatic
    /// memory management and event subscription. It's a bad name,
    /// but it's short and covinient.
    ///
    class Xml : public ResourceHandle<XmlResource>
    {
        
    public:
        /// Create a new Xml from a file
        Xml(const string& name);
    };
}

////////////////////////////////////////////////////////////////////////////////
//  XmlResource.h
//
//  Created by Bojan Endrovski on 12/11/11.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Xml.h"
#include "ResourceManager.h"
#include "FileIO.h"
#include <assert.h>

using namespace std;
using namespace Furiosity;


 XmlResource::XmlResource() : Resource(RESOURCE_TYPE_XML), doc(0), root(0) {
 
 }

////////////////////////////////////////////////////////////////////////////////
// Ctor
// Loads the xml into memory
////////////////////////////////////////////////////////////////////////////////
XmlResource::XmlResource(const std::string& filename) :
    Resource(RESOURCE_TYPE_XML), doc(0), root(0)
{
    // This not a vrtual call
    resourcePath = filename;
    Reload(false);
}

XmlResource* XmlResource::LoadData(const std::string& data)
{
    if(doc != 0)
        delete doc;
    
    doc = new XMLDocument();
    doc->Parse(data.c_str());
    
    size = (int)data.length();
    root = doc->FirstChildElement();
    
    if(!root)
        ERROR("Failed to parse XML");
    
    return this;
}


void XmlResource::Reload(bool cached)
{
    // Get latest file
    string filename = gResourceManager.GetLatestPath(resourcePath);
    
    // Read the while file into memory    
    const string& data = Furiosity::ReadFile(filename);
    
    // Load XML
    LoadData(data);

    // Call substibers
    Resource::Reload(cached);
    
    // Call substibers
    for(std::pair<void*, Callback> p : callbacks)
        p.second(p.first, *this);
}


////////////////////////////////////////////////////////////////////////////////
//                                  Xml
////////////////////////////////////////////////////////////////////////////////
Xml::Xml(const string& name) : ResourceHandle<XmlResource>(gResourceManager.LoadXmlResource(name)){}

#pragma once

#include "Resource.h"
#include "Xml.h"
#include <vector>
#include "ResourceManager.h"

namespace Furiosity
{

class ResourcePack : public Resource
{
    friend class ResourceManager;
    
private:
    std::vector<Resource*> resources;
    void LoadResource(const XMLElement* element);

    /// Convienent helper.
    static const XMLAttribute* GetAttribute(const XMLElement* element, const std::string& key);
    
protected:
    ResourcePack(XmlResource* xml);
    virtual ~ResourcePack();
};

}
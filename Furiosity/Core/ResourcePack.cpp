#include "ResourcePack.h"
#include "Shader.h"
#include "Font.h"
#include "Label.h"
#include "SoundResource.h"

using namespace Furiosity;


ResourcePack::ResourcePack(XmlResource* xml) : Resource(RESOURCE_TYPE_RESOURCEPACK)
{
    const XMLElement& root    = xml->RootElement();
    const XMLElement* setting = root.FirstChildElement("resource");
    
    while(setting != 0)
    {
        LoadResource(setting);

        setting = setting->NextSiblingElement("resource");
    }
}

ResourcePack::~ResourcePack()
{
    for(Resource* resource : resources)
    {
        gResourceManager.ReleaseResource(resource);
    }
}

const XMLAttribute* ResourcePack::GetAttribute(const XMLElement* element, const std::string& key)
{
    const XMLAttribute* attribute = element->FindAttribute(key.c_str());
        
    if(attribute == 0) {
        LOG("ResourcePack::GetAttribute() Unable to get attibute '%s' from XML. Resource type: '%s' \n", key.c_str(), element->Attribute("type"));
        
        assert(0);
    }
    
    return attribute;
}

void ResourcePack::LoadResource(const XMLElement* element)
{
    std::string type = GetAttribute(element, "type")->Value();
    
    Resource* resource = 0;
    
#ifdef DEBUG
    LOG("Preloading: %s \n", type.c_str());
#endif
    
    if(type.compare("texture") == 0)
    {
        std::string file = GetAttribute(element, "file")->Value();

        resource = gResourceManager.LoadTexture(file);
    }
    else if(type.compare("sound") == 0)
    {
        std::string file = GetAttribute(element, "file")->Value();
        
        resource = gResourceManager.LoadSoundData(file);
    }
    else if(type.compare("shader") == 0)
    {
        std::string vertex   = GetAttribute(element, "vertex")->Value();
        std::string fragment = GetAttribute(element, "fragment")->Value();
        
        resource = gResourceManager.LoadShader(vertex, fragment);
    }
    else if(type.compare("font") == 0)
    {
        std::string ID = GetAttribute(element, "ID")->Value();
        
        // Lookup the text (font) corresponding to the ID:
        std::string text = gResourceManager.GetString(ID);
        
        resource = gResourceManager.LoadFont(text);
    }
    else if(type.compare("label") == 0)
    {
        std::string ID = GetAttribute(element, "ID")->Value();
        std::string fontfile = GetAttribute(element, "fontfile")->Value();
        int fontsize = GetAttribute(element, "fontsize")->IntValue();
        
        // Lookup the text corresponding to the ID:
        std::string text = gResourceManager.GetString(ID);
        
        resource = gResourceManager.LoadLabel(text, fontfile, fontsize);
    }
    else if(type.compare("xml") == 0)
    {
        std::string file = GetAttribute(element, "file")->Value();
        
        resource = gResourceManager.LoadXmlResource(file);
    }
    else if(type.compare("resourcepack") == 0)
    {
        std::string file = GetAttribute(element, "file")->Value();
        
        resource = gResourceManager.LoadResourcePack(file);
    }
    else
    {
        LOG("ResourcePack::LoadResource() No handler for type '%s' found \n", type.c_str());
        assert(0);
    }
    
    resources.push_back(resource);
}

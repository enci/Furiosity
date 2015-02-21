////////////////////////////////////////////////////////////////////////////////
//  Texture.h
//  Furiosity
//
//  Created by Bojan Endrovski on 19/10/2011.
//  Copyright (c) 2011 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include <string>
#include <assert.h>


#include "gl.h"
#include "logging.h"
#include "Resource.h"
#include "Defines.h"


namespace Furiosity
{
    class ResourceManager;
    class FontManager;
    
    class Texture : public Resource
    {
        // Resource's lifetime is handeled by the resource manager
        friend class ResourceManager;
        friend class FontManager;
        
    private:
        // const std::string filename;

    public:
        GLuint      name;
        uint32_t    width;
        uint32_t    height;
        GLenum      internalFormat;
        GLboolean   hasAlpha;
        bool        genMipMap;

        // As resource no public ctor and dtor
        Texture() : Resource(RESOURCE_TYPE_TEXTURE) {}
        Texture(const std::string& _filename, bool _genMipMap = false);
        virtual ~Texture();
        
        // Load PVR file
        bool LoadPVR(const std::string& filename, bool genMipMap);
        
        // Load PNG image file
        bool LoadPNG(const std::string& filename, bool genMipMap);

        // Load DDS image file (Android's GL_EXT_texture_compression_dxt1) (nvidia Tegra)
        bool LoadDDS(const std::string& filename, bool genMipMap);

        // Load AMD ATI Texture Compressed assets.
        bool LoadATC(const std::string& filename, bool genMipMap);

        virtual void Reload(bool cached = false) override;
        virtual bool IsValid() override;
        virtual void Invalidate() override;
        
        uint Width() const { return width;      }
        
        uint Height() const { return height;    }
    };
}

#endif

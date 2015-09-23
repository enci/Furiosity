////////////////////////////////////////////////////////////////////////////////
//  FXParticles2D.h
//
//  Created by Bojan Endrovski on 29/05/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Framework includes
#include "gl.h"

// Local includes
#include "Frmath.h"
#include "Color.h"
#include "Camera2D.h"
#include "Texture.h"
#include "Shader.h"
//
#include "BaseFXParticles.h"
#include "FXParticle.h"

typedef unsigned short ushort;              // 16-bit integer

namespace Furiosity
{
    template<class ParticleT>
    class FXParticleManager2D : public BaseFXParticleManager<ParticleT, Vector2>
    {
    protected:
        
        const Camera2D* camera;
        
        // OpenGL stuff ////////////////////////////////////
        
        enum Uniform
        {
            UNIFORM_TEXSAMPLER,     // Texture sampler
            UNIFORM_PROJECTION,     // Camera projection
            UNIFORM_ZOOM,           // Camera zoom
            UNIFORM_COUNT           // Counter
        };
        
        /// Declaration
        static GLint uniforms[];
                
        Texture* texture;
                
        /// Compiles both shaders and loads them
		bool LoadShaders();

		Shader* shader;
		GLint attribColor;
		GLint attribPosition;

    public:
        /// Type of the particles.
        // typedef ParticleT ParticleType;
    
        static void ShadersReloaded(void* sender, Shader& shader) {
            static_cast<FXParticleManager2D*>(sender)->LoadShaders();
        }

        /// Constructor, for constructing.
        FXParticleManager2D(ushort max,
                            const Camera2D* camera,
                            const string& texfile = "/SharedResources/Textures/particle.png");
        
        /// Destructor, for destructing.
        ~FXParticleManager2D();
                
        /// Draws all particles.
        void Render();        
    };
    
    typedef FXParticle<Vector2> FXParticle2D;
    
    typedef FXParticleManager2D<FXParticle2D> FXParticleSystem2D;
}

// Inline code inclusion due to templates.
#include "FXParticles2D.hpp"
    
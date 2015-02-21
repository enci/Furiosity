////////////////////////////////////////////////////////////////////////////////
//  FXParticles3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 10/19/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Framework includes
#include "gl.h"

// Local includes
#include "BaseFXParticles.h"
#include "FXParticle.h"
#include "Frmath.h"
#include "Camera3D.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer3D.h"
#include "Entity3D.h"
#include "World3D.h"

namespace Furiosity
{
    template<class ParticleT>
    class FXParticleManager3D : public BaseFXParticleManager<ParticleT, Vector3>,
                                public Entity3D,
                                public Renderable3D
    {
    protected:
//        const Camera3D& camera;
        
        Texture* texture;
        
        Effect shader;
        ShaderAttribute& attribColor;
        ShaderAttribute& attribPosition;
        ShaderParameter& paramTexture;
        ShaderParameter& paramWVP;
        ShaderParameter& paramZoom;
        
    public:
        
        /// Constructor, for constructing.
        FXParticleManager3D(World3D* world,
                            ushort max,
//                            const Camera3D& camera,
                            const string& texfile = "/SharedResources/Textures/particle.png");
        
        /// Destructor, for destructing.
        ~FXParticleManager3D();
        
        /// Draws all particles.
//        void Render();
        
        virtual void Update(float dt) override
        { BaseFXParticleManager<ParticleT, Vector3>::UpdateParticles(dt); }
        
        virtual void Render(RenderManager3D& renderManager) override;
    };
    
    typedef FXParticle<Vector3> FXParticle3D;
    
    typedef FXParticleManager3D<FXParticle3D> FXParticleSystem3D;
}


// Inline code inclusion due to templates.
namespace Furiosity
{
    ////////////////////////////////////////////////////////////////////////////////
    // Ctor
    ////////////////////////////////////////////////////////////////////////////////
    template <class ParticleT>
    FXParticleManager3D<ParticleT>::FXParticleManager3D(World3D* world,
                                                        ushort max,
                                                        const string& texfile) :
        BaseFXParticleManager<ParticleT, Vector3>(max),
        Entity3D(world, nullptr	, -1),
        shader("/SharedResources/Shaders/Particles3D.vsh",
               "/SharedResources/Shaders/Particles3D.fsh"),
        attribColor(    *shader->GetAttribute("a_color")),
        attribPosition( *shader->GetAttribute("a_position")),
        paramTexture(   *shader->GetParameter("s_texture")),
        paramWVP(       *shader->GetParameter("u_worldviewproj")),
        paramZoom(      *shader->GetParameter("u_zoom"))
    {
        // Load texture
        texture = gResourceManager.LoadTexture(texfile, true);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Dtor
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    FXParticleManager3D<T>::~FXParticleManager3D()
    {
        gResourceManager.ReleaseResource(texture);
    }
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Render
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    void FXParticleManager3D<T>::Render(RenderManager3D& renderManager)
    {
        Camera3D& camera = *renderManager.activeCamera;

        shader->Activate();
        
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GL_GET_ERROR();
        
        // Update uniform value projection
        Matrix44 viewproj = camera.Projection() * camera.View();
        paramWVP.SetValue(viewproj);
        paramZoom.SetValue(this->pixelScaling);
        paramTexture.SetValue(*texture);
        
        // Update attribute position values
        // !!! Also the size hides in the W !!!
        attribPosition.SetAttributePointer(4,
                                            GL_FLOAT,
                                            GL_FALSE,
                                            sizeof(T),
                                            &this->particles[0].Position);
        
        // Update attribute color values
        attribColor.SetAttributePointer(4,
                                        GL_UNSIGNED_BYTE,
                                        GL_TRUE,
                                        sizeof(T),
                                        &this->particles[0].CurrentColor);
        
        glDrawArrays(GL_POINTS, 0, this->maxParticles);
        GL_GET_ERROR();
    }
    
}


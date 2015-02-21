////////////////////////////////////////////////////////////////////////////////
//  Renderer3D.h
//
//  Created by Bojan Endrovski on 09/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#pragma once

// Framework includes
#include "gl.h"

// Local
#include "ModelMesh3D.h"
#include "Camera3D.h"
#include "Texture.h"
#include "Shader.h"
#include "Transformable3D.h"
#include "Effect.h"
#include "RenderLayarable3D.h"

namespace Furiosity
{
    // Fwd decl
    // class Renderer3D;
    
    class RenderManager3D;
    
    enum class RenderQueue
    {
        Opaque,
        Transparent
    };
    
    ///
    /// Very simple interface for getting render calls
    ///
    class Renderable3D : public RenderLayarable3D
    {
        // Renderer3D& renderer;
        
        //float       layer       = 0.0f;
        
        RenderQueue renderQueue = RenderQueue::Opaque;
        
    public:
        Renderable3D(/*Renderer3D&    renderer,
                     float          layer = 0.0f */);
        
        virtual ~Renderable3D();
        
        // TODO: The parameter is not need	ed here
        // virtual void Render(Renderer3D& renderer) = 0;
        
        virtual void Render(RenderManager3D& renderManager) = 0; // {}
        
        // float RenderLayer() const { return layer; }

        // void SetRenderLayer(float l) { layer = l; }
    };
    
    /*
    ///
    /// Renderer3D
    ///
    class Renderer3D
    {
    protected:
        
        // Init effect first
        Effect  effect;
        
        ShaderParameter& worldViewProjParam;
        ShaderParameter& normalMatrixParam;
        ShaderParameter& lightDirectionParam;
        ShaderParameter& ambientParam;
        ShaderParameter& textureParam;
        
        ShaderAttribute& attribPosition;
        ShaderAttribute& attribNormal;
        ShaderAttribute& attribTexture;
        
        
        /// A reference to a 3D Camera
        const Camera3D&   camera;
        
        vector<Renderable3D*>  renderQueue;
        
        vector<GLint>           uniforms;
        
    public:
        
        Vector3         lightDir;
        
        /// Ctor TODO: Doc
        Renderer3D(const Camera3D&   camera,
                   const std::string& vertShaderFile = "/SharedResources/Shaders/Basic3D.vsh",
                   const std::string& fragShaderFile = "/SharedResources/Shaders/Basic3D.fsh");
        
        /// Dtor
        virtual ~Renderer3D();
        
        /// Renders all the objects in the render queue
        virtual void RenderQueue();
        
        /// Render a model TODO: Doc
        virtual void Render(const Matrix44& transform,
                            ModelMesh3D&    model,
                            Texture&        texture,
                            const Color&    ambient = Color::Black);
        
        /// Add a renderable object to the render queue. The renderable object will get a
        /// Render callback.
        //
        /// @param renderable An object to render
        void AddToRenderer(Renderable3D* renderable);
        
        /// Removes a renderable object from the render queue. The renderable object will no
        /// longer get Render callbacks.
        ///
        /// @param renderable An object not to be rendered anymore
        void RemoveFromRenderer(Renderable3D* renderable);
        
        /// Get the camera that this renderer is using
        const Camera3D& Camera() const { return camera; }        
    };
        
    ///
    /// MeshRenderable3D
    ///
    class MeshRenderable3D : public Renderable3D
    {
    protected:
        ModelMesh3D*                mesh;
        
        Texture*                    texture;
        
        const Entity3D&             transformableRef;
        
        Color                       ambient;
        
    public:
        MeshRenderable3D(Renderer3D&                renderer,
                         const Entity3D&            transformable,
                         const std::string&         meshFile,
                         const std::string&         textureFile,
                         const Color&               ambient = Color::Black);
        
        virtual ~MeshRenderable3D();
        
        // virtual void Render(Renderer3D& renderer) override;
        
        virtual void Render(RenderManager3D& renderManager);
        
        const Color& Ambient() const    { return ambient; }
        
        void SetAmbient(const Color& a) { ambient = a; }
        
        void SetTexture(Texture* t);
    };
    */
}

////////////////////////////////////////////////////////////////////////////////
//  SpriteRender.h
//  Furiosity
//
//  Created by Bojan Endrovski on 09/04/2012.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef SPRITERENDER_H
#define SPRITERENDER_H

// Framework includes
#include "gl.h"
#include <string>

// Local
#include "Camera2D.h"
#include "Texture.h"
#include "Color.h"
#include "VertexFormats.h"
#include "ResourceManager.h"
#include "Matrix33.h"
#include "SpriteAnimator.h"
#include "Defines.h"
#include "Shader.h"

using namespace std;

namespace Furiosity
{
    // Forward decl
    class Entity2D;
    class SpriteRender;
    
    ///
    /// A simple interface to do some rendering with
    ///
    class Renderable
    {
    protected:
        
        /// Rendering layer, all items will be sorted based on this value
        float renderLayer;
        
    public:
        /// Ctor with default render layer of zero
        Renderable() { renderLayer = 0; }
        
        /// Ctor with xml
        Renderable(const XMLElement& settings);
        
        /// Place your rendering call here
        virtual void Render(SpriteRender* render) = 0;
        
        // Get the render layer for this item
        float RenderLayer() const { return renderLayer; }
    };
    
    
    ////
    /// A render class that can be used as somewhat of a manager, as it holds a list
    /// of renderable objects
    ///
    class SpriteRender
    {
    private:
        GLint attribTexture;
        GLint attribPosition;
        Shader* shader;
        
    protected:
        
        // Uniform index
        enum 
        {
            UNIFORM_PROJECTION,     // Camera matrix
            UNIFORM_WORLD,          // World matrix
            UNIFORM_TEXSAMPLER,     // Texture
            UNIFORM_TINT,           // Tint
            NUM_UNIFORMS            // Counter
        };
        
    protected:
        // Shader uniforms
        GLint*              uniforms;
                
        // A pointer to the camera
        const Camera2D*     camera;
        
        //
        vector<Renderable*> renderQueue;
        
    protected:                 
        // Link uniforms and attributes. Assumes a valid Shader* is available.
		bool LinkShaders();
        
        // Activate shader, used mostly internally
        void ActivateShader(const Texture* texture,
                            const Color& tint,
                            const Matrix33& transform = Matrix33::Identity);
        
        // Does nothing here
        virtual void SetUniforms() {}
        
        /*
        // Does nothing here
        virtual void GetUniforms() {}
        */
        
    public:
        SpriteRender(const Camera2D* camera,
                     const std::string& vertShaderFile = "/SharedResources/Shaders/Sprite.vsh",
                     const std::string& fragShaderFile = "/SharedResources/Shaders/Sprite.fsh");
        
        virtual ~SpriteRender();
        
        /// Add a renderable object to the render queue. The renderable object will get a
        /// Render callback.
        //
        /// @param renderable An object to render
        virtual void AddToRenderer(Renderable* renderable);
        
        /// Removes a renderable object from the render queue. The renderable object will no
        /// longer get Render callbacks.
        ///
        /// @param renderable An object not to be rendered anymore
        virtual void RemoveFromRenderer(Renderable* renderable);

        
        virtual void DrawQuad(const Matrix33& transform,
                              float width,
                              float height,
                              const Texture* texture,
                              Vector2 offset            = Vector2(0.0f, 0.0f),
                              Color tint                = Color::White,
                              Vector2 uvFrom            = Vector2(0.0f, 0.0f),
                              Vector2 uvTo              = Vector2(1.0f, 1.0f));
        
        virtual void DrawPrimitive(uint primitive,
                                   VertexPosition2DTexture* vertices,
                                   ushort vCount,
                                   ushort* indices,
                                   ushort iCount,
                                   const Texture* texture,
                                   Color tint = Color::White,
                                   const Matrix33& transform = Matrix33::Identity);
        
        virtual void DrawRoundedQuad(float radius, int numVerticesPerCorner,
                                     const Matrix33& transform,
                                     float width,
                                     float height,
                                     const Texture* texture,
                                     Vector2 offset            = Vector2(0.0f, 0.0f),
                                     Color tint                = Color::White,
                                     Vector2 uvFrom            = Vector2(0.0f, 0.0f),
                                     Vector2 uvTo              = Vector2(1.0f, 1.0f));
        
        virtual void DrawEllipse(int numVertices,
                                 const Matrix33& transform,
                                 float width,
                                 float height,
                                 const Texture* texture,
                                 Vector2 offset            = Vector2(0.0f, 0.0f),
                                 Color tint                = Color::White,
                                 Vector2 uvFrom            = Vector2(0.0f, 0.0f),
                                 Vector2 uvTo              = Vector2(1.0f, 1.0f));
        
        virtual void DrawLine(const Vector2& from,
                              const Vector2& to,
                              const Matrix33& transform,
                              float thickness,
                              const Texture* texture,
                              Color tint                    = Color::White);
        
        virtual void DrawArc(const Matrix33& transform,
                             float innerRadius,
                             float outerRadius,
                             float from,
                             float to,
                             int   slices,
                             const Texture* texture,
                             Color tint                = Color::White);
        

        static void ShaderReloaded(void* sender, Shader& shader) {
            static_cast<SpriteRender*>(sender)->LinkShaders();
        }
        
        /// Renders all the objects in the render queue
        virtual void RenderQueue();
    };
}


#endif

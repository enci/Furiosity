////////////////////////////////////////////////////////////////////////////////
//  CanvasRenderer.h
//
//  Created by Gerard Meier on 7/19/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gl.h"
#include "Camera2D.h"
#include "Canvas.h"
#include "Shader.h"
#include "ResourceManager.h"

namespace Furiosity
{
    ///
    /// No in use, not stable - remove untill done
    ///
    class CanvasRenderer
    {
        /// The camera. Used to retrieve the project matrix from.
        const Camera2D* camera;
        
        /// The shader, to shade with.
        Shader* shader;
        
        /// OpenGL handle to the camera matrix uniform:
        GLuint uniformProjection;
        
        /// OpenGL handle to local transform matrix uniform:
        GLuint uniformWorld;
        
        /// OpenGL handle pointing to the position
        GLuint attribPosition;
        
        /// Note to future me: perhaps calculate the color in the shader
        /// and specify the gradient as a uniform?
        GLuint attribColor;
        
        GLuint glPrimitive;
        
        /// Link the shader to this renderer. Retrieves handles to
        /// all uniforms and attributes.
        void LinkShaders();
        
    public:
        /// Create a new canvas renderer. The renderer doesn't have a unique
        /// or identifiable state, it's pointless to have multiple instances.
        CanvasRenderer(const Camera2D* camera,
            const std::string& vertexShader = "/SharedResources/Shaders/Svg.vsh",
            const std::string& fragmentShader = "/SharedResources/Shaders/Svg.fsh",
            GLuint glPrimitive = GL_TRIANGLES
        );
        
        /// Destroys and releases the shader.
        virtual ~CanvasRenderer();
        
        /// Renders...
        void Render(const Matrix33& transform, Canvas* resource);
        
        
        /// When a shader is recompiled, the attributes and unforms must
        /// be re-linked. This method does just that. Mostly used for
        /// android asset reloading.
        static void ShaderReloaded(void* sender, Shader& shader) {
            static_cast<CanvasRenderer*>(sender)->LinkShaders();
        }
    };
}
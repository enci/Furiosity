////////////////////////////////////////////////////////////////////////////////
//  ModelMesh3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/5/13.
//  Copyright (c) 2013 Game Oven. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// STL
#include <string>

// Framework
#include "gl.h"

// Local
#include "Frmath.h"
#include "Color.h"
#include "Resource.h"
#include "VertexFormats.h"

namespace Furiosity
{
    class ResourceManager;
    class Effect;
    class ShaderAttribute;
    
    // Wrraper for an obj mesh
    class ModelMesh3D : public Resource
    {
        /// Resource's lifetime is handeled by the resource manager
        friend class ResourceManager;
        
        typedef VertexPositionNormalTexture Vertex;
        
    protected:        

        /// Mesh vertices
        std::vector<Vertex>    vertices;
        
        /// Mesh indices
        std::vector<GLushort>  indices;
        
        /// Vertex buffers
        GLuint vbo[2];

        
        /// Imports an OBJ model mesh
        ModelMesh3D(const std::string& _filename);
        
        /// Protected dtor
        ~ModelMesh3D();
        
    public:
        
        /// Determine if this mesh is usable for OpenGL.
        virtual bool IsValid() override;

        /// Reload the OpenGL resources.
        virtual void Reload(bool cached = false) override;

        /// Dispose any OpenGL resources.
        virtual void Invalidate() override;
        
        /// Mesh vertices
        const Vertex*   Vertices() const    { return & vertices[0]; }
        
        /// Number of mesh vertices
        const int       VertexCount() const { return (int)vertices.size(); }

        /// Mesh indices
        const GLushort* Indices() const     { return & indices[0]; }
        
        /// Number of elements in the indices array.
        const int       IndexCount() const  { return (int)indices.size(); }
        
        /// Getter for VBO's
        const GLuint* VertexBuffers() const { return vbo; };
        
        /// Determine if this mesh uses VBO's
        bool HasVertexBuffers() const { return vbo[0] != 0; };
        
        int GetVertexBuffer() const;
        
        int GetIndexBuffer() const;
        
        /// Draws the mesh using the shader settings already set. Basically it
        /// just sends the mesh data to the GPU, so it only works in context
        void Render(Effect& shader);
        
        /// Draws the mesh using the shader settings already set. Basically it
        /// just sends the mesh data to the GPU, so it only works in context
        void Render(ShaderAttribute&    attribPosition,
                    ShaderAttribute&    attribNormal,
                    ShaderAttribute&    attribTexture);
    };
}

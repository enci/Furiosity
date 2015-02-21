////////////////////////////////////////////////////////////////////////////////
//  ModelScene3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 1/5/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Resource.h"
#include "Frmath.h"
#include "VertexFormats.h"
#include "Shader.h"
#include "Effect.h"

// Assimp
#include "Importer.hpp"         // C++ importer interface
#include "scene.h"              // Output data structure
#include "postprocess.h"        // Post processing flags


using std::string;

namespace Furiosity
{
    // Forward declarations
    class ModelScene3D;
    class Entity3D;
    class World3D;
    
    ///
    /// A renderable mesh, with the vertex infromation stored in a VBO or a
    /// vector.
    ///
    class Mesh3D
    {
        friend class ModelScene3D;
        
        typedef VertexPositionNormalTexture Vertex;
        
    protected:
        
        /// Mesh vertices
        std::vector<Vertex>    vertices;
        
        /// Mesh indices
        std::vector<GLushort>  indices;
        
        /// Vertex buffers
        GLuint vbo[2];
        
    public:
        
        /// Imports a model mesh from an Assimp mesh
        Mesh3D(aiMesh* aiMesh);
        
        /// Protected dtor
        ~Mesh3D()
        {
            // TODO: Release resources
        }
        
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
        
        /// Draws the mesh using the shader settings already set. Basically it
        /// just sends the mesh data to the GPU, so it only works in context
        void Render(Effect& shader);
        
        /// Draws the mesh using the shader settings already set. Basically it
        /// just sends the mesh data to the GPU, so it only works in context
        void Render(ShaderAttribute&    attribPosition,
                    ShaderAttribute&    attribNormal,
                    ShaderAttribute&    attribTexture);
    };
    
    ///
    ///
    ///
    class ModelScene3DResource : public Resource
    {
        /// Resource's lifetime is handled by the resource manager
        friend class ResourceManager;
        
    public:
        
        /// Determine if this meshes are usable for OpenGL.
        virtual bool IsValid() override
        {
            // TODO: Implement this
            return true;
        }
        
        /// Reload the OpenGL resources.
        virtual void Reload(bool cached = false) override;
        
        /// Dispose any OpenGL resources.
        virtual void Invalidate() override
        {
            // TODO: Implement this
        }

        const aiLight*      GetLight(const aiString& name);

        //        const aiMaterial*   GetMaterial(aiString& name);

        const aiCamera*     GetCamera(const aiString& name);
    
        //public:

    protected:
        
        /// Imports a 3d files with mesh, lights and such
        ModelScene3DResource(const string& filename);
                     
         /// Protected dtor
        ~ModelScene3DResource()
        {
            // TODO: Implement this
             importer.FreeScene();
             scene = nullptr;
        }
        
    public:
        
        /// Assimp importer is used for managment of the scene
        Assimp::Importer    importer;
        
        /// The Assimp DOM
        const aiScene*      scene;
        
        /// A collection of processed meshes
        vector<Mesh3D*>     meshes;
    };


    ///
    ///
    ///
    class ModelScene3D : public ResourceHandle<ModelScene3DResource>
    {
    public:
        ModelScene3D(const string& file);
    };
    

    ///
    ///
    ///
    class SceneProcessor
    {
        World3D& world;
        
    public:
        SceneProcessor(World3D& world) : world(world) {}
        
        virtual void Process(Entity3D* parent, ModelScene3D & scene);
        
        virtual void Process(Entity3D* parent, ModelScene3D & scene, const aiNode* node);
        
        virtual Entity3D* AddLight(Entity3D* parent, ModelScene3D & scene, const aiNode* node);
        
        virtual Entity3D* AddModelMesh(Entity3D* parent, ModelScene3D & scene, const aiNode* node);
    };
}


////////////////////////////////////////////////////////////////////////////////
//  ModelScene3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 1/5/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#if USE_ASSIMP

#include "ModelScene3D.h"
#include "Entity3D.h"
#include "StaticMeshEntity3D.h"
#include "World3D.h"
#include "AssimpTools.h"

#define DEBUG_MODEL_LOADING

using namespace Furiosity;


Mesh3D::Mesh3D(aiMesh* mesh)
{
    // Test
    if(!mesh->HasPositions()    ||
       !mesh->HasNormals()      ||
       !mesh->HasTextureCoords(0))
    {
        LOG("Malformed mesh!");
        return;
    }
    
    
    // Vertex data
    vertices.resize(mesh->mNumVertices);
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        auto& v = mesh->mVertices[i];
        auto& n = mesh->mNormals[i];
        auto& t = mesh->mTextureCoords[0][i];
        //
        vertices[i].Position    = Vector3(v.x, v.y, v.z);
        vertices[i].Normal      = Vector3(n.x, n.y, n.z);
        vertices[i].Texture     = Vector2(t.x, t.y);
    }
    
    // Index data
    indices.resize(mesh->mNumFaces * 3);
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];
        
        if(face.mNumIndices != 3)
        {
            LOG("Malformed mesh!");
            return;
        }
        
        int idx = i * 3;
        indices[idx++] = face.mIndices[0];
        indices[idx++] = face.mIndices[1];
        indices[idx++] = face.mIndices[2];
    }
    
    // TODO: Bring this back
    // size  = sizeof(vertices[0]) * (int)vertices.size();
    // size += sizeof(indices[0]) * (int)indices.size();
    
    // Allocate two buffers
    glGenBuffers(2, vbo);
    GL_GET_ERROR();
    
    // Array buffer contains the attribute data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    GL_GET_ERROR();
    
    // Copy into VBO:
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    GL_GET_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind buffer
    GL_GET_ERROR();
    
    // Element array buffer contains the indices.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    GL_GET_ERROR();
    
    // Copy into VBO:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
    GL_GET_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind buffer
    GL_GET_ERROR();

}



void Mesh3D::Render(Effect &shader)
{
    ShaderAttribute& attribPosition = *shader->GetAttribute("a_position");
    ShaderAttribute& attribNormal   = *shader->GetAttribute("a_normal");
    ShaderAttribute& attribTexture  = *shader->GetAttribute("a_texture");
    //
    Render(attribPosition, attribNormal, attribTexture);
}


void Mesh3D::Render(ShaderAttribute &attribPosition,
                         ShaderAttribute &attribNormal,
                         ShaderAttribute &attribTexture)
{
    const bool useVbo = HasVertexBuffers();
    
    // Default to VBO values, the pointer addresses are interpreted as byte-offsets.
    const void* firstPosition = (void*) offsetof(VertexPositionNormalTexture, Position);
    const void* firstNormal   = (void*) offsetof(VertexPositionNormalTexture, Normal);
    const void* firstTexture  = (void*) offsetof(VertexPositionNormalTexture, Texture);
    const void* firstIndex    = 0;
    
    // We are not using VBO, provide actual pointers to data instead of byte-offsets.
    if(!useVbo)
    {
        firstPosition = &Vertices()[0].Position;
        firstNormal   = &Vertices()[0].Normal;
        firstTexture  = &Vertices()[0].Texture;
        firstIndex    = Indices();
    }
    else
    {
        const GLuint* vbo = VertexBuffers();
        
#ifdef DEBUG
        // It's typical during Android development that buffers break.
        if(glIsBuffer(vbo[0]) != GL_TRUE)
            //ERROR("The vertex buffer is not a valid buffer (VBO). File: %s.", Path().c_str());
            ERROR("The vertex buffer is not a valid buffer (VBO).");
        
        if(glIsBuffer(vbo[1]) != GL_TRUE)
            ERROR("The index buffer is not a valid buffer (VBO).");
#endif
        
        // Bind the buffers to the global state
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    }
    
    
    GLsizei size = sizeof(VertexPositionNormalTexture);
    attribPosition.SetAttributePointer( 3, GL_FLOAT, GL_FALSE, size, firstPosition);
    attribNormal.SetAttributePointer(   3, GL_FLOAT, GL_FALSE, size, firstNormal);
    attribTexture.SetAttributePointer(  2, GL_FLOAT, GL_FALSE, size, firstTexture);
    
    // Validate just before drawing. If the shader has errors, then this call will find them.
#if defined(DEBUG)
    //    if (!ValidateProgram(effect->GetProgram()))
    //    {
    //        LOG("Failed to validate program: %d", effect->GetProgram());
    //        return;
    //    }
#endif
    
    glDrawElements(GL_TRIANGLES, IndexCount(), GL_UNSIGNED_SHORT, firstIndex);
    GL_GET_ERROR();
    
    if(useVbo)
    {
        // Unbind buffers from global state.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        GL_GET_ERROR();
    }
}


ModelScene3DResource::ModelScene3DResource(const string& filename) : Resource(RESOURCE_TYPE_MESH)
{
    resourcePath = filename;
    Reload();
}

void ModelScene3DResource::Reload(bool cached)
{
    // Get latest file
    string file = gResourceManager.GetLatestPath(resourcePath);
    scene = importer.ReadFile(file,
                              aiProcess_Triangulate            |
                              aiProcess_JoinIdenticalVertices  |
                              aiProcess_SortByPType);
#ifdef DEBUG_MODEL_LOADING
    LOG("Loading scene: %s", file.c_str());
#endif
    
    
    if(scene == nullptr)
    {
        LOG("Error loading scene: %s", file.c_str());
        return;
    }
    
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* aimesh = scene->mMeshes[i];            
        Mesh3D* mesh = new Mesh3D(aimesh);
        meshes.push_back(mesh);
    }
}

const aiLight*ModelScene3DResource::GetLight(const aiString& name)
{
    for(int i = 0; i < scene->mNumLights; i++)
    {
        aiLight* light = scene->mLights[i];
        if(light->mName == name)
            return light;
    }
    
    return nullptr;
}

/*
const aiMaterial* ModelScene3D::GetMaterial(aiString& name)
{
    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material = scene->mMaterials[i];
        if(material->  == name)
            return material;
    }
    
    return nullptr;
}
*/

const aiCamera* ModelScene3DResource::GetCamera(const aiString& name)
{
    for(int i = 0; i < scene->mNumCameras; i++)
    {
        aiCamera* camera = scene->mCameras[i];
        if(camera->mName == name)
            return camera;
    }
    
    return nullptr;
}


ModelScene3D::ModelScene3D(const string& file) :
    ResourceHandle(gResourceManager.LoadModelScene3D(file))
{
}

////////////////////////////////////////////////////////////////////////////////
//
//                              SceneProcessor
//
////////////////////////////////////////////////////////////////////////////////

SceneProcessor::SceneProcessor(World3D& world) : world(world) {}


void SceneProcessor::Process(Entity3D* parent, ModelScene3D& scene)
{
    aiNode* rootNode = scene->scene->mRootNode;
    for (int i = 0; i < rootNode->mNumChildren; i++)
        Process(parent, scene, rootNode->mChildren[i]);
}

void SceneProcessor::Process(Entity3D* parent, ModelScene3D & scene, const aiNode *node)
{
    Entity3D* entity = nullptr;
    
    if(node->mNumMeshes > 0)
    {
        // Create a renderable mesh object
        entity = AddModelMesh(parent, scene, node);
    }
    else if(scene->GetLight(node->mName))
    {
        entity = AddLight(parent, scene, node);
    }
    else if(node->mName.C_Str())
    {
        
    }
    
    // Process all the childern too
    if(entity)
        for (int i = 0; i < node->mNumChildren; i++)
            Process(entity, scene, node->mChildren[i]);
}


Entity3D* SceneProcessor::AddLight(Entity3D* entity, ModelScene3D& scene, const aiNode* node)
{
    Light3D* light              = new Light3D(&world, entity);
    const aiLight* assimpLight  = scene->GetLight(node->mName);
    light->diffuse              = AssimpTools::ConvertColor(assimpLight->mColorDiffuse);
    light->SetTransformation(AssimpTools::ConvertMatrix(node->mTransformation));
    world.AddEntity(light);
    return light;
}

Entity3D* SceneProcessor::AddModelMesh(Entity3D* entity, ModelScene3D& scene, const aiNode* node)
{
    StaticMeshEntity3D* mesh = new StaticMeshEntity3D(&world,
                                                      entity,
                                                      scene,
                                                      node);
    world.AddEntity(mesh);
    return mesh;
}

#endif














// end
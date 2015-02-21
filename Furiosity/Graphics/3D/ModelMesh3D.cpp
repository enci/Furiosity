////////////////////////////////////////////////////////////////////////////////
//  ModelMesh3D.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/5/13.
//  Copyright (c) 2013 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ModelMesh3D.h"

// Frameworks
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>

// local
#include "FileIO.h"
#include "ResourceManager.h"
#include "Effect.h"
#include "Shader.h"
//#include "Defines.h"

using namespace std;
using namespace Furiosity;

typedef uint64_t        ulong;

#ifdef ANDROID
	typedef unsigned short	ushort;
#endif

ulong trihash(short k, short l, short m)
{
    // Create hash
    ulong h = k;
    h <<= 20;
    h += l;
    h <<= 20;
    h += m;
    
    // All good
    return h;
}

ModelMesh3D::ModelMesh3D(const std::string& _filename) :
    Resource(RESOURCE_TYPE_MESH),
    vertices(0),
    indices(0),
    vbo{0}
{
    resourcePath = _filename;
    Reload();
}


ModelMesh3D::~ModelMesh3D()
{
    if(HasVertexBuffers()) {
        glDeleteBuffers(2, vbo);
        GL_GET_ERROR();
    }
}


void ModelMesh3D::Reload(bool cached)
{
    // Get latest file
    string filename = gResourceManager.GetLatestPath(resourcePath);
    
    // This is most compatible with Android. Soon this is to be replaced with
    // fancy COLLADA files.
    const std::string dataString = Furiosity::ReadFile(filename);
    
    if(dataString.empty())
    {
        LOG("Unable to open obj file or it is empty: %s\n", filename.c_str());
        return;
    }
    
    std::stringstream is(dataString);
    
    
    char input[512];
    char line[512];
    
    float x, y, z, u, v;
    ushort k,l,m;
    
    vector<Vector3>                         offsetVec;      // Position offsets
    vector<Vector2>                         textureVec;     // Texture coordinates
    vector<Vector3>                         normalVec;      // Normals
    vector<ushort>                          indicesVec;     // Indices
    //
    vector<VertexPositionNormalTexture>     vertexVec;      // Generated vertices
    //
    ushort idx = 0;
    map<ulong, uint> indexForVertex;
    
    
    // Comment, skip line
    // is.getline(line, 511, '\n');

    while(is >> input)
    {
        if(input[0] ==  '#')
        {
            // Comment, skip line
            is.getline(line, 255, '\n');
        }
        else if( strcmp(input, "v") == 0 )
        {
            // Vertex offset
            is >> x >> y >> z;
            offsetVec.push_back(Vector3(x, y, z));
        }
        else if( strcmp(input, "vt") == 0)
        {
            // Texture coord
            is >> u >> v;
            textureVec.push_back(Vector2(u, 1.0f - v));
        }
        else if(strcmp(input, "vn") == 0 )
        {
            // Vertex normal
            is >> x >> y >> z;
            normalVec.push_back(Vector3(x, y, z));
        }
        else if( strcmp(input, "f") == 0 )
        {
            // Face - only triangles will work!!!
            
            char slash;
            // Three vertices on a triangle
            for (int i = 0;  i < 3; ++i)
            {
                // Read indices
                is >> k >> slash >> l >> slash >> m;
                k -= 1;
                l -= 1;
                m -= 1;
                
                // Generate unique hash
                ulong hash = trihash(k, l, m);
                
                // Check if this a new vertex
                map<ulong, uint>::iterator itr;
                itr = indexForVertex.find(hash);
                // indexForVertex.end();//
                if (false && itr != indexForVertex.end())
                {
                    // Vertex in set
                    indicesVec.push_back(itr->second);
                }
                else
                {
                    // New vertex
                    VertexPositionNormalTexture vertex;
                    vertex.Position = offsetVec[k];
                    vertex.Normal = normalVec[m];
                    vertex.Texture = textureVec[l];
                    
                    // Add it to the vector
                    vertexVec.push_back(vertex);
                    // Book-keep index by vertex hash
                    indexForVertex[hash] = idx;
                    // Add index to indices vector
                    indicesVec.push_back(idx);
                    idx++;
                }
            }
        }
    }
    
    vertices = std::move(vertexVec);
    indices  = std::move(indicesVec);

    size  = sizeof(vertices[0]) * (int)vertices.size();
    size += sizeof(indices[0]) * (int)indices.size();
    
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

/// Determine if this mesh is usable for OpenGL.
bool ModelMesh3D::IsValid() {

	// The buffer handle is not zero, this indicates that we
	// "once upon a time" used buffers. Let's test of those
	// buffers are still valid.
	if(HasVertexBuffers()) {
		return glIsBuffer(vbo[0]) && glIsBuffer(vbo[1]);
	}

	// We never used buffers. Rely on an array of vertices.
	return ! vertices.empty();
}

/// Dispose any OpenGL resources.
void ModelMesh3D::Invalidate() {
	if(HasVertexBuffers()) {
		glDeleteBuffers(2, vbo);

		vbo[0] = vbo[1] = 0;
    	GL_GET_ERROR();
	}
}

void ModelMesh3D::Render(Effect &shader)
{
    ShaderAttribute& attribPosition = *shader->GetAttribute("a_position");
    ShaderAttribute& attribNormal   = *shader->GetAttribute("a_normal");
    ShaderAttribute& attribTexture  = *shader->GetAttribute("a_texture");
    //
    Render(attribPosition, attribNormal, attribTexture);
}


void ModelMesh3D::Render(ShaderAttribute &attribPosition,
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
        if(glIsBuffer(vbo[0]) != GL_TRUE) {
            ERROR("The vertex buffer is not a valid buffer (VBO). File: %s.", Path().c_str());
        }
        
        if(glIsBuffer(vbo[1]) != GL_TRUE) {
            ERROR("The index buffer is not a valid buffer (VBO). File: %s.", Path().c_str());
        }
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

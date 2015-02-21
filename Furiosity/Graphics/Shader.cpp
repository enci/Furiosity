////////////////////////////////////////////////////////////////////////////////
//  Renderer.cpp
//
//  Created by Bojan Endrovski on 09/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "Shader.h"
#include "FileIO.h"
#include "Texture.h"
#include "Effect.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                              ShaderParameter
//
////////////////////////////////////////////////////////////////////////////////

void ShaderParameter::SetValue(float val)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_FLOAT);
    glUniform1f(location, val);
    GL_GET_ERROR();
}


void ShaderParameter::SetValue(int val)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_INT);
    glUniform1i(location, val);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(bool val)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_BOOL);
    glUniform1i(location, val);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(const Vector2& vec)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_FLOAT_VEC2);
    glUniform2fv(location, 1, vec.f);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(const Vector3& vec)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_FLOAT_VEC3);
    glUniform3fv(location, 1, vec.f);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(const Vector4& vec)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_FLOAT_VEC4);
    glUniform4fv(location, 1, vec.f);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(const Color& color)
{
    Vector4 c(color.r, color.g, color.b, color.a);
    c *= 1.0f / 255.0f;
    if(type == GL_FLOAT_VEC4)
        glUniform4fv(location, 1, c.f);
    else if (type == GL_FLOAT_VEC3)
        glUniform3fv(location, 1, c.f);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(const Matrix33& mtx, bool transpose)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_FLOAT_MAT3);
    glUniformMatrix3fv(location, 1, transpose, mtx.f);
    GL_GET_ERROR();
}

void ShaderParameter::SetValue(const Matrix44& mtx, bool transpose)
{
    if(!IsValid())
        return;

    ASSERT(type == GL_FLOAT_MAT4);
    glUniformMatrix4fv(location, 1, transpose, mtx.f);
    GL_GET_ERROR();
}


void ShaderParameter::SetValue(const Texture &texture)
{
    if(!IsValid())
        return;
    
    ASSERT(type == GL_SAMPLER_2D);
    // Use texture with index sampler. GL_TEXTURE1 = GL_TEXTURE10+1 is always true
    glActiveTexture(GL_TEXTURE0 + sampler);
    GL_GET_ERROR();
    // Work with this texture
    glBindTexture(GL_TEXTURE_2D, texture.name);
    GL_GET_ERROR();
    // Set the sampler
    glUniform1i(location, sampler);
    GL_GET_ERROR();    
}

void ShaderParameter::SetValue(const CubeMap& cubeMap)
{
    if(!IsValid())
        return;
    
    ASSERT(type == GL_SAMPLER_CUBE);
    // Use texture with index sampler. GL_TEXTURE1 = GL_TEXTURE10+1 is always true
    glActiveTexture(GL_TEXTURE0 + sampler);
    GL_GET_ERROR();
    
    // Work with this texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.cubeMapID);
    GL_GET_ERROR();
    // Set the sampler
    glUniform1i(location, sampler);
    GL_GET_ERROR();
}

////////////////////////////////////////////////////////////////////////////////
//
//                          ShaderAttribute
//
////////////////////////////////////////////////////////////////////////////////

void ShaderAttribute::SetAttributePointer(GLint size,
                                          GLenum type,
                                          GLboolean normalized,
                                          GLsizei stride,
                                          const GLvoid *pointer)
{
    if(!IsValid())
        return;
    
    glVertexAttribPointer(
        location,           // attribute
        size,               // number of elements per vertex element
        type,               // the type of each element
        normalized,         // take our values as-is or normalize
        stride,             // no extra data between each position
        pointer             // offset of first element
    );
    GL_GET_ERROR();
    
    glEnableVertexAttribArray(location);
    GL_GET_ERROR();
}


////////////////////////////////////////////////////////////////////////////////
//
//                                  Shader
//
////////////////////////////////////////////////////////////////////////////////

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) :
    Resource(RESOURCE_TYPE_SHADER),
    vertexPath(vertexPath),
    fragmentPath(fragmentPath),
    program(0)
{
    bool success = Load(false);
    if(!success)
    {
        LOG("Shader::Shader(%s, %s) - Unable to load shaders.", vertexPath.c_str(), fragmentPath.c_str());
        
        // Try again with the error shader
        success = Load(false, true);
        if(!success)
        {
            LOG("Unable to load error fallback shader!");
            return;
        }
    }

    LoadParamters();
}

Shader::~Shader()
{
    if(program > 0)
    {
        glDeleteProgram(program);
        program = 0;
        GL_GET_ERROR();
    }
}

bool Shader::Load(bool cached, bool errorShader)
{
    GLuint vertShader = 0, fragShader = 0;

    program = glCreateProgram();

    // Temp result
    GLboolean res;
    
    
    string vertFullPath = gResourceManager.GetPath(vertexPath);
    string fragFullPath = gResourceManager.GetPath(fragmentPath);
    
    if(errorShader)
    {
        vertFullPath = gResourceManager.GetPath("/SharedResources/Shaders/Magenta3D.vsh");
        fragFullPath = gResourceManager.GetPath("/SharedResources/Shaders/Magenta3D.fsh");
    }
    else if(cached)
    {
        vertFullPath = gResourceManager.GetCachedPath(vertexPath);
        fragFullPath = gResourceManager.GetCachedPath(fragmentPath);
    }

    std::string vertShaderSource = ReadFile(vertFullPath);
    res = CompileShader(&vertShader, GL_VERTEX_SHADER, vertShaderSource.c_str());
    if (!res)
    {
        LOG("Shader::Load() Failed to compile vertex shader");
        return false;
    }

    // Create and compile fragment shader.
    std::string fragShaderSource = ReadFile(fragFullPath);
    
    res = CompileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderSource.c_str());
    if (!res)
    {
        LOG("Shader::Load() Failed to compile fragment shader");
        return false;
    }

    // Attach vertex shader to program
    glAttachShader(program, vertShader);

    // Attach fragment shader to program
    glAttachShader(program, fragShader);

    // Link program
    if (!LinkProgram(program))
    {
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }

        GL_GET_ERROR();

        // We crash here, else the logs will be flooded with repeated
        // error messages.
        //ERROR("Shader::Load() Failed to link program: %d", program);

        return false;
    }

    glDeleteShader(vertShader);
    GL_GET_ERROR();
    glDeleteShader(fragShader);
    GL_GET_ERROR();

    return true;
}

void Shader::Reload(bool cached)
{
    bool success = Load(cached);
    if(!success)
    {
        LOG("Shader::Shader(%s, %s) - Unable to load shaders.", vertexPath.c_str(), fragmentPath.c_str());
        
        // Try again with the error shader
        success = Load(cached, true);
        if(!success)
        {
            LOG("Unable to load error fallback shader!");
            return;
        }
    }
    
    // Call all subscribers
    Resource::Reload(cached);
}

void Shader::Invalidate()
{
	glDeleteProgram(program);
	program = 0;
	GL_GET_ERROR();
}

bool Shader::IsValid()
{
    return glIsProgram(program) == GL_TRUE;
}

GLuint Shader::GetProgram() const
{
    return program;
}

void Shader::LoadParamters()
{
    /// The shader should invalidate when reloading a new shader file
    /// a some information can be old
    for(auto& itr : parameters)
        itr.second->Invalidate();
    
    // Get the number of uniforms and resize the parameters collection accordingly
    GLint numActiveUniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    //parameters.resize(numActiveUniforms);

    // Get the maximum name length
    GLint maxUniformNameLength = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH ,&maxUniformNameLength);
    vector<GLchar> uniformNameData(maxUniformNameLength);

    // Go over all the uniforms
    int samplerCount = 0;
    // int samplerCubeCount = 0;
    for (int uniform = 0; uniform < numActiveUniforms; uniform++)
    {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveUniform(
                program,
                uniform,
                maxUniformNameLength,
                &actualLength,
                &arraySize,
                &type,
                &uniformNameData[0]);
        string name(&uniformNameData[0], actualLength);
        GLint location = glGetUniformLocation(program, name.c_str());
        
        auto itr = parameters.find(name);
        if(itr != parameters.end())
        {
            if(type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
                itr->second->Reset(this, name, type, location, samplerCount++);
            /*
            if(type == GL_SAMPLER_CUBE)
                itr->second->Reset(this, name, type, location, samplerCubeCount++);
            */
            else
                itr->second->Reset(this, name, type, location);
        }
        else
        {
            ShaderParameter* param = nullptr;
            if(type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
                param = new ShaderParameter(this, name, type, location, samplerCount++);
            else
                param = new ShaderParameter(this, name, type, location);
            parameters[name] = unique_ptr<ShaderParameter>(param);
        }
    }


    GLint numActiveAttribs = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
    
    
    GLint maxAttribNameLength = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
    vector<GLchar> attribNameData(maxAttribNameLength);
    
    
    for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
    {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei actualLength = 0;
        glGetActiveAttrib(program,
                          attrib,
                          attribNameData.size(),
                          &actualLength,
                          &arraySize,
                          &type,
                          &attribNameData[0]);
        std::string name((char*)&attribNameData[0]);
        
        auto itr = attributes.find(name);
        if(itr != attributes.end())
        {
            itr->second->Reset(this, name, type, attrib);
        }
        else
        {
            ShaderAttribute* attribute = new ShaderAttribute(this, name, type, attrib);
            attributes[name] = unique_ptr<ShaderAttribute>(attribute);
        }
    }
}

ShaderParameter* Shader::GetParameter(const string& name)
{
    // Try to find param
    auto itr = parameters.find(name);
    if(itr != parameters.end())
        return itr->second.get();

    // Create and return a non-valid param that is stored in collection
    // in case it becomes valid after a reload
    ShaderParameter* param = new ShaderParameter();
    parameters[name] = unique_ptr<ShaderParameter>(param);
    return param;
}


ShaderAttribute* Shader::GetAttribute(const string& name)
{
    // Try to find param
    auto itr = attributes.find(name);
    if(itr != attributes.end())
        return itr->second.get();
    
    // Create and return a non-valid param that is stored in collection
    // in case it becomes valid after a reload
    ShaderAttribute* attrib = new ShaderAttribute();
    attributes[name] = unique_ptr<ShaderAttribute>(attrib);
    return attrib;
}

void Shader::Activate()
{
    glUseProgram(GetProgram());
    GL_GET_ERROR();
}

bool Shader::Validate()
{
#if defined(DEBUG)
    if(!ValidateProgram(program))
    {
        LOG("Failed to validate program: %d", program);
        GL_GET_ERROR();
        return false;
    }
#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//                          LightShaderParameter
//
////////////////////////////////////////////////////////////////////////////////
LightShaderParameter::LightShaderParameter(Effect& shader, const string& name) :
    position(   *shader->GetParameter(name + ".position"    )),
    direction(  *shader->GetParameter(name + ".direction"   )),
    diffuse(    *shader->GetParameter(name + ".diffuse"     )),
    specular(   *shader->GetParameter(name + ".specular"    ))
{}

void LightShaderParameter::SetValue(const Light3D& light)
{
    Vector3 dir(0.0f, 0.0f, 1.0f);
    Matrix44 trns = light.Transform();
    dir = trns.TransformDirecton(dir);
    dir.Normalize();
    //
    direction.SetValue(dir);
    diffuse.SetValue(light.diffuse);
    
    // diffuse.SetValue(light.diffuse);
}




#pragma once

#include <map>
#include <functional>
#include <string>
#include <cassert>
#include <memory>

#include "gl.h"
#include "logging.h"
#include "Resource.h"
#include "ShaderTools.h"
#include "ResourceManager.h"
#include "Frmath.h"
#include "Color.h"
#include "CubeMap.h"
#include "Light3D.h"

namespace Furiosity
{
    class Shader;
    class Effect;

    ///
    /// ShaderParameter is a representation of an shader parameter.
    /// It has a type and it will complain if the type declared in the
    /// shader program is different.
    ///
    class ShaderParameter
    {
        friend class Shader;

    protected:

        /// The effect this parameter belongs to. Unused (beyond debugging)
        Shader* shader;

        /// Name as defined in the shader file.
        string  name;

        /// Type as defined in the shader file.
        GLenum  type;

        /// Location for the setting for this.
        GLint   location;
        
        /// Only valid for type sampler (GL_SAMPLER_2D)
        GLint   sampler;

        /// The shader creates a parameter.
        ShaderParameter(Shader* shader, string name, GLenum type, GLint location, GLint sampler = -1) :
                shader(shader),
                type(type),
                location(location),
                name(name),
                sampler(sampler)
        {}

        /// Constructor for an invalid ShaderParameter
        ShaderParameter() : shader(nullptr), name(""), type(0), location(-1), sampler(-1) {}


        /// The shader can reset the parameter after a reload
        void Reset(Shader* shader, string name, GLenum type, GLint location, GLint sampler = -1)
        {
            this->shader    = shader;
            this->type      = type;
            this->location  = location;
            this->name      = name;
            this->sampler   = sampler;
        }

        /// The shader should invalidate when reloading a new shader file
        /// a some information can be old
        void Invalidate()
        {
            shader      = nullptr;
            name        = "";
            type        = 0;
            location    = -1;
            sampler     = -1;
        }

    public:

        /// Check if this parameter object is valid
        bool IsValid() { return location != -1; }

        /// Gets the type of this parameter
        GLenum GetType() const { return type; }
        
        /// Location of this parameter
        GLint GetLocation() const { return location; }

        /// Set the value. Parameter should be float.
        void SetValue(float val);

        /// Set the value. Parameter should be int.
        void SetValue(int val);

        /// Set the value. Parameter should be bool.
        void SetValue(bool val);

        /// Set the value. Parameter should be Vector2.
        void SetValue(const Vector2& vec);

        /// Set the value. Parameter should be Vector3.
        void SetValue(const Vector3& vec);

        /// Set the value. Parameter should be Vector4.
        void SetValue(const Vector4& vec);

        /// Set the value. Parameter should be Color.
        void SetValue(const Color& color);

        /// Set the value. Parameter should be Matrix33.
        void SetValue(const Matrix33& mtx, bool transpose = false);

        /// Set the value. Parameter should be Matrix33.
        void SetValue(const Matrix44& mtx, bool transpose = false);
        
        /// Set the value. Parameter should be Texture.
        void SetValue(const Texture& texture);
        
        /// Set the value. The parameter should be CubeMap
        void SetValue(const CubeMap& cubeMap);
    };
    
    
    ///
    /// A vertex attribute
    ///
    class ShaderAttribute
    {
        friend class Shader;
        
    protected:
        
        /// The effect this parameter belongs to. Unused (beyond debugging)
        Shader* shader;
        
        /// Name as defined in the shader file.
        string  name;
        
        /// Type as defined in the shader file.
        GLenum  type;
        
        /// Location for the setting for this.
        GLint   location;
        
        /// The shader creates a parameter.
        ShaderAttribute(Shader* shader, string name, GLenum type, GLint location) :
            shader(shader),
            type(type),
            location(location),
            name(name)
        {}
        
        /// Constructor for an invalid ShaderParameter
        ShaderAttribute() : shader(nullptr), name(""), type(0), location(-1) {}
        
        
        /// The shader can reset the parameter after a reload
        void Reset(Shader* shader, string name, GLenum type, GLint location)
        {
            this->shader    = shader;
            this->type      = type;
            this->location  = location;
            this->name      = name;
        }
        
        /// The shader should invalidate when reloading a new shader file
        /// as some information can be old
        void Invalidate()
        {
            shader      = nullptr;
            name        = "";
            type        = 0;
            location    = -1;
        }
        
    public:
        
        /// Check if this parameter object is valid
        bool IsValid() { return location != -1; }
        
        /// Gets the type of this parameter
        GLenum GetType() const { return type; }
        
        /// Check documentation for glVertexAttribPointer
        void SetAttributePointer(GLint size,
                                 GLenum type,
                                 GLboolean normalized,
                                 GLsizei stride,
                                 const GLvoid * pointer);

    };

    ///
    /// Shader is a class representing a compiled GPU program
    ///
    class Shader : public Resource
    {
    private:
        friend class ResourceManager;

        /// Local path to vertex shader
        const string vertexPath;
        
        /// Local path to fragment shader
        const string fragmentPath;

        /// GL id (name) of the compiled program
        GLuint program;

        /// Store all the parameters
        std::map<string, unique_ptr<ShaderParameter>> parameters;
        
        /// Store all the attributes
        std::map<string, unique_ptr<ShaderAttribute>> attributes;


    protected:
        /// Create a new shader from a vertex and fragment shader files
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        /// Destroy a shader
        ~Shader();
        
        
        /// Load the shader from a resource
        /// @param cached is a flag used to load the latest
        /// @param fallbackShader is a flag that will force the load of the
        /// fallback (pink) shader in vase the shader requested is not valid
        bool Load(bool cached, bool errorShader = false);

        /// Load the parameters and attributes from the shader file
        void LoadParamters();

    public:
        /// Get the GL id (name) of the compiled program
        GLuint GetProgram() const;

        /// The local path to the vertex shader.
        const string& VertexShaderPath() const { return vertexPath; }
        
        /// The local path to the fragment shader.
        const string& FragmentShaderPath() const { return fragmentPath; }

        /// Test if this shader is valid
        virtual bool IsValid() override;

        /// Reload this shader. It will load the latest version of the shader file,
        /// checking the resource folder and the resource cache
        virtual void Reload(bool cached) override;

        /// Manually invalidate the shader, forcing a reload event later.
        virtual void Invalidate() override;

        /// Return a pointer to a shader parameter with the given name. If no such
        /// parameter is found, a invalid one is returned. An invalid parameter might
        /// become valid after a shader reload, so don't throw it away just yet.
        ShaderParameter* GetParameter(const string& name);

        /// Return a pointer to a shader attribute with the given name. If no such
        /// attribute is found, a invalid one is returned. An invalid attribute might
        /// become valid after a shader reload, so don't throw it away just yet.
        ShaderAttribute* GetAttribute(const string& name);
        
        /// Activates the shader so that it can be used
        void Activate();
        
        /// Validates the shader before drawing
        bool Validate();
    };
    
    ///
    /// A shortcut to setting up lights quickly
    ///
    class LightShaderParameter
    {
        ShaderParameter& position;
        
        ShaderParameter& direction;
        
        ShaderParameter& diffuse;
        
        ShaderParameter& specular;
        
    public:
        
        /// Create a shader
        LightShaderParameter(Effect& shader, const string& name);
        
        /// Set a light. The type will be set from the type
        void SetValue(const Light3D& light);
    };
}

////////////////////////////////////////////////////////////////////////////////
//  Vector3.h
//
//  Created by Bojan Endrovski on 20/11/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math.h>
#include "tinyxml2.h"

using tinyxml2::XMLElement;

namespace Furiosity
{
    ///
    /// A 3d vector
    ///
    class Vector3
    {
    public:
        // The union trick in not standard C++ but should work with the any compiler
        // and makes usage of C style libs a lot easier
        union {
            // Holds all the values 
            float f[3];
            
            struct
            {
                /// Holds the value along the x axis
                float x;
                
                /// Holds the value along the y axis
                float y;
                
                /// Holds the value along the z axis
                float z;
            };
        };
        
        /// Explicit disabling of conversion from float to 
        // Vector3 operator=(const float&) = delete;
        
        // Vector3 operator=(float)        = delete;
        
    public:
        /// The default constructor creates a zero vector.
        Vector3() : x(0), y(0), z(0) {}
        
        /// The explicit constructor creates a vector with the given components
        Vector3(float x, float y, float z)
        : x(x), y(y), z(z) {}
        
        /// The explicit constructor creates a vector with the given components
        explicit Vector3(float v) : x(v), y(v), z(v) {}
        
        // Vector from Xml DOM
        explicit  Vector3(const XMLElement& vec)
        {
            x = vec.FloatAttribute("x");
            y = vec.FloatAttribute("y");
            z = vec.FloatAttribute("z");
        }
        
        float operator[](unsigned i) const
        {
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }
        
        float& operator[](unsigned i)
        {
            if (i == 0) return x;
            if (i == 1) return y;
            return z;
        }
        
        /// Returns the value of the given vector added to this
        Vector3 operator+(const Vector3& v) const
        {
            return Vector3(x+v.x, y+v.y, z+v.z);
        }
        
        /// Returns the value of the given vector subtracted from this
        Vector3 operator-(const Vector3& v) const
        {
            return Vector3(x-v.x, y-v.y, z-v.z);
        }
        
        /// Returns a copy of this vector scaled the given value
        Vector3 operator*(const float value) const
        {
            return Vector3(x*value, y*value, z*value);
        }
        
        /// Calculates and returns the dot product of this
        /// with the given vector
        float operator *(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }
        

        /// Calculates and returns the cross product of this vector
        /// with the given vector
        Vector3 operator%(const Vector3 &vector) const
        {
            return Vector3(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }
        
        /// Adds the given vector to this
        void operator+=(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }
        
        /// Subtracts the given vector from this
        void operator-=(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }
        
        /// Multiplies this vector by the given scalar
        void operator*=(const float value)
        {
            x *= value;
            y *= value;
            z *= value;
        }
        

        /// Calculates and returns a component-wise product of this
        /// vector with the given vector
        Vector3 ComponentProduct(const Vector3 &vector) const
        {
            return Vector3(x * vector.x, y * vector.y, z * vector.z);
        }
        

        /// Performs a component-wise product with the given vector and
        /// sets this vector to its result
        void ComponentProductUpdate(const Vector3 &vector)
        {
            x *= vector.x;
            y *= vector.y;
            z *= vector.z;
        }
        
        /// Calculates and returns the cross product of this vector
        /// with the given vector
        Vector3 Cross(const Vector3 &vector) const
        {
            return Vector3(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }
        
        /// Calculates and returns the dot product of this vector
        /// with the given vector
        float Dot(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }
        
        ///  Gets the magnitude of this vector
        float Magnitude() const
        {		
            return sqrtf(x*x+y*y+z*z);
        }
        
        ///  Gets the squared magnitude of this vector
        float SquareMagnitude() const
        {
            return x*x+y*y+z*z;
        }
        
        /// Limits the size of the vector to the given maximum
        inline void Trim(float size)
        {
            if (SquareMagnitude() > size*size)
            {
                Normalize();
                x *= size;
                y *= size;
                z *= size;
            }
        }
        
        
        /// Limits the size of the vector to the given maximum
        inline void SetMagnitude(float size)
        {
            Normalize();
            x *= size;
            y *= size;
            z *= size;
        }

        
        /// Turns a non-zero vector into a vector of unit length
        void Normalize()
        {
            float l = Magnitude();
            if (l > 0)
            {
                (*this) *= ((float)1)/l;
            }
        }
        
        /// Returns the normalised version of a vector
        Vector3 Unit() const
        {
            Vector3 result = *this;
            result.Normalize();
            return result;
        }
        

        /// Updates this vector to be the cross product of its current
        /// value and the given vector
        inline void operator %=(const Vector3 &vector)
        {
            *this = Cross(vector);
        }
        
        /// Checks if the two vectors have identical components
        bool operator==(const Vector3& other) const
        {
            return x == other.x &&
            y == other.y &&
            z == other.z;
        }
        
        /// Checks if the two vectors have non-identical components
        bool operator!=(const Vector3& other) const
        {
            return !(*this == other);
        }
        
        /// Zero all the components of the vector
        void Clear()
        {
            x = y = z = 0;
        }
        
        ///  Flips all the components of the vector
        void Invert()
        {
            x = -x;
            y = -y;
            z = -z;
        }
        
        float AngleTo(const Vector3& rhs);
        
        Vector3 Slerp(float fact, const Vector3& r)
        {
            float cosAlpha, alpha, sinAlpha;
            Vector3 result;
            
            // get cosine of angle between vectors (-1 -> 1)
            cosAlpha = this->Dot(r);
            
            // get angle (0 -> pi)
            alpha = acosf(cosAlpha);
            
            // get sine of angle between vectors (0 -> 1)
            sinAlpha = sinf(alpha);
            
            // simply perform linear interpolation if sinAlpha = 0
            if(sinAlpha == 0)
                return (*this) + (r - (*this)) * fact;
            

            // this breaks down when sinAlpha = 0, i.e. alpha = 0 or pi
            float t1, t2;
            t1 = sinf( (1.0f - fact) * alpha) / sinAlpha;
            t2 = sinf( fact * alpha ) / sinAlpha;
            
            // interpolate src vectors
            return *this * t1 + r * t2;
        }
        
        static Vector3 Infinity;

        static Vector3 RandomInsideUnitSphere();
        
        static Vector3 RandomOnUnitSphere();
        
        static Vector3 RandBetween(const Vector3& from, const Vector3& to);
    };
    
    /// Multiplication with Rhs Vector
    inline Vector3 operator*(float val, const Vector3& rhs)
    {
        return Vector3(rhs.x * val, rhs.y * val, rhs.z * val);
    }
}


// end //

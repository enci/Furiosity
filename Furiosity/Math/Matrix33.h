////////////////////////////////////////////////////////////////////////////////
//  Matrix33.h
//
//  Created by Bojan Endrovski on 18/03/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef MATRIX_H
#define MATRIX_H

// Framework
#include <cmath>
#include <algorithm>

// Local
#include "Vector2.h"

namespace  Furiosity
{
    /// A matrix implemetation for transforming 2D vectors,
    /// and sometimes 3D vectors
    class Matrix33
    {
    public:
        union
        {
            float f[9];
            float m[3][3];
            struct
            {
                // Todo: Row vs. Column
                float   m11, m12, m13,
                        m21, m22, m23,
                        m31, m32, m33;
            };
        };
    
        
        /// Do care values init
        Matrix33()
        {
            SetIdentity();
        }
        
        /// Totally care values init
        Matrix33(float _m00, float _m01, float _m02,
                 float _m10, float _m11, float _m12,
                 float _m20, float _m21, float _m22)
        {
            m11 = _m00; m12 = _m01; m13 = _m02;
            m21 = _m10; m22 = _m11; m23 = _m12;
            m31 = _m20; m32 = _m21; m33 = _m22;
        }

     
        /// Create from an xml element. This will parse position and rotation
        Matrix33(const XMLElement* settings);
        
        /// Default up vector if this matrix is applied as transormation
        ///
        /// @return the up vector of this matrix
        ///
        inline Vector2 Up() const                   { return Vector2(m21, m22); }
        
        /// Set the up vector
        ///
        /// @up New up vector
        ///
        inline void SetUp(const Vector2& up)        { m21 = up.x; m22 = up.y; }
        
        /// Right vector if this matrix is applied as transormation
        ///
        /// @return The right vector
        ///
        inline Vector2 Right() const                { return Vector2(m11, m12); }
        
        ///
        ///
        ///
        inline void SetRight(const Vector2& fwd)    { m11 = fwd.x; m12 = fwd.y; }

        
        /// Get translation from origin vector
        inline Vector2 Translation() const          { return Vector2(m31, m32); }
        
        /// Get scaling vector
        inline Vector2 Scaling() const              { return Vector2(m11, m22); }
        
        /// Set translation without changing the rest of the properties.
        inline void SetTranslation(const Vector2& translation)
        { m31 = translation.x; m32 = translation.y; m33 = 1.0f; }

        
        /// Sets the matrix to identity
        inline void SetIdentity()
        {
            m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
            m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
            m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
        }
        
        /// Sets the scale scale along the x and y axis
        ///
        /// @scale Scaling vector
        ///
        inline void SetScale(const Vector2& scale)
        {
            
			m11 *= scale.x;
            m21 *= scale.y;
            m12 *= scale.x;
            m22 *= scale.y;
        }
        
        /// Set the orientation, without changing the rest of the properties. To keep 
        /// transformations rigid the vectors must be perpedicular and normalized.
        ///
        /// @up     New up vector
        /// @right  New right vector
        ///
        inline void SetOrientation(const Vector2& up, const Vector2& right)
        {
            m11 = right.x;  m12 = right.y;  m13 = 0.0f;
            m21 = up.x;     m22 = up.y;     m23 = 0.0f;
        }
                
        /// Set orientation and traslation. To keep transformations rigid,
        ///  the up and right vectors must be perpedicular and normalized.
        ///
        /// @up             New up vector
        /// @right          New right vector
        /// @translation    New position vector
        ///
        inline void SetTransform(const Vector2& up,
                                 const Vector2& right,
                                 const Vector2& translation)
        {
            m11 = right.x;          m12 = right.y;          m13 = 0.0f;
            m21 = up.x;             m22 = up.y;             m23 = 0.0f;
            m31 = translation.x;    m32 = translation.y;    m33 = 1.0f;
        }
        
        /*
        /// Set the rotation of the matrix. This does not effect the trasnaltion
        inline void SetRotation(float theta)
        {
            m11 = cosf(theta);  m12 = -sinf(theta);
            m21 = sinf(theta);  m22 = m11;
        }
         */
        
        
        /// Set the rotation of the matrix. This does not effect the trasnaltion
        inline void SetRotation(float theta)
        {
            m11 = cosf(theta);  m12 = sinf(theta);
            m21 = -sinf(theta);  m22 = m11;
        }

        
        // TODO: Check this
        inline float Rotation() const
        {
//            return atan2f(m21, m11);
//            return (float)asin(m21);
            
            const float TwoPi = 3.1415926535897932384626433832795f * 2;
            float r = atan2f(m12, m11);
            if(r < 0) r = TwoPi + r;
            return r;
        }
        
        /*
        inline float Rotation()
        {
            return atan2f(m21, m11);
            //            return (float)asin(m21);
        }
         */
        
        /// Multiplies this matrix with an another one and saves the result in the current one.
        inline void Multiply(const Matrix33& matrix)
        {
            Matrix33 temp = *this;
            
            // First row
            this->m11 = matrix.m11 * temp.m11 + matrix.m12 * temp.m21 + matrix.m13 * temp.m31;
            this->m12 = matrix.m11 * temp.m12 + matrix.m12 * temp.m22 + matrix.m13 * temp.m32;
            this->m13 = matrix.m11 * temp.m13 + matrix.m12 * temp.m23 + matrix.m13 * temp.m33;
            
            // Second row
            this->m21 = matrix.m21 * temp.m11 + matrix.m22 * temp.m21 + matrix.m23 * temp.m31;
            this->m22 = matrix.m21 * temp.m12 + matrix.m22 * temp.m22 + matrix.m23 * temp.m32;
            this->m23 = matrix.m21 * temp.m13 + matrix.m22 * temp.m23 + matrix.m23 * temp.m33;
            
            // Third row
            this->m31 = matrix.m31 * temp.m11 + matrix.m32 * temp.m21 + matrix.m33 * temp.m31;
            this->m32 = matrix.m31 * temp.m12 + matrix.m32 * temp.m22 + matrix.m33 * temp.m32;
            this->m33 = matrix.m31 * temp.m13 + matrix.m32 * temp.m23 + matrix.m33 * temp.m33;
        }
		
        
        /// Translate this tranformation
		inline void Translate(float x, float y)
		{
			// Create an translation matrix
			Matrix33 mat;
			mat.m11 = 1.0f; mat.m12 = 0.0f; mat.m13 = 0.0f;
			mat.m21 = 0.0f; mat.m22 = 1.0f; mat.m23 = 0.0f;
			mat.m31 = x;    mat.m32 = y;    mat.m33 = 1.0f;
            //
			Multiply(mat);
		}
        
        /// Translate this tranformation
		inline void Translate(const Vector2& pos)
		{
			Translate(pos.x, pos.y);
		}
        
        /*
        /// <summary>
        /// 2D rotation
        /// </summary>
        /// <param name="rotation">the amount of rotation in degrees; always starts from 0</param>
        public void Rotate2D(float rotation)
        {
            float radians = MathHelper.ToRadians(rotation);
            
            this.m11 = (float)Math.Cos(radians); this.m12 = (float)-Math.Sin(radians);
            this.m21 = (float)Math.Sin(radians); this.m22 = this.m11;
        }
    
        public void Translate2D(Vector2D translation)
        {
            this.m13 = translation.X;
            this.m23 = translation.Y;
        }
        
        
        /// <summary>
        /// Add orientation (rotation)
        /// </summary>
		public void Rotate(Vector2D fwd, Vector2D side)
		{
			Matrix3x3 mat = new Matrix3x3();
			mat.m11 = fwd.X; mat.m12 = fwd.Y; mat.m13 = 0.0f;
			mat.m21 = side.X; mat.m22 = side.Y; mat.m23 = 0.0f;
			mat.m31 = 0.0f; mat.m32 = 0.0f; mat.m33 = 1.0f;
			
			// Mutliply
			Multiply(ref mat);
        }
        */
        
        /// Multiply with a scalar
        ///
        /// @f Scalar value
        ///
        void Multiply(float f)
        {
            m11 *= f; m12 *= f; m13 *= f;
            m21 *= f; m22 *= f; m23 *= f;
            m31 *= f; m32 *= f; m33 *= f;
        }
        
        /// Inverse of this matrix
        ///
        /// @return The inverse matrix if there is one, undefined otherwise
        ///
        Matrix33 Inverse() const
        {
            float determinant = m11 * (m33 * m22 - m32 * m23)
                                - m21 * (m33 * m12 - m32 * m13)
                                + m31 * (m23 * m12 - m22 * m13);
            //
            float invrdet = 1.0f / determinant;
            
            Matrix33 t;
            //
            t.m11 = m33 * m22 - m32 * m23;
            t.m12 = -(m33 * m12 - m32 * m13);
            t.m13 = m23 * m12 - m22 * m13;
            //
            t.m21 = -(m33 * m21 - m31 * m23);
            t.m22 = m33 * m11 - m31 * m13;
            t.m23 = -(m23 * m11 - m21 * m13);
            //
            t.m31 = m32 * m21 - m31 * m22;
            t.m32 = -(m32 * m11 - m31 * m12);
            t.m33 = m22 * m11 - m21 * m12;
            
            t.Multiply(invrdet);
            //
            return t;
        }
        
        /// Transform a 2D vector
        ///
        /// @vector Transforms this vector and stores the result in it
        ///
        inline void TransformVector2(Vector2& vector) const
		{
			float tempX = (m11 * vector.x) + (m21 * vector.y) + m31;
			float tempY = (m12 * vector.x) + (m22 * vector.y) + m32;
			vector.x = tempX;
			vector.y = tempY;
		}
        
        /// Tranforms a normal vector, does not affect translation
        inline void TransformNormal2(Vector2& vector) const
		{
			float tempX = (m11 * vector.x) + (m21 * vector.y);
			float tempY = (m12 * vector.x) + (m22 * vector.y);
			vector.x = tempX;
			vector.y = tempY;
		}
        
        /// Invert this matrix
        inline Matrix33& Invert()
        {
            *this = Inverse();
            return *this;
        }
        
        
        ///
        inline Matrix33& Transpose()
        {
            std::swap(m12, m21);
            std::swap(m13, m31);
            std::swap(m23, m32);
            return *this;
        }
        
        
        static Matrix33 CreateScale(Vector2 scale)
        {
            // Set zeros
            Matrix33 res;
            memset((void*)res.m, 0x0, 3 * 3 * sizeof(float));
            //
            res.m[0][0] = scale.x;
            res.m[1][1] = scale.y;
            res.m[2][2] = 1.0f;
            //
            return res;
        }
        
        /// The identity matrix
        static Matrix33 Identity;
    };
}

#endif
////////////////////////////////////////////////////////////////////////////////
//  Matrix44.h
//
//  Created by Bojan Endrovski on 21/11/2010.
//  Copyright 2010 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef MATRIX44_H
#define MATRIX44_H

#include <math.h>
//#include <string.h>

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix33.h"

namespace Furiosity
{
    class Matrix44
    {
    public:
        union
        {
            /// Matrix elements
            float m[4][4];
            float f[16];
            struct
            {
                float m00; float m01; float m02; float m03;
                float m10; float m11; float m12; float m13;
                float m20; float m21; float m22; float m23;
                float m30; float m31; float m32; float m33;
            };
        };
        
    public:
        
        /// Constructs a new matrix, the values are row major (I think)
        ///
        /// @param m00 The m[0][0] value of the matrix
        /// @param m01 What do you think?
        Matrix44(float m00, float m01, float m02, float m03,
                 float m10, float m11, float m12, float m13,
                 float m20, float m21, float m22, float m23,
                 float m30, float m31, float m32, float m33)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
        }
        
        
        /// Constructs a new matrix, the values are row major (I think)
        ///
        /// @param m00 The m[0][0] value of the matrix
        /// @param m01 What do you think?
        Matrix44(const Matrix33& mtx)
        {
            m[0][0] = mtx.m11; m[0][1] = mtx.m12; m[0][2] = mtx.m13; m[0][3] = 0;
            m[1][0] = mtx.m21; m[1][1] = mtx.m22; m[1][2] = mtx.m23; m[1][3] = 0;
            m[2][0] = mtx.m31; m[2][1] = mtx.m32; m[2][2] = mtx.m33; m[2][3] = 0;
            m[3][0] = 0;       m[3][1] = 0;       m[3][2] = 0;       m[3][3] = 1;
        }
        
        /// Constructs a new identity matrix
        Matrix44()
        {
            // Set zeros
            memset((void*)m, 0x0, 4 * 4 * sizeof(float));
            //
            m[0][0] = 1.0f;
            m[1][1] = 1.0f;
            m[2][2] = 1.0f;
            m[3][3] = 1.0f;
        }
        
        /// Constructs a new diagonal matrix
        ///
        /// @param diagval A value to set along the matrix diagonal
        Matrix44(float diagval)
        {
            // Set zeros
            memset((void*)m, 0x0, 4 * 4 * sizeof(float));
            //
            m[0][0] = diagval;
            m[1][1] = diagval;
            m[2][2] = diagval;
            m[3][3] = diagval;
        }
        
        /// Transform the given vector by this matrix.
        ///
        /// @param vec vector that is asssumed to be homogenuos with w=1
        /// @return Resulting vector is asssumed to be homogenuos with w=1
        Vector3 operator*(const Vector3& vec) const
        {
            return Vector3(vec.x * m[0][0] +
                           vec.y * m[1][0] +
                           vec.z * m[2][0] + m[3][0],
                           
                           vec.x * m[0][1] +
                           vec.y * m[1][1] +
                           vec.z * m[2][1] + m[3][1],
                           
                           vec.x * m[0][2] +
                           vec.y * m[1][2] +
                           vec.z * m[2][2] + m[3][2]);
        }
        
        /// Transform a given vector by this matrix
        ///
        /// @param vec Homogenuos vector  with explicit w value
        /// @return Homogenuos vector with explicit non-normalized w value
        Vector4 operator*(const Vector4& vec) const
        {
            Vector4 res(f[0] * vec.f[0] + f[4] * vec.f[1] + f[8]  * vec.f[2] + f[12] * vec.f[3],
                        f[1] * vec.f[0] + f[5] * vec.f[1] + f[9]  * vec.f[2] + f[13] * vec.f[3],
                        f[2] * vec.f[0] + f[6] * vec.f[1] + f[10] * vec.f[2] + f[14] * vec.f[3],
                        f[3] * vec.f[0] + f[7] * vec.f[1] + f[11] * vec.f[2] + f[15] * vec.f[3]);
            return res;
        }
        
        /// Matrix addition
        ///
        /// @param mat Right side operand
        Matrix44 operator+(const Matrix44& mat) const;
        
        /// Matrix substraction
        ///
        /// @param mat Right side operand
        Matrix44 operator-(const Matrix44& mat) const;
        
        // Matrix multiplication
        ///
        /// @param mat Right side operand
        Matrix44 operator*(const Matrix44& mat) const;
        
        
        void SetIdentity()
        {
            // Set zeros
            memset((void*)m, 0x0, 4 * 4 * sizeof(float));
            //
            m[0][0] = 1.0f;
            m[1][1] = 1.0f;
            m[2][2] = 1.0f;
            m[3][3] = 1.0f;
        }
        
        /// Translation bit of the matrix
        inline Vector3 Translation() const
        {
            return Vector3(m[3][0],  m[3][1],  m[3][2]);
        }
        
        /// Set the transltion of the matrix
        inline void SetTranslation(float x, float y, float z)
        {
            m[3][0] = x;
            m[3][1] = y;
            m[3][2] = z;
        }
        
        /// Set the transltion of the matrix
        inline void SetTranslation(const Vector3& vec)
        {
            m[3][0] = vec.x;
            m[3][1] = vec.y;
            m[3][2] = vec.z;
        }
        

        /// Set the scale of the matrix. This will chage the rotation.
        ///
        /// @param scale A vector containing the compenent
        inline void SetScale(Vector3 scale)
        {
            m[0][0] = scale.x;
            m[1][1] = scale.y;
            m[2][2] = scale.z;
        }
        
        /// Get the scale components.
        ///
        ///
        inline Vector3 GetScale() const
        {
            return Vector3(m[0][0], m[1][1], m[2][2]);
        }
        
        
        
        /// Creates an identity matrix
        ///
        /// @return Identity matrix
        static Matrix44 CreateIdentity()
        {
            return Matrix44();		
        }
        
        /// Creates a transation matrix
        ///
        /// @return Translation matrix
        static Matrix44 CreateTranslation(float x, float y, float z)
        {
            Matrix44 result;
            result.SetIdentity();
            //
            result.m[3][0] = x;
            result.m[3][1] = y;
            result.m[3][2] = z;
            //
            return result;
        }
        
        static Matrix44 CreateScale(Vector3 scale)
        {
            // Set zeros
            Matrix44 res;
            memset((void*)res.m, 0x0, 4 * 4 * sizeof(float));
            //
            res.m[0][0] = scale.x;
            res.m[1][1] = scale.y;
            res.m[2][2] = scale.z;
            res.m[3][3] = 1.0f;
            //
            return res;
        }
        
        static Matrix44 CreateScale(float scale)
        {
            // Set zeros
            Matrix44 res;
            memset((void*)res.m, 0x0, 4 * 4 * sizeof(float));
            //
            res.m[0][0] = scale;
            res.m[1][1] = scale;
            res.m[2][2] = scale;
            res.m[3][3] = 1.0f;
            //
            return res;
        }
        
        /// Get the determinant of this matrix
        float Determinant() const;
        
        
        /// Inverts this matrix
        const Matrix44& Invert();
        
        /// Transposes this matrix
        void Transpose();
        
        /// Sets the orientation of the matrix to the orthogonal basis vector
        /// It perfoms no checks on the orthogonality!
        ///
        /// @param x X orthogonal basis vector
        /// @param y Y orthogonal basis vector
        /// @param z Z orthogonal basis vector
        void SetOrientation(const Vector3& x,
                            const Vector3& y,
                            const Vector3& z);
        
        ///
        Vector3 GetXAxis() const { return Vector3(m00, m01, m02); }
        
        ///
        Vector3 GetYAxis() const { return Vector3(m10, m11, m12); }
        
        ///
        Vector3 GetZAxis() const { return Vector3(m20, m21, m22); }
        
        ///
        void SetEulerAxis(float yaw, float pitch, float roll);
        
        /// Rotates this matrix
        Matrix44& Rotate(float angle, float x, float y, float z);
        
        /// Get the rotational part of the transformation only
        ///
        /// @return 3x3 submatrix
        Matrix33 GetMatrix33() const;
        
        /// Creates a rotation matrix around an arbitrary axis
        static Matrix44 CreateRotate(float angle, float x, float y, float z);
        
        /// Creates a rotation matrix around an arbitrary axis
        static Matrix44 CreateRotate(float angle, const Vector3& axis);
        
        static Matrix44 CreateRotateX(float angle);
        
        static Matrix44 CreateRotateY(float angle);
        
        static Matrix44 CreateRotateZ(float angle);
        
        /// Creates an orthographic projection matrix
        static Matrix44 CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ);
        
        /// Creates a frustum projection matrix
        static Matrix44 CreateFrustum(float left, float right, float bottom, float top, float nearZ, float farZ);
        
        /// Creates a perspective projection matrix from camera settings
        static Matrix44 CreatePerspective(float fovy, float aspect, float nearZ, float farZ);
        
        /// Creates a look at matrix, usualy a view matrix  
        static Matrix44 CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
        
        /// Creates a  matrix
        static Matrix44 CreateFromQuaternion(float quaternion[]);

        // Streaming
        void SaveToXml(XMLElement& element);
        
        void ReadFromXml(const XMLElement& element);
        
        Vector3 TransformDirecton(const Vector3& direction);
        
        Vector3 Transform(const Vector3& vector);
    };
}


#endif

// end //

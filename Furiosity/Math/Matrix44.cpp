////////////////////////////////////////////////////////////////////////////////
//  Matrix44.cpp
//
//  Created by Bojan Endrovski on 21/11/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Matrix44.h"
#include "Utils.h"
#include "Frmath.h"

#include <algorithm>

using namespace Furiosity;
using namespace std;

void q_to_m33(float *m, const float *q)
{
	const float& q0 = q[0];
	const float& q1 = q[1];
	const float& q2 = q[2];
	const float& q3 = q[3];

	const float q00 = q0*q0;
	const float q11 = q1*q1;
	const float q22 = q2*q2;
	const float q33 = q3*q3;

	const float q01 = q0*q1;
	const float q02 = q0*q2;
	const float q03 = q0*q3;
	const float q12 = q1*q2;
	const float q13 = q1*q3;
	const float q23 = q2*q3;

	const size_t stride = 3;
	const size_t stride2 = stride + stride;

	m[0] =	 q00 + q11 - q22 - q33;
	m[stride+0] =	(q12 - q03) * 2.0f;
	m[stride2+0] = (q13 + q02) * 2.0f;

	m[1] = (q12 + q03) * 2.0f;
	m[stride+1] =  q00 - q11 + q22 - q33;
	m[stride2+1] = (q23 - q01) * 2.0f;

	m[2] = (q13 - q02) * 2.0f;
	m[stride+ 2] = (q23 + q01) * 2.0f;
	m[stride2+ 2] =  q00 - q11 - q22 + q33;
}


////////////////////////////////////////////////////////////////////////////////
// Matrix addition
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::operator+(const Matrix44& mat) const
{
	Matrix44 res;
	//
	for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res.m[i][j] =m[i][j] + mat.m[i][j];
    
    return res;
}

////////////////////////////////////////////////////////////////////////////////
// Matrix addition
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::operator-(const Matrix44& mat) const
{
	Matrix44 res;
	//
	for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res.m[i][j] = m[i][j] - mat.m[i][j];
    
    return res;
}



////////////////////////////////////////////////////////////////////////////////
// Matrix multiplication, slow but reliable-ish :)
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::operator*(const Matrix44& mat) const
{
    /*
	Matrix44 res;
	//
	for (int i=0; i<4; i++)
	{
		res.m[i][0] =	m[i][0] * mat.m[0][0] +
						m[i][1] * mat.m[1][0] +
						m[i][2] * mat.m[2][0] +
						m[i][3] * mat.m[3][0];
		
		res.m[i][1] =	m[i][0] * mat.m[0][1] +
						m[i][1] * mat.m[1][1] +
						m[i][2] * mat.m[2][1] +
						m[i][3] * mat.m[3][1];
		
		res.m[i][2] =	m[i][0] * mat.m[0][2] +
						m[i][1] * mat.m[1][2] +
						m[i][2] * mat.m[2][2] +
						m[i][3] * mat.m[3][2];
		
		res.m[i][3] =	m[i][0] * mat.m[0][3] +
						m[i][1] * mat.m[1][3] +
						m[i][2] * mat.m[2][3] +
						m[i][3] * mat.m[3][3];
	}
	//
	return res;
    */
    
    Matrix44 res;
	//
	for (int i=0; i<4; i++)
	{
		res.m[i][0] =	mat.m[i][0] * m[0][0] +
                        mat.m[i][1] * m[1][0] +
                        mat.m[i][2] * m[2][0] +
                        mat.m[i][3] * m[3][0];
		
		res.m[i][1] =	mat.m[i][0] * m[0][1] +
                        mat.m[i][1] * m[1][1] +
                        mat.m[i][2] * m[2][1] +
                        mat.m[i][3] * m[3][1];
		
		res.m[i][2] =	mat.m[i][0] * m[0][2] +
                        mat.m[i][1] * m[1][2] +
                        mat.m[i][2] * m[2][2] +
                        mat.m[i][3] * m[3][2];
		
		res.m[i][3] =	mat.m[i][0] * m[0][3] +
                        mat.m[i][1] * m[1][3] +
                        mat.m[i][2] * m[2][3] +
                        mat.m[i][3] * m[3][3];
	}
	//
	return res;
}


////////////////////////////////////////////////////////////////////////////////
// CreateRotate
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateRotate(float angle, float x, float y, float z)
{
    
    Vector3 v(x, y, z);
    v.Normalize();
    float cos = cosf(angle);
    float cosp = 1.0f - cos;
    float sin = sinf(angle);
    
    Matrix44 m (cos + cosp * v.f[0] * v.f[0],
        cosp * v.f[0] * v.f[1] + v.f[2] * sin,
        cosp * v.f[0] * v.f[2] - v.f[1] * sin,
        0.0f,
        cosp * v.f[0] * v.f[1] - v.f[2] * sin,
        cos + cosp * v.f[1] * v.f[1],
        cosp * v.f[1] * v.f[2] + v.f[0] * sin,
        0.0f,
        cosp * v.f[0] * v.f[2] + v.f[1] * sin,
        cosp * v.f[1] * v.f[2] - v.f[0] * sin,
        cos + cosp * v.f[2] * v.f[2],
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f);
    
    return m;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a rotation matrix around an arbitrary axis
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateRotate(float angle, const Vector3& axis)
{
    return CreateRotate(angle, axis.x, axis.y, axis.z);
}


////////////////////////////////////////////////////////////////////////////////
// CreateRotateX
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateRotateX(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);
    //
    Matrix44 m(1.0f, 0.0f, 0.0f, 0.0f,
               0.0f, cos, sin, 0.0f,
               0.0f, -sin, cos, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
    //
    return m;
}

////////////////////////////////////////////////////////////////////////////////
// CreateRotateY
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateRotateY(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);
    //
    Matrix44 m(cos, 0.0f, -sin, 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
               sin, 0.0f, cos, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
    //
    return m;
}

////////////////////////////////////////////////////////////////////////////////
// CreateRotateZ
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateRotateZ(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);
    //
    Matrix44 m(cos, sin, 0.0f, 0.0f,
               -sin, cos, 0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f);
    //
    return m;
}

////////////////////////////////////////////////////////////////////////////////
// Create a frustum projection. From Guide to OpenGL ES 2.0
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateFrustum(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
	//
    Matrix44    frust;
	frust.SetIdentity();
	
    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
		(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
		return frust;
	
    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;
	
    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;
	
    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;
	
    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;
	
    return frust;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a perspective projection. From Guide to OpenGL ES 2.0
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreatePerspective(float fovy, float aspect, float nearZ, float farZ)
{
    float cotan = 1.0f / tanf(fovy / 2.0f);
    
    Matrix44 m(cotan / aspect, 0.0f, 0.0f, 0.0f,
               0.0f, cotan, 0.0f, 0.0f,
               0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
               0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f);
    return m;
    
    /*
	float frustumW, frustumH;
	
	frustumH = tanf( fovy / 2 ) * nearZ;
	frustumW = frustumH * aspect;
	
	return CreateFrustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
     */
}

////////////////////////////////////////////////////////////////////////////////
// Create Orthographic projection. From Guide to OpenGL ES 2.0
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	// Get bounds
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
	//
    Matrix44    ortho;
	ortho.SetIdentity();
	
	// Check bad parameters
    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return ortho;
	
	// 
	ortho.SetIdentity();
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;
	
    return ortho;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a look at matrix for OpenGL rendering
////////////////////////////////////////////////////////////////////////////////
Matrix44 Matrix44::CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
    Vector3 n = eye - center;
    n.Normalize();
    //
    Vector3 u = up.Cross(n);
    u.Normalize();
    //
    Vector3 v = n.Cross(u);
    
    Matrix44 m( u.f[0], v.f[0], n.f[0], 0.0f,
                u.f[1], v.f[1], n.f[1], 0.0f,
                u.f[2], v.f[2], n.f[2], 0.0f,
                (u * -1).Dot(eye),
                (v * -1).Dot(eye),
                (n * -1).Dot(eye),
               1.0f);
    
    return m;
}


float Matrix44::Determinant() const
{
    /*
    return  f[8] * f[5] * f[2]+
            f[4] * f[9] * f[2]+
            f[8] * f[1] * f[6]-
            f[0] * f[9] * f[6]-
            f[4] * f[1] * f[10]+
            f[0] * f[5] * f[10];
     */
    
    return  f[2] * f[5] * f[8]+
            f[1] * f[6] * f[8]+
            f[2] * f[4] * f[9]-
            f[0] * f[6] * f[9]-
            f[1] * f[4] * f[10]+
            f[0] * f[5] * f[10];
}


const Matrix44& Matrix44::Invert()
{
    /*
    /// TODO: Check this
    
    // Make sure the determinant is non-zero.
    float det = Determinant();
    if (det == 0) return;
    det = 1.0f / det;
    
    Matrix44 m = *this; // Copy this
    
    f[0] = (-m.f[9]*m.f[6]+m.f[5]*m.f[10])*det;
    f[4] = (m.f[8]*m.f[6]-m.f[4]*m.f[10])*det;
    f[8] = (-m.f[8]*m.f[5]+m.f[4]*m.f[9])*det;
    
    f[1] = (m.f[9]*m.f[2]-m.f[1]*m.f[10])*det;
    f[5] = (-m.f[8]*m.f[2]+m.f[0]*m.f[10])*det;
    f[9] = (m.f[8]*m.f[1]-m.f[0]*m.f[9])*det;
    
    f[2] = (-m.f[5]*m.f[2]+m.f[1]*m.f[6])*det;
    f[6] = (+m.f[4]*m.f[2]-m.f[0]*m.f[6])*det;
    f[10] = (-m.f[4]*m.f[1]+m.f[0]*m.f[5])*det;
    
    f[3] = (m.f[9]*m.f[6]*m.f[3]
               -m.f[5]*m.f[10]*m.f[3]
               -m.f[9]*m.f[2]*m.f[7]
               +m.f[1]*m.f[10]*m.f[7]
               +m.f[5]*m.f[2]*m.f[11]
               -m.f[1]*m.f[6]*m.f[11])*det;
    f[7] = (-m.f[8]*m.f[6]*m.f[3]
               +m.f[4]*m.f[10]*m.f[3]
               +m.f[8]*m.f[2]*m.f[7]
               -m.f[0]*m.f[10]*m.f[7]
               -m.f[4]*m.f[2]*m.f[11]
               +m.f[0]*m.f[6]*m.f[11])*det;
    f[11] =(m.f[8]*m.f[5]*m.f[3]
               -m.f[4]*m.f[9]*m.f[3]
               -m.f[8]*m.f[1]*m.f[7]
               +m.f[0]*m.f[9]*m.f[7]
               +m.f[4]*m.f[1]*m.f[11]
               -m.f[0]*m.f[5]*m.f[11])*det;
     */
    
    Matrix44 inv;
    
//    double inv[16], det;
    int i;
    
    inv.f[0] =  f[5]  * f[10] * f[15] -
                f[5]  * f[11] * f[14] -
                f[9]  * f[6]  * f[15] +
                f[9]  * f[7]  * f[14] +
                f[13] * f[6]  * f[11] -
                f[13] * f[7]  * f[10];
    
    inv.f[4] = -f[4]  * f[10] * f[15] +
                f[4]  * f[11] * f[14] +
                f[8]  * f[6]  * f[15] -
                f[8]  * f[7]  * f[14] -
                f[12] * f[6]  * f[11] +
                f[12] * f[7]  * f[10];
    
    inv.f[8] =  f[4]  * f[9] * f[15] -
                f[4]  * f[11] * f[13] -
                f[8]  * f[5] * f[15] +
                f[8]  * f[7] * f[13] +
                f[12] * f[5] * f[11] -
                f[12] * f[7] * f[9];
    
    inv.f[12] = -f[4]  * f[9] * f[14] +
    f[4]  * f[10] * f[13] +
    f[8]  * f[5] * f[14] -
    f[8]  * f[6] * f[13] -
    f[12] * f[5] * f[10] +
    f[12] * f[6] * f[9];
    
    inv.f[1] = -f[1]  * f[10] * f[15] +
    f[1]  * f[11] * f[14] +
    f[9]  * f[2] * f[15] -
    f[9]  * f[3] * f[14] -
    f[13] * f[2] * f[11] +
    f[13] * f[3] * f[10];
    
    inv.f[5] = f[0]  * f[10] * f[15] -
    f[0]  * f[11] * f[14] -
    f[8]  * f[2] * f[15] +
    f[8]  * f[3] * f[14] +
    f[12] * f[2] * f[11] -
    f[12] * f[3] * f[10];
    
    inv.f[9] = -f[0]  * f[9] * f[15] +
    f[0]  * f[11] * f[13] +
    f[8]  * f[1] * f[15] -
    f[8]  * f[3] * f[13] -
    f[12] * f[1] * f[11] +
    f[12] * f[3] * f[9];
    
    inv.f[13] = f[0]  * f[9] * f[14] -
    f[0]  * f[10] * f[13] -
    f[8]  * f[1] * f[14] +
    f[8]  * f[2] * f[13] +
    f[12] * f[1] * f[10] -
    f[12] * f[2] * f[9];
    
    inv.f[2] = f[1]  * f[6] * f[15] -
    f[1]  * f[7] * f[14] -
    f[5]  * f[2] * f[15] +
    f[5]  * f[3] * f[14] +
    f[13] * f[2] * f[7] -
    f[13] * f[3] * f[6];
    
    inv.f[6] = -f[0]  * f[6] * f[15] +
    f[0]  * f[7] * f[14] +
    f[4]  * f[2] * f[15] -
    f[4]  * f[3] * f[14] -
    f[12] * f[2] * f[7] +
    f[12] * f[3] * f[6];
    
    inv.f[10] = f[0]  * f[5] * f[15] -
    f[0]  * f[7] * f[13] -
    f[4]  * f[1] * f[15] +
    f[4]  * f[3] * f[13] +
    f[12] * f[1] * f[7] -
    f[12] * f[3] * f[5];
    
    inv.f[14] = -f[0]  * f[5] * f[14] +
    f[0]  * f[6] * f[13] +
    f[4]  * f[1] * f[14] -
    f[4]  * f[2] * f[13] -
    f[12] * f[1] * f[6] +
    f[12] * f[2] * f[5];
    
    inv.f[3] = -f[1] * f[6] * f[11] +
    f[1] * f[7] * f[10] +
    f[5] * f[2] * f[11] -
    f[5] * f[3] * f[10] -
    f[9] * f[2] * f[7] +
    f[9] * f[3] * f[6];
    
    inv.f[7] = f[0] * f[6] * f[11] -
    f[0] * f[7] * f[10] -
    f[4] * f[2] * f[11] +
    f[4] * f[3] * f[10] +
    f[8] * f[2] * f[7] -
    f[8] * f[3] * f[6];
    
    inv.f[11] = -f[0] * f[5] * f[11] +
    f[0] * f[7] * f[9] +
    f[4] * f[1] * f[11] -
    f[4] * f[3] * f[9] -
    f[8] * f[1] * f[7] +
    f[8] * f[3] * f[5];
    
    inv.f[15] = f[0] * f[5] * f[10] -
    f[0] * f[6] * f[9] -
    f[4] * f[1] * f[10] +
    f[4] * f[2] * f[9] +
    f[8] * f[1] * f[6] -
    f[8] * f[2] * f[5];
    
    float det = f[0] * inv.f[0] + f[1] * inv.f[4] + f[2] * inv.f[8] + f[3] * inv.f[12];
    
    if (det == 0)
        return *this; // false
    
    det = 1.0 / det;
    
    for (i = 0; i < 16; i++)
        f[i] = inv.f[i] * det;
    
    return *this; // true
}


void Matrix44::Transpose()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < i; j++)
            std::swap(m[i][j], m[j][i]);
}

void Matrix44::SetOrientation(const Furiosity::Vector3 &x,
                              const Furiosity::Vector3 &y,
                              const Furiosity::Vector3 &z)
{
    // Option 1
    m00 = x.x; m01 = x.y; m02 = x.z;
    m10 = y.x; m11 = y.y; m12 = y.z;
    m20 = z.x; m21 = z.y; m22 = z.z;
 
    /*
    // Option 2
    m00 = x.x; m01 = y.x; m02 = z.x;
    m10 = x.y; m11 = y.y; m12 = z.y;
    m20 = x.z; m21 = y.z; m22 = z.z;
   */
}

void Matrix44::SetEulerAxis(float yaw, float pitch, float roll)
{
    float phi      = 0;
    float psi      = yaw;   // pitch;
    float theta    = 0;     // roll;
    //
    SetIdentity();
    m[0][0]	= cosf(psi) * cosf(phi) - cosf(theta) * sinf(phi) * sinf(psi);
    m[0][1]	= cosf(psi) * sinf(phi) + cosf(theta) * cosf(phi) * sinf(psi);
    m[0][2]	= sinf(psi) * sinf(theta);
    //
    m[1][0]	= -sinf(psi) * cosf(phi) - cosf(theta) * sinf(phi) * cosf(psi);
    m[1][1]	= -sinf(psi) * sinf(phi) + cosf(theta) * cosf(phi) * cosf(psi);
    m[1][2]	= cosf(psi) * sinf(theta);
    //
    m[2][0]	= sinf(theta) * sinf(phi);
    m[2][1]	= -sinf(theta) * cosf(phi);
    m[2][2]	= cosf(theta);
    Transpose();
}

Matrix44& Matrix44::Rotate(float angle, float x, float y, float z)
{
    Matrix44 rot = Matrix44::CreateRotate(angle, x, y, z);
    *this = *this * rot;
    return *this;
}

Matrix33 Matrix44::GetMatrix33() const
{
    return Matrix33(m00, m01, m02,
                    m10, m11, m12,
                    m20, m21, m22);
}

void Matrix44::SaveToXml(XMLElement &element)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            string name = "m" + ToString(i) + ToString(j);
            element.SetAttribute( name.c_str(), ToString( m[i][j]).c_str() );
        }
    }
}

void Matrix44::ReadFromXml(const XMLElement &element)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            string name = "m" + ToString(i) + ToString(j);
            m[i][j] = element.FloatAttribute(name.c_str());
        }
    }
}

Matrix44 Matrix44::CreateFromQuaternion(float quaternion[])
{
	Matrix33 rot;
	q_to_m33(rot.f, quaternion);
	return Matrix44(rot);
}

Vector3 Matrix44::TransformDirecton(const Vector3& dir)
{
    Vector3 res(dir.x * m[0][0] +
                dir.y * m[1][0] +
                dir.z * m[2][0],
            
                dir.x * m[0][1] +
                dir.y * m[1][1] +
                dir.z * m[2][1],
            
                dir.x * m[0][2] +
                dir.y * m[1][2] +
                dir.z * m[2][2]);
    return res;
}

Vector3 Matrix44::Transform(const Vector3& vec)
{
    Vector3 res(vec.x * m[0][0] +
                vec.y * m[1][0] +
                vec.z * m[2][0] + m[3][0],
            
                vec.x * m[0][1] +
                vec.y * m[1][1] +
                vec.z * m[2][1] + m[3][1],
            
                vec.x * m[0][2] +
                vec.y * m[1][2] +
                vec.z * m[2][2] + m[3][2]);
    return res;
}


// end

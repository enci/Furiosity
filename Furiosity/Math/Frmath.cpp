////////////////////////////////////////////////////////////////////////////////
//  Frmath.cpp
//
//  Created by Bojan Endrovski on 23/09/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "Frmath.h"

using namespace Furiosity;

int Furiosity::Factorial(int val) {
    
    // Precomputed :)
    switch(val)
    {
        case 0: return 1;
        case 1: return 1;
        case 2: return 2;
        case 3: return 6;
        case 4: return 24;
        case 5: return 120;
        case 6: return 720;
    }
    
    int r = 1;
    
    while( val-- > 1) {
        r *= (val + 1);
    }
    
    return r;
}

void Furiosity::PolarToCartesian(float r, float theta, float &x, float &y)
{
    x = r * sinf(theta);
    y = r * cosf(theta);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void Furiosity::CartesianToSpherical(float& r, float& theta, float& fi,
                                     float x, float y, float z)
{
    r       = sqrtf(x*x + y*y + z*z);
    theta   = acosf(z/r);
    fi      = atanf(y/x);
}

////////////////////////////////////////////////////////////////////////////////
// Spherical to cartesian
////////////////////////////////////////////////////////////////////////////////
void Furiosity::SphericalToCartesian(float& x, float& y, float& z,
                                     float r, float theta, float fi)
{
    x = r * sinf(theta) * cosf(fi);
    y = r * cosf(theta);
    z = r * sinf(theta) * sinf(fi);
}

////////////////////////////////////////////////////////////////////////////////
// Spherical to cartesian
////////////////////////////////////////////////////////////////////////////////
Vector3 Furiosity::SphericalToCartesian(float r, float theta, float fi)
{
    return Vector3(r * sinf(theta) * cosf(fi),
                   r * cosf(theta),
                   r * sinf(theta) * sinf(fi));
}

/// Calculate Bernstein basis polynomials.
///
/// @param d Number of control points minus one.
/// @param d Current control point. Works when n <= d
/// @param d Time interval
/// @return float indicating the bernstein basis value
float Furiosity::BernsteinBasis(const int d, const int n, float x) {
    
    // Binomial coefficient:
    const int b = Factorial(d) / (Factorial(d - n) * Factorial(n));

    // Bernstein polynomial:
    return b * powf(x, n) * powf(1 - x, d - n);
}

Vector2 Furiosity::BezierCurve(const std::vector<Vector2>& points, const float& delta)
{
    if(points.empty())
    {
        ERROR("Cannot calculate a bezier curve point without any control points.");
        
        // Release build? Well, goodluck with that.
        return Vector2(0, 0);
    }
    
    
    Vector2 result;
    
    int n = (int)points.size() - 1;
    for(int i = 0; i <= n; ++i) {
        // TODO: We can cache the BernsteinBasis result.
        result += points[i] * BernsteinBasis(n, i, delta);
    }
    
    return result;
    
    
    // Parsing, triangulating and uploading an SVG souce took: 0.075797 seconds.
    
    /*
    // De Casteljau's algorithm (slightly slower)
    std::vector<Vector2> out(points);  // Intentional copy.
    std::vector<Vector2> tmp;
    
    // Reduce collection until one item remains.
    while(out.size() > 1)
    {
        for(size_t i = 0; i < out.size() - 1; ++i)
        {
            const Vector2& a = out[i];
            const Vector2& b = out[i + 1];
        
            tmp.push_back(Lerp(a, b, delta));
        }
        
        // Carry on reducing on the newly created collection.
        std::swap(tmp, out);
        
        tmp.clear();
    }
    
    return out.front();*/
}

////////////////////////////////////////////////////////////////////////////////
// Line to line intersection
////////////////////////////////////////////////////////////////////////////////
Vector2 Furiosity::LineIntersection(
        const Vector2& p1, const Vector2& d1,
        const Vector2& p2, const Vector2& d2)
{
    // Calculate slope "a" in "y = a * x + b":
    const float a_a = (d1.y - p1.y) / (d1.x - p1.x);
    const float b_a = (d2.y - p2.y) / (d2.x - p2.x);

    // Using slope "a", calculate "b". This is eventually
    // the constant in "c = m * x + n * y":
    const float a_c = a_a * p1.x - p1.y;
    const float b_c = b_a * p2.x - p2.y;

    // Straight line early out:
    if(d1.x == p1.x) {
        // Perpendicular OR not
        const float y = (b_a == 0) ? d2.y : b_a * d1.x - b_c;
    
        return Vector2(d1.x, y);
    }

    // Straight line early out:
    if(d2.x == p2.x) {
        // Perpendicular OR not
        const float y = (a_a == 0) ? d1.y : a_a * d2.x - a_c;

        return Vector2(d2.x, y);
    }

    // Elimination method applied:
    const float x = (a_c - b_c) / (a_a - b_a);
    
    // Feed "x" back into "c = mx + my" to find "y":
    const float y = a_a * x - a_c;
    
    // Et voila.
    return Vector2(x, y);
}


////////////////////////////////////////////////////////////////////////////////
// Great Circle Distance
////////////////////////////////////////////////////////////////////////////////
float Furiosity::GreatCircleDistance(const Furiosity::Vector3 &a,
                                     const Furiosity::Vector3 &b)
{
    Vector3 x = a;
    Vector3 y = b;
    //
    x.Normalize();
    y.Normalize();
    //
    return acosf(x.Dot(y));
}


// end
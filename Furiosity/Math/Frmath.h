////////////////////////////////////////////////////////////////////////////////
//  Frmath.h
//
//  Created by Bojan Endrovski on 23/09/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <math.h>
#include <stdlib.h>
#include <vector>

// Just an entry for all users of the math things
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix44.h"
#include "Matrix33.h"
#include "logging.h"

namespace Furiosity
{    
    /// A simple ray in 3D
    struct Ray3D
    {
        /// Ray's origin
        Vector3 Origin;
        
        /// Ray's direction
        Vector3 Direction;
    };

    /// Pi, like the film
    const float Pi			= 3.1415926535897932384626433832795f;
    const float TwoPi		= Pi * 2;
    const float HalfPi		= Pi / 2;
    const float QuarterPi	= Pi / 4;

    inline float DegToRad(float deg) { return deg * Pi / 180.0f; }

    /// Returns a random float between zero and 1
    inline float RandFloat()
    {
        return ((rand())/(RAND_MAX+1.0));
    }

    /// Returns a random float between x and y
    inline float RandInRange(float x, float y)
    {
        return x + RandFloat()*(y-x);
    }

    /// Normally-ish distributed float in range (-0.5, 0.5) with a EX=0
    inline float RandNormal()
    {
        return (RandFloat() - RandFloat()) * RandFloat();
    }

    /// Returns a random int between from and to
    inline int RandInRange(int from, int to)
    {
        int diff = to - from;
        return from + rand() % diff;
    }
    
    inline Vector2 RandomOnUnitDisk()
    {
        float R = RandFloat();
        float T = RandInRange(0.0f, TwoPi);
        return Vector2( R * cosf(T), R * sinf(T) );
    }
    
    inline Vector2 RandomOnUnitCircle()
    {
        float T = RandInRange(0.0f, TwoPi);
        return Vector2(cosf(T), sinf(T) );
    }


    /// Return min a
    inline float min(float x, float y)
    {
        if(x > y)
            return y;
        else
            return x;
    }

    /// Return absolute value of a number
    inline float Absf(float f)
    {
        return  (f < 0.0f) ? -f : f;
    }
    
    template <class T>
    T Lerp(T a, T b, float t)
    {
        if(t < 0)
            t = 0;
        else if( t > 1)
            t = 1;
        //
        return a * (1.0f - t) + b * t;
    }

    
    /// Does hermite spline interpolation of a real value
    template <class T>
    T HermiteInterpolation(T from, T to, float t, float m0, float m1)
    {
        // Put t in [0,1]
        if(t < 0)
            t = 0;
        else if( t > 1)
            t = 1;

        // Interpolate
        float t2 = t * t;		// t squared
        float t3 = t2 * t;		// t cubbed
        //
        T val   = (2*t3 - 3*t2 + 1) * from; // h00 * p0
        val     += (t3 - 2*t2 + t)   * m0;	// h10 * m0
        val     += (-2*t3 + 3*t2)    * to;	// h01 * p1
        val     += (t3 - t2)         * m1;  // h11 * m1
        //
        return val;
    }

    enum InterpolationType
    {
        INTERPOLATION_LERP,
        INTERPOLATION_EASE_IN,
        INTERPOLATION_EASE_OUT,
        INTERPOLATION_EASE_IN_OUT,
        INTERPOLATION_EASE_MID,
        INTERPOLATION_SPRING
    };
    
    
    template <class T>
    T Interpolate(T a, T b, float t, InterpolationType interpolation)
    {
        // Put t in [0,1]
        if(t < 0)
            t = 0;
        else if( t > 1)
            t = 1;
        
        // Change the interpolation
        switch(interpolation)
        {
            case INTERPOLATION_SPRING:
            case INTERPOLATION_LERP:
                break;
                
            case INTERPOLATION_EASE_IN:
                t = HermiteInterpolation(0.0f, 1.0f, t, 0.0f, 1.0f);
                break;
                
            case INTERPOLATION_EASE_OUT:
                t = HermiteInterpolation(0.0f, 1.0f, t, 1.0f, 0.0f);
                break;
                
            case INTERPOLATION_EASE_IN_OUT:
                t = HermiteInterpolation(0.0f, 1.0f, t, 0.0f, 0.0f);
                break;
                
            case INTERPOLATION_EASE_MID:
                t = HermiteInterpolation(0.0f, 1.0f, t, 1.0f, 1.0f);
                break;
        }
        
        // Return a lerp with the
        return Lerp(a, b, t);
    }
    

    /// Clamps a value in a certain interval
    ///
    /// @param val Value that will be clamped
    /// @param from Low bound
    /// @param to High bound
    template <class T>
    T Clamp(T val, T from, T to)
    {
        T t = val;
        if(t < from)
            t = from;
        else if( t > to)
            t = to;
        
        return t;
    }
    
    void PolarToCartesian(float r, float theta,
                          float& x, float& y);
    
    
    
    void CartesianToSpherical(float& r, float& theta, float& fi,
                              float x, float y, float z);
    
    /*
    int Nmod(int x, int m)
    {
        return (x % m + m) % m;
    }
     */
    
    
    /// Converts from spherical coordinates to cartesian
    ///
    /// @param r Radius
    /// @param theta Inclination
    /// @param azimuth
    void SphericalToCartesian(float& x, float& y, float& z,
                              float r, float theta, float fi);

    /// Converts from spherical coordinates to cartesian
    ///
    /// @param r Radius
    /// @param theta Inclination
    /// @param azimuth
    Vector3 SphericalToCartesian(float r, float theta, float fi);

    
    /// Calculate integer Factorials. Uses a for-loop internally.
    ///
    /// @param val Integer to calculate the factorial of
    /// @return The factorial.
    inline int Factorial(int val);
    
    /// Calculate Bernstein basis polynomials.
    ///
    /// @param d Number of control points minus one.
    /// @param d Current control point. Works when n <= d
    /// @param d Time interval
    /// @return float indicating the bernstein basis value
    inline float BernsteinBasis(const int d, const int n, float x);
    
    /// Calculate a point on a bézier curve. Works for any number of control
    /// points. Internaly uses Bernstein basis polynomals.
    ///
    /// TODO: contemplate a generic version that does not require std::vector.
    /// perhaps iterators or clever template usage? Resort to C style arrays?
    ///
    ///
    /// @param points A collection indicating the control points.
    /// @param delta Interval at which to calculate the point, ranges from 0 to 1.
    /// @return a point on a bézier curve.
    Vector2 BezierCurve(const std::vector<Vector2>& points, const float& delta);
    
    /// Calculate the point of intersection of two lines. Each line is
    /// represented by a two points on said line. This does not work
    /// as expected for parallel lines.
    ///
    /// @todo special case for parallel lines.
    ///
    /// @param p1 The first point on line 1.
    /// @param d1 The second point on line 1.
    /// @param p2 The first point on line 2.
    /// @param d2 The second point on line 2.
    ///
    /// @returns The point of intersection.
    Vector2 LineIntersection(
        const Vector2& p1, const Vector2& d1,
        const Vector2& p2, const Vector2& d2);

    /// Calculates great circle distance.
    ///
    /// @param a The begin point. Must be a normal.
    /// @param b The end point. Must be a normal.
    /// @return The great circle distance between the points on the sphere
    float GreatCircleDistance(const Vector3& a, const Vector3& b);
}




// end

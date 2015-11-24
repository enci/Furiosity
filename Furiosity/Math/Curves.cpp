////////////////////////////////////////////////////////////////////////////////
//  Curves.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 14/08/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Curves.h"
#include "Frmath.h"


using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Contruct an emtpy curve
////////////////////////////////////////////////////////////////////////////////
CardinalCurve::CardinalCurve()
{
    tension = 0.0f;
    closed = false;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a new polynimal for a segment from four control vertices
////////////////////////////////////////////////////////////////////////////////
CubicPolynomial CardinalCurve::CalculatePolynomial(float p0, float p1, float p2, float p3) const
{
    float p1Der = 0.5f * (1 - tension) * (p2 - p0);
    float p2Der = 0.5f * (1 - tension) * (p3 - p1);
    //
    float a = 2 * p1 + p1Der - 2 * p2 + p2Der;
    float b = -3 * p1 - 2 * p1Der + 3 * p2 - p2Der;
    float c = p1Der;
    float d = p1;
    //
    return CubicPolynomial(a, b, c, d);
}

void CardinalCurve::Rebuild()
{
    if(closed)
        RebuildClosed();
    else
        RebuildOpen();
}

        // Rebuilds the curve
void CardinalCurve::RebuildOpen()
{
    // Minimum for creating a segment
    if (CV.size() < 2)
        return;
    
    Px.clear();
    Py.clear();
    
    int start = 0; //CV.Count - 2;// Px.Count;
    auto end = CV.size() - 1;
    
    // Got through all the CVs
    for (int i = start; i < end; i++)
    {
        //int i = CV.Count - 2;
        int prev = (i - 1) < 0 ? 0 : (i - 1);
        int next = i + 1;
        int last = (i + 2) > (CV.size() - 1) ? (i + 1) : (i + 2);
        
        float p0 = CV[prev].x;
        float p1 = CV[i].x;
        float p2 = CV[next].x;
        float p3 = CV[last].x;
        //
        Px.push_back(CalculatePolynomial(p0, p1, p2, p3));
        
        p0 = CV[prev].y;
        p1 = CV[i].y;
        p2 = CV[next].y;
        p3 = CV[last].y;
        //
        Py.push_back(CalculatePolynomial(p0, p1, p2, p3));
    }
}

void CardinalCurve::RebuildClosed()
{

    // Minimum for creating a segment
    if (CV.size() < 2)
        return;
    
    Px.clear();
    Py.clear();
    
    int start = 0;
    int end = CV.size();
    
    // Got through all the CVs
    for (int i = start; i < end; i++)
    {
        int curr = i;
        int prev = Modulo(i - 1, end);
        int next = Modulo(i + 1, end);
        int last = Modulo(i + 2, end);
        //
        Vector2 p0 = CV[prev];
        Vector2 p1 = CV[curr];
        Vector2 p2 = CV[next];
        Vector2 p3 = CV[last];
        //
        Px.push_back(CalculatePolynomial(p0.x, p1.x, p2.x, p3.x));
        Py.push_back(CalculatePolynomial(p0.y, p1.y, p2.y, p3.y));
    }
}

// Calculates the samples and returns them for drawing
Vector2 CardinalCurve::Value(float t) const
{
    auto count = Px.size();
    
    if(count == 0)
        return Vector2();
    if(closed)
    {
        if (t < 0.0f) t = 1.0f + t;
        if (t >= 1.0f) t = t - 1.0f;
    }
    else
    {
        if (t <= 0.0f) t = 0.0f;
        if (t >= 1.0f) t = 0.99999f;
    }
    
    int poly = (int)floorf(t * count);
    t = t - ((float)poly / count);
    t *= count;
    
    float x = Px[poly].Value(t);
    float y = Py[poly].Value(t);
    
    return Vector2(x, y);
}

// Calculates the samples and returns them for drawing
Vector2 CardinalCurve::Derivative(float t) const
{
    /*
    int count = Px.Count;
    
    if (count == 0)
        return new Vector2();
    
    if (t <= 0.0f) t = 0.0f;
    if (t >= 1.0f) t = 0.999f;
    
    int poly = Mathf.FloorToInt(t * count);
    t = t - ((float)poly / count);
    t *= count;
    
    float x = Px[poly].FirstDerivate(t);
    float y = Py[poly].FirstDerivate(t);
    
    return new Vector2(x, y);
     */
    
    return Vector2();
}

float CardinalCurve::AproximateLength() const
{
    /*
    float l = 0;
    for (int i = 0; i < CV.Count - 1; i++)
    {
        
        Vector2 prev = CV[i];
        Vector2 next = CV[i+1];
        l += Vector2.Distance(prev, next);
    }
    return l;            
    */
    
    return 0.0f;
}


////////////////////////////////////////////////////////////////////////////////
// BSpline
////////////////////////////////////////////////////////////////////////////////
BSpline::BSpline()
    : closed(false)
    , count(0)
{
}

void BSpline::Rebuild()
{
    count = CV.size();
    Px.clear();
    Py.clear();
    
    if(closed)
    {
        // Create closed curve
        knots.resize(count + 3);
        // Px.resize(count);
        // Py.resize(count);
        //Pz.resize(count);
        for(int i = 0; i < count; i++)
            knots[i] = i;
        knots[count] = 0;
        knots[count + 1] = 1;
        knots[count + 2] = 2;
    }
    else
    {
        // Create a clamped curve
        knots.resize(count + 6);
        // Px.resize(count + 3);
        // Py.resize(count + 3);
        // Pz.resize(count + 3);;
        knots[0] = 0;
        knots[1] = 0;
        knots[2] = 0;
        for(int i = 3; i < count + 3; i++)
            knots[i] = i - 3;
        knots[count + 3] = count - 1;
        knots[count + 4] = count - 1;
        knots[count + 5] = count - 1;
    }
    
    CalculatePolys();
}

void BSpline::CalculatePolys()
{
    for(int i = 0; i < knots.size() - 3; i++)
    {
        Px.push_back(CubicPolynomial(
            (-CV[knots[i]].x + 3.0f * CV[knots[i + 1]].x - 3.0f * CV[knots[i + 2]].x + CV[knots[i + 3]].x) / 6.0f,
            (3.0f * CV[knots[i]].x - 6.0f * CV[knots[i + 1]].x + 3.0f * CV[knots[i + 2]].x) / 6.0f,
            (-3.0f * CV[knots[i]].x + 3.0f * CV[knots[i + 2]].x) / 6.0f,
            (CV[knots[i]].x + 4.0f * CV[knots[i + 1]].x + CV[knots[i + 2]].x) / 6.0f));
        
        Py.push_back(CubicPolynomial(
            (-CV[knots[i]].y + 3.0f * CV[knots[i + 1]].y - 3.0f * CV[knots[i + 2]].y + CV[knots[i + 3]].y) / 6.0f,
            (3.0f * CV[knots[i]].y - 6.0f * CV[knots[i + 1]].y + 3.0f * CV[knots[i + 2]].y) / 6.0f,
            (-3.0f * CV[knots[i]].y + 3.0f * CV[knots[i + 2]].y) / 6.0f,
            (CV[knots[i]].y + 4.0f * CV[knots[i + 1]].y + CV[knots[i + 2]].y) / 6.0f));
        
        /*
        Pz[i] = new CubicPolynomial(
                                    (-CV[knots[i]].Z + 3.0f * CV[knots[i + 1]].Z - 3.0f * CV[knots[i + 2]].Z + CV[knots[i + 3]].Z) / 6.0f,
                                    (3.0f * CV[knots[i]].Z - 6.0f * CV[knots[i + 1]].Z + 3.0f * CV[knots[i + 2]].Z) / 6.0f,
                                    (-3.0f * CV[knots[i]].Z + 3.0f * CV[knots[i + 2]].Z) / 6.0f,
                                    (CV[knots[i]].Z + 4.0f * CV[knots[i + 1]].Z + CV[knots[i + 2]].Z) / 6.0f);
         */
    }
}

Vector2 BSpline::Value(float t) const
{
    auto count = Px.size();
    
    if(count == 0)
        return Vector2();
    if(closed)
    {
        if (t < 0.0f) t = 1.0f + t;
        if (t >= 1.0f) t = t - 1.0f;
    }
    else
    {
        if (t <= 0.0f) t = 0.0f;
        if (t >= 1.0f) t = 0.99999f;
    }
    
    int poly = (int)floorf(t * count);
    t = t - ((float)poly / count);
    t *= count;
    
    float x = Px[poly].Value(t);
    float y = Py[poly].Value(t);
    
    return Vector2(x, y);
}




// end
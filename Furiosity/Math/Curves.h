////////////////////////////////////////////////////////////////////////////////
//  Curves.h
//  Furiosity
//
//  Created by Bojan Endrovski on 14/08/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#pragma once

// STL
#include <vector>

// Fr
#include "Vector2.h"
#include "CubicPolynomial.h"

namespace Furiosity
{
    enum SplineType { Normal, Clamped, Closed };
    
    class CardinalCurve
    {
    public:
        float tension;
            
        bool closed;
            
        // Control Vertices
        std::vector<Vector2> CV;
            
        // Polynomials for interpolation of the x coordinate
        std::vector<CubicPolynomial> Px;
            
        // Polynomials for interpolation of the y coordinate
        std::vector<CubicPolynomial> Py;
            
        // Contruct an emtpy curve
        CardinalCurve();
        
        // Creates a new polynimal for a segment from four control vertices
        CubicPolynomial CalculatePolynomial(float p0, float p1, float p2, float p3) const;
            
        void Rebuild();
            
        // Rebuilds the curve
        void RebuildOpen();
        
        void RebuildClosed();
        
        // Calculates the samples and returns them for drawing
        Vector2 Value(float t) const;
        
        // Calculates the samples and returns them for drawing
        Vector2 Derivative(float t) const;
        
        float AproximateLength() const;
    };
    
    class BSpline
    {
    public:
        bool closed;
        
        // Control Vertices
        std::vector<Vector2> CV;
        
        std::vector<int> knots;
        
        // Polynomials for interpolation of the x coordinate
        std::vector<CubicPolynomial> Px;
        
        // Polynomials for interpolation of the y coordinate
        std::vector<CubicPolynomial> Py;
        
        int count;
        
        // Contruct an emtpy curve
        BSpline();
        
        // Creates a new polynimal for a segment from four control vertices
        // CubicPolynomial CalculatePolynomial(float p0, float p1, float p2, float p3) const;
        
        void Rebuild();
        
        void CalculatePolys();
        
        // Rebuilds the curve
        // void RebuildOpen();
        
        // void RebuildClosed();
        
        // Calculates the samples and returns them for drawing
        Vector2 Value(float t) const;
        
    };
}
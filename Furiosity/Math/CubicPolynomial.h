////////////////////////////////////////////////////////////////////////////////
//  CubicPolynomial.h
//  Furiosity
//
//  Created by Bojan Endrovski on 14/08/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once


namespace Furiosity
{
    /// Most curves use cubic polynomials exclusively, and this is a simple
    // class implemeting one.
    class CubicPolynomial
    {
    protected:
        float a, b, c, d;
        
    public:
        
        /// Creates the a*x^3 + b*x^2 + c*x + d polynomial
        CubicPolynomial(float a, float b, float c, float d)
        {
            this->a = a;
            this->b = b;
            this->c = c;
            this->d = d;
        }
        
        /// Calculates the value using the horner rule
        /// @return The value ot the polynomial at x
        inline float Value(float x) const
        {
            float result = a;
            result = result * x + b;
            result = result * x + c;
            result = result * x + d;
            return result;
        }
        
        /// Calculates the value of the first derivate using the horner rule
        inline float FirstDerivate(float x) const
        {
            float result = 3.0f * a;
            result = result * x + 2.0f * b;
            result = result * x + c;
            return result;
        }
        
        /// Calculates the value of the second derivate using the horner rule
        inline float SecondDerivate(float x)
        {
            float result = 6.0f * a;
            result = result * x + 2.0f * b;           
            return result;
        }
    };
}
////////////////////////////////////////////////////////////////////////////////
//  Color.h
//
//  Created by Bojan Endrovski on 15/10/2010.
//  Copyright 2010 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <algorithm>

using namespace std;

typedef unsigned char uchar;

namespace Furiosity
{
    class Color
    {
    public:
        // The union trick in not standard C++ but should work with the any compiler
        // and makes usage of C style libs a lot easier. CHAR is always a byte	
        union
        {
            // Holds all the values
            char rgba[4];		
            
            struct
            {
                // Red
                unsigned char r;
                
                // Green
                unsigned char g;
                
                // Blue
                unsigned char b;
                
                // Alpha
                unsigned char a;	
            };
            
            // Holds all the values as a 32 bit unsigned int
            unsigned int integervalue;
        };
        
    public:
        // Ctor as black
        Color(): r(0), g(0), b(0), a(255) {}
        
        // Full ctor 
        Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
        : r(red), g(green), b(blue), a(alpha) {}
        
        // Opaque ctor 
        Color(unsigned char red, unsigned char green, unsigned char blue)
        : r(red), g(green), b(blue), a(255) {}
        
        /*
        Color(v)
        :   r( (uchar)(red * 255.0f) ),
        g( (uchar)(green * 255.0f) ),
        b( (uchar)(blue * 255.0f) ),
        a( (uchar)(alpha * 255.0f) ) {}
        */
        
        Color(string hex);
        
        
        /** Returns the value of the given vector added to this. */
        Color operator+(const Color& c) const
        {
            return Color(r+c.r, g+c.g, b+c.b, a+c.a);
        }
        
        /** Returns the value of the given vector added to this. */
        Color operator*(const Color& c) const;

        
        /** Returns the value of the given vector subtracted from this. */
        Color operator-(const Color& c) const
        {
            return Color(r-c.r, g-c.g, b-c.b, a-c.a);
        }
        
        /** Returns a copy of this color scaled the given value. */
        Color operator*(const float value) const
        {
            return Color(r*value, g*value, b*value, a*value);
        }
        
        /** Multiplies this vector by the given scalar. */
        void operator*=(const float value)
        {
            r = (uchar)min(r * value, 255.0f);
            g = (uchar)min(g * value, 255.0f);
            b = (uchar)min(b * value, 255.0f);
            a = (uchar)min(a * value, 255.0f);
        }
        
        /** Multiplies this vector by the given scalar. */
        void operator+=(const Color& c)
        {
            r += c.r;
            g += c.g;
            b += c.b;
            a += c.a;
        }
        
        /// Checks if two colors are the same
        bool operator==(const Color& c)
        {
            return r == c.r && g == c.g && b == c.b && a == c.a;
        }

        /// Checks if two colors are different
        bool operator!=(const Color& c)
        {
            return !(r == c.r && g == c.g && b == c.b && a == c.a);
        }
        
        /**
         * Calculates and returns the dot product of this vector
         * with the given vector.

        float operator *(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }
        */
        
        /*
        Color(int red, int green, int blue, int alpha)
        :   r( (uchar)red   ),
            g( (uchar)green ),
            b( (uchar)blue  ),
            a( (uchar)alpha ) {}

        
        Color(float red, float green, float blue, float alpha)
        :   r( (uchar)(red * 255.0f) ),
            g( (uchar)(green * 255.0f) ),
            b( (uchar)(blue * 255.0f) ),
            a( (uchar)(alpha * 255.0f) ) {}
         */
        
        static Color RandomColor();
        
        void LerpColors(Color first, Color second, float t);
        
        // Define some quick colors
        static Color Red;
        static Color Green;
        static Color Blue;
        static Color White;
        static Color Black;
        static Color Yellow;
        static Color Orange;
        static Color Purple;
        static Color Cyan;
        static Color Magenta;
        static Color Brown;
        static Color Grey;
        static Color CornFlower;
        
        static Color WhiteTransparent;
        static Color BlackTransparent;
        
        Color Transparent(float opacity = 0.0f) const;
    };
    
    /** Returns a copy of the color scaled the given value. */
    inline Color operator*(float value, Color c)
    {
        return Color(c.r*value, c.g*value, c.b*value, c.a*value);
    }
    
    

}
#endif

// end //

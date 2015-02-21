////////////////////////////////////////////////////////////////////////////////
//  Color.cpp
//
//  Created by Bojan Endrovski on 15/10/2010.
//  Copyright 2010 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Color.h"

#define bswap_32(x) ((((x) & 0xFF000000) >> 24) | \
                     (((x) & 0x00FF0000) >> 8)  | \
                     (((x) & 0x0000FF00) << 8)  | \
                     (((x) & 0x000000FF) << 24) )

// Framework
#include <sstream>
#include <cstdlib>

using namespace Furiosity;

Color Color::Red        = Color((uchar)255,    (uchar)0,     (uchar)0,       (uchar)255);
Color Color::Green      = Color((uchar)0,      (uchar)255,   (uchar)0,       (uchar)255);
Color Color::Blue       = Color((uchar)0,      (uchar)0,     (uchar)255,     (uchar)255);
Color Color::White      = Color((uchar)255,    (uchar)255,   (uchar)255,     (uchar)255);
Color Color::Black      = Color((uchar)0,      (uchar)0,     (uchar)0,       (uchar)255);
Color Color::Yellow     = Color((uchar)255,    (uchar)255,   (uchar)0,       (uchar)255);
Color Color::Orange     = Color((uchar)255,    (uchar)165,   (uchar)0,       (uchar)255);
Color Color::Purple     = Color((uchar)160,    (uchar)32,    (uchar)240,     (uchar)255);
Color Color::Cyan       = Color("00ffff");
Color Color::Magenta    = Color("ff00ff");
Color Color::Brown      = Color("7B4A12");
Color Color::Grey       = Color((uchar)128,    (uchar)128,   (uchar)128,     (uchar)255);
Color Color::CornFlower = Color((uchar)154,    (uchar)206,   (uchar)235,     (uchar)255);

Color Color::WhiteTransparent      = Color::White.Transparent();
Color Color::BlackTransparent      = Color::Black.Transparent();

Color::Color(string hexstring)
{
    if (hexstring.length() == 6)
        hexstring.append("ff");
    
    stringstream ss;
    unsigned int temp;
    ss << std::hex << hexstring;
    ss >> temp;
    integervalue = bswap_32(temp);
}

Color Color::operator*(const Color& c) const
{
    Color t(    (uchar)(( (uint)r * (uint)c.r) / 255),
                (uchar)(( (uint)g * (uint)c.g) / 255),
                (uchar)(( (uint)b * (uint)c.b) / 255),
                (uchar)(( (uint)a * (uint)c.a) / 255) );
    //
    return t;
}


void Color::LerpColors(Color first, Color second, float t)
{
    if (t > 1.0f) t = 1.0f;
    if (t < 0.0f) t = 0.0f;
    //
    r = (unsigned char)(first.r * (1 - t) + second.r * t);  // red
    g = (unsigned char)(first.g * (1 - t) + second.g * t);  // green
    b = (unsigned char)(first.b * (1 - t) + second.b * t);  // blue
    a = (unsigned char)(first.a * (1 - t) + second.a * t);  // alpha
}

Color Color::RandomColor()
{
    Color c;
    c.integervalue = rand();
    c.a = 255;
    return c;
}

Color Color::Transparent(float opacity) const
{
    return Color(this->r, this->g, this->b, (uchar)(opacity * 255.0f));
}




// end


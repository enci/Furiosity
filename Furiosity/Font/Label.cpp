////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include "Label.h"
#include "Vector2.h"
#include "ResourceManager.h"
#include "Font.h"
#include "utf8.h"

// #include <ft2build.h>
// #include FT_FREETYPE_H

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

using namespace Furiosity;
using namespace std;


inline uint NextPowerOfTwo(uint val)
{
    uint t = 2;
    while (t < val) t *= 2;
    return t;
}


Label::Label() : Texture()
{
    type = RESOURCE_TYPE_LABEL;
}

Label::Label(const string& originalText,
             const string& font,
             unsigned fontsize,
             float maxWidth)
{
    type = RESOURCE_TYPE_LABEL;
    this->originalText = originalText;
    this->font = font;
    this->fontsize = fontsize;
    this->maxWidth = maxWidth;

    Reload();
}

Label::~Label()
{
    // The superclass releases the glTexture.
}

// Static buffer for
const uint bufferWidth = 512;
const uint bufferHeight = 512;

void Label::Reload(bool cached)
{
    float downscale = gResourceManager.GetFontDownscale() * 0.5f;
    
    // A language may have its own font, these fonts are stored as
    // "translated" strings, too.
    std::string fontfile = gResourceManager.GetString( font );
    
    if(fontfile.empty())
        fontfile = font;
    
    Font* font = gResourceManager.LoadFont(fontfile);
    
    auto fontinfo = font->Fontinfo();
    
    // Aqcuire the translated string for this label:
    std::string text = gResourceManager.GetString( originalText );
    
    // No translation available, revert to default (english) text.
    if(text.empty())
        text = originalText;
    
    
    Vector2 outSize, outUV;
    //     bool multiline = false;
    
    // Check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
    auto end_it = utf8::find_invalid(text.begin(), text.end());
    if (end_it != text.end())
        LOG("Invalid UTF-8 encoding detected");
    
    vector<uint> utf32text;
    utf8::utf8to32(text.begin(), text.end(), back_inserter(utf32text));

    int ascent;
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, fontsize / downscale);   // get the scale for certain
    stbtt_GetFontVMetrics(&fontinfo, &ascent, 0, 0);                // get the the ascent
    int baseline = (int) (ascent*scale);                      // calculate the baseline in pixels
    
    
    unsigned char* buffer = (unsigned char*)calloc(bufferWidth * bufferHeight, sizeof(char));
    
    float xpos = 2;
    float ymax = -FLT_MAX;
    for(int ch = 0; ch < utf32text.size(); ++ch)
    {
        auto codepoint = utf32text[ch];
        int advance, lsb, x0, y0, x1, y1;
        float x_shift = xpos - (float)floor(xpos);
        
        stbtt_GetCodepointHMetrics(&fontinfo,                   // font
                                   (int)codepoint,              // current
                                   &advance,                    // advance
                                   &lsb);                       // left side bearing
        
        stbtt_GetCodepointBitmapBoxSubpixel(&fontinfo,          // font
                                            (int)codepoint,     // current character
                                            scale,              // scale x (not in pixels)
                                            scale,              // scale y (not in pixels)
                                            x_shift,            // subpixel shift x
                                            0,                  // subpixel shift y
                                            &x0,                // x min (from)
                                            &y0,                // y min (from)
                                            &x1,                // x max (to)
                                            &y1);               // y max (to)
        
        int idx = (baseline + y0) * bufferWidth + (int)xpos + x0;
        
        // This output can be saved into a bitmap and
        stbtt_MakeCodepointBitmapSubpixel(&fontinfo,                                // font
                                          &buffer[idx],  // pos in final image
                                          x1-x0,                                    // output_width
                                          y1-y0,                                    // output_height
                                          512,                                      // stride
                                          scale,                                    // scale x
                                          scale,                                    // scale y
                                          x_shift,                                  // subpixel shift x
                                          0,                                        // subpixel shift y
                                          (int)codepoint);                          // codepoint
        
        xpos += (advance * scale);
        if (ch < utf32text.size() - 1)
            xpos += scale * stbtt_GetCodepointKernAdvance(
               &fontinfo,
               (int)codepoint,
               (int)utf32text[ch + 1]);
        
        if((y1-y0) > ymax)
            ymax = (y1-y0);
    }
    
    ymax *= 1.5;
    
    float xmax = xpos;
    
    outSize.x = xmax * downscale;
    outSize.y = ymax * downscale;
    yoffset = 0;
    uint sizex = NextPowerOfTwo(xmax);
    uint sizey = NextPowerOfTwo(ymax);
    
//     uint sizex = 512;
//     uint sizey = 512;

    outUV.x = ((float)xmax) / sizex;
    outUV.y = ((float)ymax) / sizey;
    
    std::stringstream ss;
    // We're encapsulating strings in single quotes, and replacing existing
    // quotes with an escaped variant. This is OK-robust, but once iOS has
    // proper c++11 regex support, this should be replaced.
    ss << "label:'"  << Furiosity::StringReplace(text, "'", "\'")
    << "' font:'" << Furiosity::StringReplace(fontfile, "'", "\'")
    << "' size:'" << fontsize << "'";
    
    // Update the internal details:
    resourcePath = ss.str();
    resourceID = StringHash(resourcePath);
    
    // Set the texture internal properties:
    dimension           = outSize;
    uvTo                = outUV;
    width               = sizex;
    height              = sizey;
    genMipMap           = false;
    internalFormat      = GL_RGBA;
    hasAlpha            = true;
    size                = width * height * 4;
    GLubyte*  imageData = (GLubyte*) calloc(size, 1);
    
    if(name != 0)
    {
        glDeleteTextures(1, &name);
        name = 0;
    }
    
    glGenTextures(1, &name);               // Gen
    GL_GET_ERROR();
    glBindTexture(GL_TEXTURE_2D, name);    // Bind
    GL_GET_ERROR();
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Minmization
    GL_GET_ERROR();
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Magnification
    GL_GET_ERROR();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_GET_ERROR();
    
    
    
    // Draw a solid label background:
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned char c = buffer[i * bufferWidth + j];
         
            // Base index in buffer
            int baseIdx = (i * width + j) * 4;
            imageData[baseIdx + 0] = (GLubyte)255;
            imageData[baseIdx + 1] = (GLubyte)255;
            imageData[baseIdx + 2] = (GLubyte)255;
            imageData[baseIdx + 3] = (GLubyte)c;
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D,                 // What (target)
                 0,                             // Mip-map level
                 internalFormat,                // Internal format
                 width,                         // Width
                 height,                        // Height
                 0,                             // Border
                 internalFormat,                // Format (how to use)
                 GL_UNSIGNED_BYTE,              // Type   (how to intepret)
                 imageData);                    // Data
    
    GL_GET_ERROR();
    
    
    free(buffer);
    buffer = 0;
    
    free(imageData);
    imageData = 0;
    
    // To really benefit from caching, the font should
    // be retained through a ResourcePack.
    gResourceManager.ReleaseResource(font);
    
    Resource::Reload();
    
    /*
    // float scale = gResourceManager.GetFontDownscale();
    
    // TODO: This a very bad way to increase resolution on labels
    // Put device information to use
    float scale = gResourceManager.GetFontDownscale() * 0.5f;
    
    // A language may have its own font, these fonts are stored as
    // "translated" strings, too.
    std::string fontfile = gResourceManager.GetString( font );
    
    if(fontfile.empty())
        fontfile = font;
    
    Font* f = gResourceManager.LoadFont(fontfile);
    
    //     FT_Face face = f->GetFace();
    
    // Aqcuire the translated string for this label:
    std::string text = gResourceManager.GetString( originalText );
    
    // No translation available, revert to default (english) text.
    if(text.empty())
        text = originalText;
   

    Vector2 outSize, outUV;
    bool multiline = false;

    // Check for invalid utf-8 (for a simple yes/no check, there is also utf8::is_valid function)
    auto end_it = utf8::find_invalid(text.begin(), text.end());
    if (end_it != text.end())
        LOG("Invalid UTF-8 encoding detected");

    vector<uint> utf32text;
    utf8::utf8to32(text.begin(), text.end(), back_inserter(utf32text));

//    FT_Error error;
  
//    error = FT_Set_Pixel_Sizes(face,        // handle to face object
//                               0,           // pixel_width
//                               fontsize * (1.0f / scale));   // pixel_height
    
    ASSERT(!error);
    
    ///////////////////////////////////////////////////
    // Get the size of this label
    uint xmax = 0;
    uint ymax = 0;
    uint ymin = 0;
    uint x    = 0;
//    uint ascmax = 0;
    long dscmax = 0;
    //
    size_t from = 0;
    size_t to = utf32text.size();
    int di = 1;
    
    if( ! utf32text.empty()) {
        // Check if arabic
        const FT_ULong& firstCharCode = utf32text[0];
        if(firstCharCode >= 0x0600 && firstCharCode <= 0x06FF)
        {
            from = utf32text.size() - 1;
            to = -1;
            di = -1;
        }
    }
    
    //for(const FT_ULong& charcode : utf32text)
    for (size_t i = from; i != to; i += di)
    {
        const FT_ULong& charcode = utf32text[i];
        
        // TODO: Maybe check windows endings too
        if(charcode == 0x0000000a ||
           (charcode == 0x00000020 && x > (maxWidth / scale)))
        {
            ymin += face->size->metrics.height / 64;
            x = 0;
            multiline = true;
            continue;
        }
        
        // Load glyph image into the slot (erase previous one)                
        FT_UInt charindex = FT_Get_Char_Index(face, charcode);
        error = FT_Load_Glyph(face, charindex, FT_LOAD_RENDER);
        if ( error )
            LOG("The font could not load a character.");

        FT_GlyphSlot    slot    = face->glyph;  // a small shortcut
        FT_Bitmap&      bitmap  = slot->bitmap;

        
        long drop = (slot->metrics.height - slot->metrics.horiBearingY) / 64;
        //
        FT_Int  baseline = (FT_Int)((face->size->metrics.ascender - slot->metrics.horiBearingY) / 64);
        FT_Int  toy = ymin + baseline + bitmap.rows;
        //
        if(toy > ymax) ymax = toy;
        if(drop > dscmax) dscmax = drop;
        
        // FT_Int drop = face->size->metrics.height;

        // increment pen position
        x += slot->advance.x / 64;
        if(x > xmax) xmax = x;
    }
    //
    // Save the size to output size
    outSize.x = xmax * scale;
    outSize.y = ymax * scale;
    yoffset = dscmax * 0.5f;
    uint sizex = NextPowerOfTwo(xmax);
    uint sizey = NextPowerOfTwo(ymax);
    outUV.x = (float)xmax / sizex;
    outUV.y = (float)ymax / sizey;
    if(multiline) yoffset = 0.0f;
    //////////////////////////////////////////////////////

    //////////////////////////////////////////////////////

    std::stringstream ss;
    // We're encapsulating strings in single quotes, and replacing existing
    // quotes with an escaped variant. This is OK-robust, but once iOS has
    // proper c++11 regex support, this should be replaced.
    ss << "label:'"  << Furiosity::StringReplace(text, "'", "\'")
       << "' font:'" << Furiosity::StringReplace(fontfile, "'", "\'")
       << "' size:'" << fontsize << "'";

    // Update the internal details:
    resourcePath = ss.str();
    resourceID = StringHash(resourcePath);
    
    // Set the texture internal properties:
    dimension           = outSize;
    uvTo                = outUV;
    width               = sizex;
    height              = sizey;
    internalFormat      = GL_RGBA;
    hasAlpha            = true;
    size                = width * height * 4;
    GLubyte*  imageData = (GLubyte*) calloc(size, 1);

    glGenTextures(1, &name);               // Gen
    GL_GET_ERROR();
    glBindTexture(GL_TEXTURE_2D, name);    // Bind
    GL_GET_ERROR();
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Minmization
    GL_GET_ERROR();
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Magnification
    GL_GET_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_GET_ERROR();

    
    // Draw a solid label background:
    //for (int i = 0; i < tex->height; i++)
//    {
//        for (int j = 0; j < tex->width; j++)
//        {
//            tex->imageData[i * tex->width * 4 + j * 4 + 0] = (GLubyte)255;
//            tex->imageData[i * tex->width * 4 + j * 4 + 1] = (GLubyte)0;
//            tex->imageData[i * tex->width * 4 + j * 4 + 2] = (GLubyte)0;
//            tex->imageData[i * tex->width * 4 + j * 4 + 3] = (GLubyte)255;
//        }
//    }

    size_t pen_x, pen_y;
    pen_x = 0;
    pen_y = 0;

    for(size_t idx = from; idx != to; idx += di)
    {
        const FT_ULong& charcode = utf32text[idx];
        
        // TODO: Maybe check windows endings too
        if(charcode == 0x0000000a ||
           (charcode == 0x00000020 && pen_x > (maxWidth / scale)))
        {
            // increment pen position
            pen_x = 0;
            pen_y += face->size->metrics.height / 64;
            continue;
        }
        
        // load glyph image into the slot (erase previous one)
        FT_UInt charindex = FT_Get_Char_Index(face, charcode);
        error = FT_Load_Glyph(face, charindex, FT_LOAD_RENDER);

        // error = FT_Load_Char( face, charcode, FT_LOAD_RENDER );

        if ( error )
            LOG("The font could not load character code. ");

        FT_GlyphSlot    slot    = face->glyph;  // a small shortcut
        FT_Bitmap&      bitmap  = slot->bitmap;

        long  i, j, p, q;
        FT_Int  x_max = (FT_Int)pen_x + bitmap.width;
        FT_Int  y_min = (FT_Int)pen_y + (FT_Int)((face->size->metrics.ascender - slot->metrics.horiBearingY) / 64);
        FT_Int  y_max = y_min + bitmap.rows;
        //
        for (i = y_min, p = 0; i < y_max; i++, p++) // vertical
        {
            for (j = pen_x, q = 0; j < x_max; j++, q++)   // horizontal
            {
                unsigned char val = bitmap.buffer[p * bitmap.width + q];

                if (i < 0 || j < 0  || i >= height || j >= width)
                {
                    // NB: last time this happened (on android) was because the
                    // freetype static library didn't match the given headers.
                    LOG("Label::Reload(%s) FreeType tried to draw text outside of the texture boundaries.", resourcePath.c_str());
                    //assert(false);
                    continue;
                }

                // Fill the RGB bytes with a white color:
                for (int k = 0; k < 3; k++)
                    imageData[i * width * 4 + j * 4 + k] = (GLubyte)255;
                
                // Take the "whitest" pixel. We could do alpha blending here,
                // but I don't think that will look nice with fonts.
                imageData[i * width * 4 + j * 4 + 3] = std::max<GLubyte>(
                    imageData[i * width * 4 + j * 4 + 3],
                    val
                );
            }
        }

        // increment pen position
        pen_x += slot->advance.x / 64;
        pen_y += slot->advance.y / 64;
    }

    glTexImage2D(GL_TEXTURE_2D,                 // What (target)
                 0,                             // Mip-map level
                 internalFormat,                // Internal format
                 width,                         // Width
                 height,                        // Height
                 0,                             // Border
                 internalFormat,                // Format (how to use)
                 GL_UNSIGNED_BYTE,              // Type   (how to intepret)
                 imageData);                    // Data

    GL_GET_ERROR();
    
    free(imageData);
    imageData = 0;
    
    // To really benefit from caching, the font should
    // be retained through a ResourcePack.
    gResourceManager.ReleaseResource(f);
    
    Resource::Reload();
     */
}

bool Label::IsValid() {
    return Texture::IsValid();
}


void Label::SetText(const std::string& text)
{
    if(originalText.compare(text) != 0) {
        Invalidate();
        originalText = text;
        Reload();
    }
}

void Label::SetFont(const std::string& fontName)
{
    if(font.compare(fontName) != 0) {
        Invalidate();
        font = fontName;
        Reload();
    }
}


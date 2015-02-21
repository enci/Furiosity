////////////////////////////////////////////////////////////////////////////////
//  Texture.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 19/10/2011.
//  Copyright (c) 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Texture.h"

#include "png.h"
#include "Defines.h"
#include "ResourceManager.h"
#include "FileIO.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>


// Enable some undocumented Android features:
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
//
#define ATC_RGB_AMD                        0x8C92   //  8 bit blocksize
#define ATC_RGBA_EXPLICIT_ALPHA_AMD        0x8C93   // 16 bit blocksize
#define ATC_RGBA_INTERPOLATED_ALPHA_AMD    0x87EE   // 16 bit blocksize
//
#define PVR_TEXTURE_FLAG_TYPE_MASK         0xff

using namespace std;

using namespace Furiosity;

namespace Furiosity
{
enum
{
    PVRTextureFlagTypeRGBA_4444 = 16,
    PVRTextureFlagTypeRGBA_8888 = 18,
	PVRTextureFlagTypePVRTC_2 = 24,
	PVRTextureFlagTypePVRTC_4
};


typedef struct _PVRTexHeader
{
	uint32_t headerLength;
	uint32_t height;
	uint32_t width;
	uint32_t numMipmaps;
	uint32_t flags;
	uint32_t dataLength;
	uint32_t bpp;
	uint32_t bitmaskRed;
	uint32_t bitmaskGreen;
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;
	uint32_t pvrTag;
	uint32_t numSurfs;
} PVRTexHeader;

/// KtxFormat container as per Khronos spec.
struct KtxFormat {
    /// Constructor is only required when creating KTX images.
    KtxFormat() :
        endianness(0), glType(0), glTypeSize(0), glFormat(0), glInternalFormat(0),
        glBaseInternalFormat(0), pixelWidth(0), pixelHeight(0), pixelDepth(0),
        numberOfArrayElements(0), numberOfFaces(0), numberOfMipmapLevels(0), bytesOfKeyValueData(0) {

        identifier[0]  = 0xAB;
        identifier[1]  = 0x4B;
        identifier[2]  = 0x54;
        identifier[3]  = 0x58;
        identifier[4]  = 0x20;
        identifier[5]  = 0x31;
        identifier[6]  = 0x31;
        identifier[7]  = 0xBB;
        identifier[8]  = 0x0D;
        identifier[9]  = 0x0A;
        identifier[10] = 0x1A;
        identifier[11] = 0x0A;
    }

    unsigned char identifier[12];
    uint32_t endianness;
    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
};


}

////////////////////////////////////////////////////////////////////////////////
// Ctor
// Checks the extension of the resource and calls the apropriate loader
// Supported formatas are PVR: RGBA4444, RGBA_8888, PVRTC_2, PVRTC_4
// and now PNG, but not interlaced
////////////////////////////////////////////////////////////////////////////////
Texture::Texture(const std::string& _filename,  bool _genMipMap) :
    Resource(RESOURCE_TYPE_TEXTURE),
//    filename(_filename),
    name(0),
    width(0),
    height(0),
    internalFormat(0),
    hasAlpha(0),
    genMipMap(_genMipMap)
{
    resourcePath = _filename;
    Reload();
}

////////////////////////////////////////////////////////////////////////////////
// Dtor
////////////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{
    glDeleteTextures(1, &name);
    name = 0;
}

void Texture::Invalidate()
{
    glDeleteTextures(1, &name);
    name = 0;
}

bool Texture::IsValid()
{
    return glIsTexture(name);
}

void Texture::Reload(bool cached)
{
    // Get latest file
    string filename = gResourceManager.GetLatestPath(resourcePath);

    // Load
    if(filename.find(".pvr") != string::npos)
    {
        if(!LoadPVR(filename, genMipMap))
        {            
            // Try to open the PNG version of the PVR:
            FILE* pngHandle = gResourceManager.OpenFile(Furiosity::StringReplace(filename, ".pvr", ".png"));

            if(pngHandle == NULL) {
                // OK - it's really not there.
                ERROR("Texture::Load(%s, %d) - unable to load PVR image, it does not exist.)", filename.c_str(), (int)cached);
            
            } else {
                // The PNG exists, but it lacks the _c suffix. - thus was never compressed.
                ERROR("Texture::Load(%s, %d) - unable to load PVR image, the original image does not have the _c suffix, and is not automatically compressed. Try adding _c OR remove the [tex] wildcard from the source code.", filename.c_str(), (int)cached);
            }
        }
    } else if(filename.find(".png") != string::npos) {
        if(!LoadPNG(filename, genMipMap)) {
            ERROR("Texture::Load(%s, %d) - unable to load PNG image.", filename.c_str(), (int)cached);
        }
    } else if(filename.find(".dds") != string::npos) {
        if(!LoadDDS(filename, genMipMap)) {
            ERROR("Texture::Load(%s, %d) - unable to load DDS image.", filename.c_str(), (int)cached);
        }
    } else if(filename.find(".atc") != string::npos) {
        if(!LoadATC(filename, genMipMap)) {
            ERROR("Texture::Load(%s, %d) - unable to load ATC image.", filename.c_str(), (int)cached);
        }
    } else {
        ERROR("Texture::Load(%s, %d) - unable to load UNKNOWN image.", filename.c_str(), (int)cached);
    }
}


////////////////////////////////////////////////////////////////////////////////
// LoadPVR
////////////////////////////////////////////////////////////////////////////////
bool Texture::LoadPVR(const std::string& filename,  bool genMipMap)
{
    const std::string dataString = Furiosity::ReadFile(filename);
    
    if(dataString.empty()) {
        LOG("Unable to open file: %s\n", filename.c_str());
        return false;
    }
    
    //const int lenght = dataString.size();
    const char* data = dataString.c_str();

    // Get header
    PVRTexHeader* header = (PVRTexHeader*) data;
    // Read data form it
    this->width = header->width;
    this->height = header->height;
    
    // Get format
    uint32_t flags = header->flags & PVR_TEXTURE_FLAG_TYPE_MASK;
    if (flags != PVRTextureFlagTypePVRTC_4 && 
        flags != PVRTextureFlagTypePVRTC_2 &&
        flags != PVRTextureFlagTypeRGBA_8888 &&
        flags != PVRTextureFlagTypeRGBA_4444)
    {
        LOG("Unsupported format for texture: %s", filename.c_str());
        return false;
    }
    
    switch (flags)
    {
        case PVRTextureFlagTypePVRTC_4:
            internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
            break;
            
        case PVRTextureFlagTypePVRTC_2:
            internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            break;
            
        case PVRTextureFlagTypeRGBA_8888:
            internalFormat = GL_RGBA;
            break;
            
        case PVRTextureFlagTypeRGBA_4444:
            internalFormat = GL_RGBA;
            break;
            
        default:
            break;
    }
    
    // Set size, TODO: see datasize too
    size = header->dataLength;

    
    // Set data pointer
    GLubyte* imageData = (GLubyte*)data;
    imageData += sizeof(PVRTexHeader);
    
    glGenTextures(1, &name);                                            // Gen
    GL_GET_ERROR();
    
    glBindTexture(GL_TEXTURE_2D, name);                                 // Bin
    GL_GET_ERROR();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Minmization
    GL_GET_ERROR();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Magnification
    GL_GET_ERROR();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // TODO: This needs to move to texture instance
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL_GET_ERROR();
    
    //
    if(flags == PVRTextureFlagTypePVRTC_2 || flags == PVRTextureFlagTypePVRTC_4)
    {
        uint mipmap = 0;
        uint dataOffset = 0;
        //
        uint blockSize;
        uint widthBlocks;
        uint heightBlocks;
        uint bpp;
        uint mmWidth = width;
        uint mmHeight = height;

        while (dataOffset < header->dataLength && mmWidth >= 16 && mmHeight >= 16)
        {
            if(flags == PVRTextureFlagTypePVRTC_4)
            {
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = mmWidth / 4;
                heightBlocks = mmHeight / 4;
                bpp = 4;
            }
            else
            {
                assert(flags == PVRTextureFlagTypePVRTC_2);
                //
                blockSize = 8 * 4;          // Pixel by pixel block size for 2bpp
                widthBlocks = mmWidth / 8;
                heightBlocks = mmHeight / 4;
                bpp = 2;
            }
            
            uint dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
            //
            glCompressedTexImage2D(GL_TEXTURE_2D,           // What
                                   mipmap,                  // Mip-map level
                                   internalFormat,          // Format
                                   mmWidth,                 // Width
                                   mmHeight,                // Heigth
                                   0,                       // Border
                                   dataSize,                // Size in bytes
                                   imageData + dataOffset); // Actual data
            //
            dataOffset += dataSize;
            mmWidth /= 2;
            mmHeight /= 2;
            mipmap++;
            
            // Set to not load mip-maps bail out now
            if(!genMipMap)
                mmWidth = 0;
        }
    }
    else if (flags == PVRTextureFlagTypeRGBA_8888)
    {
        glTexImage2D(GL_TEXTURE_2D,                     // What (target)
                     0,                                 // Mip-map level
                     internalFormat,                    // Internal format
                     width,                             // Width
                     height,                            // Height
                     0,                                 // Border
                     internalFormat,                    // Format (how to use)
                     GL_UNSIGNED_BYTE,                  // Type   (how to intepret)
                     imageData);                        // Data
    }
    else if(flags == PVRTextureFlagTypeRGBA_4444)
    {        
        glTexImage2D(GL_TEXTURE_2D,                     // What (target)
                     0,                                 // Mip-map level
                     internalFormat,                    // Internal format
                     width,                             // Width
                     height,                            // Height
                     0,                                 // Border
                     internalFormat,                    // Format (how to use)
                     GL_UNSIGNED_SHORT_4_4_4_4,         // Type   (how to intepret)
                     imageData);
    }    
    
    GLenum err = glGetError(); _UNSUED(err);
    

#ifdef DEBUG
    if (err != GL_NO_ERROR)
    {
        LOG("Error uploading compressed texture %s. glError: 0x%04X\n", filename.c_str(), err);
        assert(0);
        return false;
    }
#endif
    
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// LoadPNG
////////////////////////////////////////////////////////////////////////////////
bool Texture::LoadPNG(const std::string& filename,  bool genMipMap)
{    
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    
    //std::string path = gResourceManager.GetPath(filename);
    FILE *fp = gResourceManager.OpenFile(filename);

    if(fp == NULL) {
    	ERROR("Texture::LoadPNG('%s') unable to open file.", filename.c_str());
    	return false;
    }
    
    // Create and initialize the png_struct
    // with the desired error handler
    // functions.  If you want to use the
    // default stderr and longjump method,
    // you can supply NULL for the last
    // three parameters.  We also supply the
    // the compiler header file version, so
    // that we know if the application
    // was compiled with a compatible version
    // of the library.  REQUIRED
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    
    if (png_ptr == NULL)
    {
        fclose(fp);
        return false;
    }
    
    // Allocate/initialize the memory  for image information.  REQUIRED
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL /* png_infopp_NULL */, NULL /* png_infopp_NULL */);
        assert(0);
        return false;
    }
    
    // Set error handling if you are using the setjmp/longjmp method
    // (this is the normal method of doing things with libpng).
    // REQUIRED unless you  set up  your own error handlers in
    // the png_create_read_struct() earlier.
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        // Free all of the memory associated with the png_ptr and info_ptr
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL /* png_infopp_NULL */ );
        fclose(fp);
        //
        // If we get here, we had a problem reading the file
        assert(0);
        return false; 
    }
    
    
    // Set up the output control if you are using standard C streams
    png_init_io(png_ptr, fp);
    
    // If we have already read some of the signature
    png_set_sig_bytes(png_ptr, sig_read);
    
    // If you have enough memory to read in the entire image at once, and
    // you need to specify only transforms that can be controlled
    // with one of the PNG_TRANSFORM_ * bits (this presently excludes
    // dithering, filling, setting background, and doing gamma
    // adjustment), then you can read the entire image (including pixels)
    // into the info structure with this call
    //
    // PNG_TRANSFORM_STRIP_16 |
    // PNG_TRANSFORM_PACKING  forces 8 bit
    // PNG_TRANSFORM_EXPAND forces to expand a palette into RGB
    png_read_png(png_ptr,
                 info_ptr,
                 PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
                 NULL /* png_voidp_NULL */);
    
    
    //    png_uint_32 pngwidth, pngheight;
    
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
    
    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGBA:
            hasAlpha = true;
            internalFormat = GL_RGBA; 
            break;
            
        case PNG_COLOR_TYPE_RGB:
            hasAlpha = false;
            internalFormat = GL_RGB;
            break;
        case PNG_COLOR_TYPE_GRAY:
            hasAlpha = false;
            internalFormat = GL_LUMINANCE;
            break;
            
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            hasAlpha = false;
            internalFormat = GL_LUMINANCE_ALPHA;
            break;
            
        default:
            //std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
            assert(0);
            return false;
    }
    
    GLubyte* data = 0;
    unsigned long int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    data = (unsigned char*) malloc(row_bytes * height);
    
    assert(data != 0);
    
    // Set size
    size = (int)row_bytes * height;
    if (genMipMap)
        size += size / 3;
    
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    
    for (int i = 0; i < height; i++)
    {
        // note that png is ordered top to bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        //memcpy(*data+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);
        memcpy(data+(row_bytes * i), row_pointers[i], row_bytes);
    }
    
    // Clean up after the read, and free any memory allocated
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL /* png_infopp_NULL */);
    
    // Close the file
    fclose(fp);
    
    GLubyte* imageData = data;
    glGenTextures(1, &name);                                            // Gen    
    GL_GET_ERROR();

    glBindTexture(GL_TEXTURE_2D, name);                                 // Bind
    GL_GET_ERROR();
    
    //
    if(genMipMap)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);    // Minmization
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                   // Minmization
    GL_GET_ERROR();
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                       // Magnification
    GL_GET_ERROR();
    
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // TODO: This needs to move to texture instance
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL_GET_ERROR();
    
    glTexImage2D(GL_TEXTURE_2D,                     // What (target)
                 0,                                 // Mip-map level
                 internalFormat,                    // Internal format
                 width,                             // Width
                 height,                            // Height
                 0,                                 // Border
                 internalFormat,                    // Format (how to use)
                 GL_UNSIGNED_BYTE,                  // Type   (how to intepret)
                 imageData);                        // Data
    GL_GET_ERROR();

    if(genMipMap) glGenerateMipmap(GL_TEXTURE_2D);
        
    GLenum err = glGetError(); _UNSUED(err);
    
#ifdef DEBUG
    if (err != GL_NO_ERROR)
    {
        ERROR("Error uploading PNG texture %s to GPU. glError: 0x%04X\n", filename.c_str(), err);
        return false;
    }
#endif
    
    free(data);

    return true;
}

bool Texture::LoadDDS(const std::string& filename, bool genMipMap)
{
#ifdef ANDROID

    // Source: http://svn.wildfiregames.com/public/ps/trunk/source/lib/tex/tex_dds.cpp
    struct DDS_PIXELFORMAT
    {
        uint32_t dwSize;                       // size of structure (32)
        uint32_t dwFlags;                      // indicates which fields are valid
        uint32_t dwFourCC;                     // (DDPF_FOURCC) FOURCC code, "DXTn"
        uint32_t dwRGBBitCount;                // (DDPF_RGB) bits per pixel
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;                   // (DDPF_ALPHAPIXELS)
    };

    struct DDS_HEADER
    {
        // (preceded by the FOURCC "DDS ")
        uint32_t dwSize;                    // size of structure (124)
        uint32_t dwFlags;                   // indicates which fields are valid
        uint32_t dwHeight;                  // (DDSD_HEIGHT) height of main image (pixels)
        uint32_t dwWidth;                   // (DDSD_WIDTH ) width  of main image (pixels)
        uint32_t dwPitchOrLinearSize;       // (DDSD_LINEARSIZE) size [bytes] of top level
                                            // (DDSD_PITCH) bytes per row (%4 = 0)
        uint32_t dwDepth;                   // (DDSD_DEPTH) vol. textures: vol. depth
        uint32_t dwMipMapCount;             // (DDSD_MIPMAPCOUNT) total # levels
        uint32_t dwReserved1[11];           // reserved
        DDS_PIXELFORMAT ddpf;               // (DDSD_PIXELFORMAT) surface description
        uint32_t dwCaps;                    // (DDSD_CAPS) misc. surface flags
        uint32_t dwCaps2;
        uint32_t dwCaps3;
        uint32_t dwCaps4;
        uint32_t dwReserved2;               // reserved
    };

    std::string data = Furiosity::ReadFile(filename);
    size = data.size();
    
    const std::string encodeType = "DDS ";
    if(data.length() < encodeType.length() || data.substr(0, 4).compare(encodeType) != 0) {
        LOG("Not a valid DDS file: '%s'. Header is broken.", filename.c_str());
        return false;
    }

    DDS_HEADER* header = (DDS_HEADER*) (data.c_str() + 4);

    const char encodingType[] = {
            (char) ((header->ddpf.dwFourCC >> 0) & 0xff),   // D
            (char) ((header->ddpf.dwFourCC >> 8) & 0xff),   // X
            (char) ((header->ddpf.dwFourCC >> 16) & 0xff),  // T
            (char) ((header->ddpf.dwFourCC >> 24) & 0xff),  // version (1, 3 or 5)
            '\0' // Let's pretend it's a c-style string.
    };

#ifdef DEBUG
    if(encodingType[0] != 'D' || encodingType[1] != 'X' || encodingType[2] != 'T') {
        LOG("Texture::loadDDS('%s') - WARNING, first 4 encoding bytes are unknown. Found: '%s', expected: 'DXTn'.", filename.c_str(), encodingType);
        return false;
    }

    if(header->dwSize != sizeof(DDS_HEADER)) {
        LOG("Texture::loadDDS('%s') - WARNING, byte size of the struct does not match the size of the header.", filename.c_str());
        return false;
    }
#endif

    int blockSize = 16;
    switch(encodingType[3]) {
    case '1':
            internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; // or: GL_COMPRESSED_RGB_S3TC_DXT1_EXT
            blockSize = 8; // No alpha, smaller blocksize.
            break;
    case '3':
            internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
    case '5':
            internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
    default:
        LOG("Texture::loadDDS('%s') - WARNING, unknown DDS type/version. Identifier: '%s'.", filename.c_str(), encodingType);
        return false;
    }

    //LOG("Width: %i px, Height: %i px, format: '%s', mipmaps: '%i', bitcount:'%i', usemipmap:%s.", header->dwWidth, header->dwHeight, encodingType, header->dwMipMapCount, header->ddpf.dwRGBBitCount, genMipMap ? "yes":"no");

#ifdef DEBUG
    if(header->dwWidth % 4 != 0 || header->dwHeight % 4 != 0) {
        LOG("Texture::loadDDS('%s') - WARNING, not to the power of 4. Unpredictable results.", filename.c_str());
    }

    if(genMipMap == true && header->dwMipMapCount <= 1) {
        LOG("Texture::loadDDS('%s') - WARNING, requested mipmap generation, but file does not contain any.", filename.c_str());
    }
#endif

    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);
    GL_GET_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Minmization
    GL_GET_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Magnification
    GL_GET_ERROR();

    int w = header->dwWidth;
    int h = header->dwHeight;

    int dataOffset = sizeof(DDS_HEADER) + 4;
    GLubyte* imageData = (GLubyte*) data.c_str();

    for(int i = 0; i < header->dwMipMapCount; ++i) {
        const int mipmap   = i;
        const int dataSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

        //LOG("Generating mipmap[%i/%i] %ipx * %ipx  with %i bytes, offset: %i.", i, header->dwMipMapCount, w, h, dataSize, dataOffset);

        glCompressedTexImage2D(GL_TEXTURE_2D,               // What
                                   mipmap,                  // Mip-map level
                                   internalFormat,          // Format
                                   w,                       // Width
                                   h,                       // Heigth
                                   0,                       // Border
                                   dataSize,                // Size in bytes
                                   imageData + dataOffset); // Actual data
        GL_GET_ERROR();

        if(!genMipMap) {
            break;
        }

        w >>= 1;
        h >>= 1;
        dataOffset += dataSize;
    }

    return true;

#endif //ifdef ANDROID

	return false;
}

bool Texture::LoadATC(const std::string& filename, bool genMipMap)
{
#ifdef ANDROID
    const std::string data = Furiosity::ReadFile(filename);
    size = data.length();
    if(data.empty()) {
        LOG("Texture::LoadATC('%s') unable to readfile, it's empty or doesn't exist?", filename.c_str());
        assert(0);
        return false;
    }
    
    const KtxFormat* ktx = (KtxFormat*) data.c_str();

#ifdef DEBUG
    KtxFormat test;

    if(strncmp((const char*) test.identifier, (const char*) ktx->identifier, 12) != 0) {
        LOG("Texture::LoadATC('%s') The file header does not match the expected sequence.", filename.c_str());

        for(int i = 0; i < 12; ++i) {
            LOG("Found: '%03d', expected: '%03d'", (char)*(ktx->identifier + i), (char)*(test.identifier + i));
        }

        assert(0);
        return false;
    }

#endif

    switch(ktx->glInternalFormat) {
        case GL_ATC_RGB_AMD:
            internalFormat = GL_ATC_RGB_AMD;
            break;
        case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
            internalFormat = GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
            break;
        case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
            internalFormat = GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
            break;
        default:
            LOG("Texture::LoadATC('%s') Unknown internal format. Supported are ATC_RGB_AMD, ATC_RGBA_EXPLICIT_ALPHA_AMD and ATC_RGBA_INTERPOLATED_ALPHA_AMD.", filename.c_str());
            assert(0);
            return false;
    }

    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);
    GL_GET_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Minmization
    GL_GET_ERROR();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Magnification
    GL_GET_ERROR();
    
    //LOG((const char*)glGetString(GL_EXTENSIONS));

    glCompressedTexImage2D(GL_TEXTURE_2D,            // What
                           0,                        // Mip-map level
                           internalFormat,           // Format
                           ktx->pixelWidth,          // Width
                           ktx->pixelHeight,         // Height
                           0,                        // Border
                           data.size() - sizeof(KtxFormat),              // Size in bytes
                           (GLubyte*) data.c_str() + sizeof(KtxFormat)); // Actual data
    GL_GET_ERROR();

    return true;
#endif
    
    return false;
}


// end

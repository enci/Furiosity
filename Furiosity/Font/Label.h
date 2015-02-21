#pragma once

#include <vector>
#include <string>
#include "Texture.h"
#include "logging.h"
#include "Vector2.h"
#include "Utils.h"
#include "gl.h"

namespace Furiosity
{
    class ResourceManager;
    class FontManager;
    
    using std::string;

    class Label : public Texture
    {
        string originalText;
        string font;
        unsigned fontsize;
        float maxWidth;
        
        Vector2 dimension;
        Vector2 uvTo;
        float   yoffset;
        
    protected:
        Label();
        Label(const string& originalText, const string& font, unsigned fontsize, float maxWidth);
        virtual ~Label();
        
    public:
        friend class ResourceManager;
        friend class FontManager;

        virtual void Reload(bool cached = false) override;
        virtual bool IsValid() override;

        const Vector2& GetSize() const      { return dimension;    }
        const Vector2& GetUvTo() const      { return uvTo;         }
        float YOffset() const               { return yoffset;      }
        const string& FontFile() const      { return font;         }
        const unsigned FontSize() const     { return fontsize;     }
        const string& OriginalText() const  { return originalText; }
        
        /// Adjust the text of this label. This fully regenerates the bitmap
        /// data using freetype, performance might not be too optimal.
        ///
        /// @param text A string indicating the new text.
        void SetText(const string& text);
        
        /// Adjust the text of this label. This fully regenerates the bitmap
        /// data using freetype, performance might not be too optimal.
        ///
        /// @param text A string indicating the new text.
        void SetFont(const string& fontName);
    };
}

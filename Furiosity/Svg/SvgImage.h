#pragma once

#include <vector>
#include <map>
#include <utility>

#include "Canvas.h"
#include "Frmath.h"
#include "Color.h"
#include "Resource.h"

#include "Utils.h"
#include "GeneralManager.h"

using tinyxml2::XMLNode;
using tinyxml2::XMLAttribute;
//using tinyxml2::XMLNode;
//using tinyxml2::XMLNode;

namespace Furiosity
{
    class ResourceManager;
    
    /// Private namespace. I'll use many common names and prefer not to collide
    /// with anything else out there.
    namespace Svg
    {
        struct Gradient
        {
            /// "name" of this gradient.
            std::string id;
            
            /// Linear gradient colors
            Color color[2];
            
            /// Direction represented as normalized offsets
            Vector2 angle[2];

            /// Default ctor, has values that are visible:
            Gradient() {
                angle[0].x = angle[0].y = 0;
                angle[1].x = angle[1].y = 1;
                color[0] = Color::Purple;
                color[1] = Color::CornFlower;
            }
        };
        
        /// A typical path command:
        struct Command
        {
            char c;
            std::vector<float> nums;
            
            Command() : c(0) {}
            explicit Command(char c) : c(c) {}
        };
        
        struct Path
        {
            /// "name" of this path.
            std::string id;
            
            std::vector<Command> commands;
            
            Gradient gradient;
            Gradient stroke;
            float strokeWidth; // 0 = no stroke.
            
            Path() : strokeWidth(0) {}
        };
    }

    class SvgImage : public Canvas, public Resource
    {
        typedef std::vector<Svg::Path> Paths;
        typedef std::map<const std::string, Svg::Gradient> Gradients;
        typedef std::vector<Svg::Command> Commands;
        
    
        /// Grand acess to protected constructor:
        friend class ResourceManager;
    
        /// Parse a SVG gradient from a XML node.
        Svg::Gradient ParseGradient(XMLNode* node) const;
        
        /// Parse a SVG path from a XML node.
        Svg::Path ParsePath(XMLNode* node) const;
        
        /// Parse anything sensible from a given node. Recursion.
        void ParseAnything(XMLNode* node);
        
        /// Parse commands from a SVG string with commands.
        Commands ParseCommands(const std::string& data) const;
        
        /// Parse commands from a SVG "point" attribute.
        Commands ParsePoints(const std::string& data) const;
        
        /// Parse a normalized float from an XML attribute. Some SVG exporters
        /// use percentages instead of [0..1] floats. This method accounts
        /// for the two types, and normalizes percentages, if found.
        ///
        /// @param attribute The XML attribute to parse from.
        /// @return A float indicating a normalized [0..1] range.
        ///
        float ParseNormalizedFloat(const XMLAttribute* attribute) const;
        
        /// Parse SVG image. This call, calls all others.
        void Parse(const std::string& source);
        
        /// Call Canvas methods based on internal state.
        void DoDrawCalls();
        
        /// Transform a given vector into furiosity coordinates.
        Vector2 ToFuriosityCoordinate(Vector2 v);
        
        /// As provided by the SVG image:
        float width;
        float height;
        
        /// Interal state:
        Paths paths;
        Gradients gradients;
    
    
        /// Default constructor, leaves the image unrenderable:
        SvgImage();
        
        /// Load raw svg xml source:
        void LoadFromSource(const std::string& source);
        
        /// Expand style="key:value;key:value" into XML attributes.
        void AttachStyleAttributes(XMLNode* node, const std::string& style) const;
        
    protected:
        /// Only the ResourceManager may access this constructor:
        SvgImage(const std::string& filename);
        
        /// TODO, perhaps this ctor makes sense:
        // SvgImage(const XmlResource* xml);
        
    public:
        /// For those who wish to inline their SVG source. Great for
        /// testing purposes.
        static SvgImage* CreateFromSource(const std::string& source) {
            /// Prepare a default empty image:
            SvgImage* image = new SvgImage();
            
            image->LoadFromSource(source);
            
            return image;
        }
    };
}


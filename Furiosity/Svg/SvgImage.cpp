#include <sstream>

#include "SvgImage.h"
#include "Xml.h"
#include "FileIO.h"
#include "ResourceManager.h"

using namespace Furiosity;

using namespace tinyxml2;

/// Global toggle for error reporting, the parser is quite verbose, which
/// is only usefull when debugging the parser itself.
#define SVGLOG(...) LOG(__VA_ARGS__)


SvgImage::SvgImage()
: Canvas()
, Resource(RESOURCE_TYPE_SVG)
, width(0)
, height(0)
{
    // Pretty much an undefined state.
}

SvgImage::SvgImage(const std::string& filename)
: Canvas()
, Resource(RESOURCE_TYPE_SVG)
, width(0)
, height(0)
{
    const std::string source = ReadFile(filename);
    
    if(source.empty()) {
        ERROR("Cannot create SVG image, file '%s' is empty or doesn't exist.", filename.c_str());
    }
    
    LoadFromSource(source);
}

void SvgImage::LoadFromSource(const std::string& source)
{
    // Parse all paths:
    Parse(source);
    
    // Issue draw calls for each path:
    DoDrawCalls();
    
    // Upload onto the GPU.
    Compile();
}

void SvgImage::Parse(const std::string& source)
{
    XMLDocument doc;
    doc.Parse(source.c_str());
    
    // The "<svg>" tag.
    XMLElement* root = doc.FirstChildElement();

    
    // Attributes attached to <svg>
    FOREACH_XML_ATTRIBUTE(attribute, root) {
        const string name(attribute->Name());
        
        if(name == "width") {
            width = attribute->FloatValue();
        } else if(name == "height") {
            height = attribute->FloatValue();
        } else {
            SVGLOG("Unknown root attribute '%s' with value '%s'.", attribute->Name(), attribute->Value());
        }
    }
    
    ParseAnything(root);
}

void SvgImage::ParseAnything(XMLNode* node)
{
    
    FOREACH_XML_ELEMENT(object, node) {
        const string name(object->Value());
                
        // Similar shapes:
        if(name == "path" || name == "polygon") {
            Svg::Path path = ParsePath(object);
            
            paths.push_back(path);
        
        } else if(name == "linearGradient") {
            Svg::Gradient g = ParseGradient(object);
            
            // Store for look-up.
            gradients[g.id] = g;
        
        // Top level elements, recurse deeper to find anything
        // of value:
        } else if(name == "defs" || name == "g") {
            ParseAnything(object);
            
        } else {
            SVGLOG("Unknown element: '%s'", name.c_str());
        }
    }
}

Vector2 SvgImage::ToFuriosityCoordinate(Vector2 v)
{
    const float w = gGeneralManager.GetDevice().GetScreenWidth();
    const float h = gGeneralManager.GetDevice().GetScreenHeight();

    const float hh = h * 0.5;
    const float hw = w * 0.5;
    
    // We shall draw from the center. Width and height are given by the
    // SVG image exporter. This alignment is on-par with PNG textures.
    v.x += (w - width) * 0.5;
    v.y += (h - height) * 0.5;
    
    const float x = -v.x + w;
    const float y = h - v.y;

    return Vector2((x - hw) * -1, y - hh);
}


void SvgImage::DoDrawCalls()
{
    // Global offset, paths inherit these from each other.
    Vector2 relative(0,0);
    Vector2 first; bool hasFirst = false;

    for(Svg::Path& path : paths) {
        Begin();
        
        char c = ' ';
        
        int commandIndex = 0;
        for(Svg::Command& command : path.commands) {
            c = command.c == ' ' ? c : command.c;
            
            std::stringstream ss;
            for(const float& num : command.nums) {
                ss << num << " ";
            }
            
            LOG("(%c) %s", c, ss.str().c_str());
            
            switch(c) {
                case 'm':
                    // "moveto" is the first command, it counts as absolute:
                    if(commandIndex == 0) {
                        relative.x = command.nums.at(0);
                        relative.y = command.nums.at(1);
                    
                    // "moveto" is not the first, it's relative:
                    } else {
                        // New relative coordinates:
                        relative.x += command.nums.at(0);
                        relative.y += command.nums.at(1);
                    }
                    
                    Vertex(ToFuriosityCoordinate(relative));
                    break;
                    
                // TODO: this works for all bezier nth orders.
                // TODO: absolute coordinates (just remove all relative
                // variables)
                case 'c': {
                    std::vector<Vector2> points;
                    
                    // Current point becomes start:
                    points.push_back(relative);
                    
                    for(size_t i = 0; i < command.nums.size(); i += 2) {
                    
                        if(command.nums.size() < i + 1) {
                            ERROR("Incorrect amount of numbers for bezier curve. Found: %d, expected one more.", (int)command.nums.size());
                        }
                    
                        const float x = command.nums[i];
                        const float y = command.nums[i + 1];
                    
                        points.push_back(
                            Vector2(x, y) + relative
                        );
                        
                        //LOG("%f %f", x, y);
                    }
                                        
                    // The end point to become the relative position
                    // for the next drawing call.
                    relative = points.back();
                    
                    // All to furiosity coordinates. Could merge with
                    // above loop.
                    for(Vector2& v : points) {
                        v = ToFuriosityCoordinate(v);
                    }
                    
                    Bezier(points);
                    
                    if(!hasFirst) {
                        first = relative;
                        hasFirst = true;
                    }
                    
                    } break;
                case 'l': { // Relative line:
                    relative.x += command.nums.at(0);
                    relative.y += command.nums.at(1);
                    
                    if(!hasFirst) {
                        first = relative;
                        hasFirst = true;
                    }
                    
                    Vertex(ToFuriosityCoordinate(relative));
                    
                    } break;
                case 'L': // absolete line:
                    relative.x = command.nums.at(0);
                    relative.y = command.nums.at(1);
                    
                    Vertex(ToFuriosityCoordinate(relative));
                    break;
                case 'z': // close loop.
                    if(hasFirst) {
                        Vertex(ToFuriosityCoordinate(first));
                        hasFirst = false;
                    } else {
                        SVGLOG("Cannot close path, there is no first coordinate.");
                    }
                    
                    break;
                default:
                    SVGLOG("No drawing intructions available for command: '%c'", c);
                    break;
            }
            
            commandIndex++;
        }
        
        
        SVGLOG("Grad A: #%x Grad B: #%x", path.gradient.color[0].integervalue, path.gradient.color[1].integervalue);
        
        if(path.gradient.angle[0] != path.gradient.angle[1]) {
            
            // We don't support offsets, but we can certainly fill by-angle:
            Vector2 direction = path.gradient.angle[1] - path.gradient.angle[0];
            float angle = atan2f(direction.y, direction.x);
            
            //SVGLOG("Gradient angle: %f (x0:%f y0:%f) (x1:%f y1:%f)", angle,
            //    path.gradient.angle[0].x, path.gradient.angle[0].y,
            //    path.gradient.angle[1].x, path.gradient.angle[1].y );
            
            // Gradient fill:
            Fill(path.gradient.color[0], path.gradient.color[1], angle + HalfPi);
        } else {
            const float angle = atan2f(path.gradient.angle[0].y, path.gradient.angle[0].x);
            // Solid color fill:
            Fill(path.gradient.color[0], path.gradient.color[1], angle);
        }
        
        if(path.strokeWidth > 0) {
            Stroke(path.strokeWidth, path.stroke.color[0], path.stroke.color[1]);
        }
        
    }
}

Svg::Gradient SvgImage::ParseGradient(XMLNode* node) const {

    Svg::Gradient g;

    // Parse usefulness from attributes:
    FOREACH_XML_ATTRIBUTE(attribute, node) {
        const std::string name(attribute->Name());
        
        if(name == "id") {
            g.id = attribute->Value();
        } else if(name == "style") {
            AttachStyleAttributes(node, attribute->Value());
        } else if(name == "x1") {
            g.angle[0].x = ParseNormalizedFloat(attribute);
        } else if(name == "y1") {
            g.angle[0].y = ParseNormalizedFloat(attribute);
        } else if(name == "x2") {
            g.angle[1].x = ParseNormalizedFloat(attribute);
        } else if(name == "y2") {
            g.angle[1].y = ParseNormalizedFloat(attribute);
        } else if(name == "xlink:href") {
            // TODO: inherit only defined values.
            ERROR("xlink:href needs to be programmed.");
        } else {
            SVGLOG("Parsing gradient: unknown attribute '%s' with value: '%s'", attribute->Name(), attribute->Value());
        }
    }
    
    int index = 0;
    
    // The colors are stored in subnodes:
    FOREACH_XML_ELEMENT(subnode, node) {
        string stop(subnode->Value());
        
        if(stop == "stop") {
            if(index >= 2) {
                SVGLOG("Found more than two colors for a gradient.");
                break;
            }
            
            FOREACH_XML_ATTRIBUTE(attribute, subnode) {
                const std::string name(attribute->Name());
                
                if(name == "offset" ) {
                    // ?
                } else if(name == "style") {
                    AttachStyleAttributes(subnode, attribute->Value());
                    
                } else if(name == "stop-color") {
                    const string color = attribute->Value();
                    const float alpha = g.color[index].a;
                    
                    // Some editor out there probably doesn't use hex.
                    if(StringStartsWith(color, "#")) {
                        g.color[index] = Color(color.substr(1, color.length()));
                        
                        g.color[index].a = alpha;
                    } else {
                        SVGLOG("Found unparsable color: '%s'", color.c_str());
                    }
                    
                } else if(name == "stop-opacity") {
                    float alpha = ParseNormalizedFloat(attribute);
                    
                    // Alpha zero may also indicate a parse error.
                    if(alpha < 0 || alpha > 1) {
                        SVGLOG("Error parsing color, not in [0..1] range. Found: '%f', id: '%s'.", alpha, g.id.c_str());
                        
                        // Clamp to sensible range:
                        alpha = Clamp(alpha, 0.0f, 1.0f);
                    }
                    
                    // Scale alpha color back to byte range:
                    g.color[index].a = alpha * 255;
                    
                } else {
                    SVGLOG("Parsing gradient color: unknown attribute '%s' with value: '%s'", attribute->Name(), attribute->Value());
                }
            }
            
            ++index;
        } else {
            SVGLOG("Unknown gradient stop found '%s'", stop.c_str());
        }
    }

    SVGLOG("Parsed gradient: %x %x with angle x1:%f y1:%f x2:%f y2:%f ",
        g.color[0].integervalue, g.color[1].integervalue,
        g.angle[0].x, g.angle[0].y, g.angle[1].x, g.angle[1].y);

    return g;
}


Svg::Path SvgImage::ParsePath(XMLNode* node) const
{

    Svg::Path path;

    FOREACH_XML_ATTRIBUTE(attribute, node) {
        const string name(attribute->Name());
        
        if(name == "style") {
            AttachStyleAttributes(node, attribute->Value());
            
        } else if(name == "id") {
            path.id = attribute->Value();
            
        } else if(name == "fill" || name == "stroke") {
            const string fill = attribute->Value();
            
            // Fill, or stroke?
            Svg::Gradient& gradient = (name == "fill") ? path.gradient : path.stroke;
            
            // Solid hex RGB:
            if(StringStartsWith(fill, "#")) {
                
                // Both ends same color:
                gradient.color[0] = Color(fill.substr(1, fill.length()));
                gradient.color[1] = gradient.color[0];
                
                // Solid gradient (as per spec!)
                gradient.angle[0].x = gradient.angle[1].x = 0;
                gradient.angle[0].y = gradient.angle[1].y = 0;
                
            // Reference to an existing gradient:
            } else if(StringStartsWith(fill, "url(#") && StringEndsWith(fill, ")")) {
            
                const string id = fill.substr(5, fill.length() - 6);
                
                auto it = gradients.find(id);
    
                if(it != gradients.end()) {
                    gradient = it->second;
                    
                    SVGLOG("using color[0]: %i %i %i %i", (int) gradient.color[0].r, (int) gradient.color[0].g, (int) gradient.color[0].b, (int) gradient.color[0].a);
                    SVGLOG("using color[1]: %i %i %i %i", (int) gradient.color[1].r, (int) gradient.color[1].g, (int) gradient.color[1].b, (int) gradient.color[1].a);
                    
                    
                } else {
                    SVGLOG("Gradient for id '%s' not found. Using default.", id.c_str());
                    // Default is already present.
                    //path.gradient = Svg::Gradient();
                }
            } else {
                // Literal names such as: "red", "green" etc.
                SVGLOG("Unknown fill color '%s'", fill.c_str());
            }
        
        } else if(name == "d") {
            string data(attribute->Value());
        
            // Contains quite some logic, so it has its own method.
            path.commands = ParseCommands(data);
        
        } else if(name == "points") {
            string data(attribute->Value());
            
            path.commands = ParsePoints(data);
        
        } else if(name == "stroke-width") {
            path.strokeWidth = attribute->FloatValue();

        } else if(StringStartsWith(name, "stroke")) {
            SVGLOG("Parsing path: '%s' is not supported.", name.c_str());
            
        } else {
            SVGLOG("Parsing path: unknown attribute '%s' with value '%s'", name.c_str(), attribute->Value());
        }
    }
            
    // I trust in RVO (return value optimization).
    return path;
}

std::vector<Svg::Command> SvgImage::ParseCommands(const std::string& data) const
{
    // Each set of coords is prefixed by a letter:
    // lowercase = relative to previous
    // uppercase = absolute value
    // no letter = inherit previous letter.
    
    // The general filosophy, we parse AS-IS and shall not repair
    // errors, or normalize any data. This should all be done when
    // the drawing instructions are given later on.
    
    std::vector<Svg::Command> commands;
    
    for(size_t i = 0; i < data.length(); ++i) {
        const char c = data[i];
        
        switch(c) {
            case 'm': case 'M': // M = moveto (two move-to in a row means an actual line)
            case 'l': case 'L': // L = lineto
            case 'h': case 'H': // H = horizontal lineto
            case 'v': case 'V': // V = vertical lineto
            case 'c': case 'C': // C = curveto
            case 's': case 'S': // S = smooth curveto
            case 'q': case 'Q': // Q = quadratic Bezier curve
            case 't': case 'T': // T = quadratic Bezier curve (inherits one control point)
            case 'a': case 'A': // A = elliptical Arc
            case 'z': case 'Z': // Z = closepath
                commands.push_back(Svg::Command(c));
                break;
            
            case ' ':
                // This state has to meanings; it is used to separate
                // numbers, OR it means repeat previous command.
                //
                // Example:
                // "L40 40" - the space separates numbers.
                // "L40 40 40 40" - the space separates numbers AND signifies
                // a repeat last command, an L command can only have two
                // subsequent numbers. Here we parse as-is, the drawing method
                // will decipher the actual meaning of this space.
                break;
            
            case ',':
                // End of some sort of sequence. Usually seperates numbers.
                break;
                
            default: {
                
                long int start = i;
                
                // While the character is number-like and doesn't
                // exceed the length, skip ahead.
                // We can add "e/E" for scientific notation. The SVG spec
                // doesn't mention it though.
                while(i < data.length() && (
                    std::isdigit(data[i]) ||
                    data[i] == '-' ||
                    data[i] == '.' ||
                    data[i] == '+'
                    )) {
                    ++i;
                }
                
                
                if(start != i) {
                    // We've determined all offsets, let's parse that into a float:
                    const float num = atof(data.substr(start, i - start).c_str());
                    
                    if( ! commands.empty()) {
                        // Append number to the recent most command:
                        commands.back().nums.push_back(num);
                    } else {
                        SVGLOG("Found a number '%f' but there is no command.", num);
                    }
                    
                    // Important, step one character back. The other
                    // for-loop will increment, too.
                    --i;
                    
                } else {
                    // Might be a new command?
                    SVGLOG("Parsing path: no unknown character '%c'", c);
                }
               
                // End of default switch/case
                } break;
        } // End of switch/case
    } // End of for each character.

    return commands;
}

std::vector<Svg::Command> SvgImage::ParsePoints(const std::string& data) const
{
    std::vector<Svg::Command> commands;
    
    std::vector<std::string> points = StringExplode(data, " ");
    
    // TODO: this parsing is not robust enough.
    for(const std::string& point : points) {
        std::vector<std::string> points = StringExplode(point, ",");
        
        if(points.size() == 2) {

            Svg::Command command('L');
            command.nums.push_back(atof(points[0].c_str())); // x
            command.nums.push_back(atof(points[1].c_str())); // y
            
            commands.push_back(command);
            
        } else {
            // Too many points:
            SVGLOG("Error parsing points from '%s', this data is wrong: '%s'.", data.c_str(), point.c_str());
        }
    }
    
    return commands;
}

float SvgImage::ParseNormalizedFloat(const XMLAttribute* attribute) const
{
    const char* value = attribute->Value();
    
    if(value)
    {
        // The editor decided to go with percentages.
        if(StringEndsWith(value, "%"))
        {
            string copy(value);
            
            // Normalize percentages to [0..1]
            return 100 / atof(copy.substr(0, copy.length() - 1).c_str());
        }
        
        return atof(value);
    }
    
    // Error, the attribute value in the XML was probably empty.
    SVGLOG("Failed to parse float from attribute: '%s'", attribute->Name());
    return 0;
}


void SvgImage::AttachStyleAttributes(XMLNode* node, const std::string& style) const
{
    XMLElement* element = node->ToElement();

    if(element == 0)
    {
        ERROR("Cannot expand style as seperate attributes, the node is not an element.");
    }

    std::vector<std::string> attributes = StringExplode(style, ";");
    
    for(const std::string &attribute : attributes)
    {
        std::vector<std::string> pair = StringExplode(attribute, ":");
        
        if(pair.size() == 2)
        {
            element->SetAttribute(pair[0].c_str(), pair[1].c_str());
            
            SVGLOG("Attaching: %s -> %s", pair[0].c_str(), pair[1].c_str());
        }
        else
        {
            SVGLOG("Error in parsing attribute: %s", attribute.c_str());
        }
    }
}

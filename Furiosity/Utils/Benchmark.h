////////////////////////////////////////////////////////////////////////////////
// Timig stuff added by Geerjo
////////////////////////////////////////////////////////////////////////////////

#include "DebugDraw2D.h"
#include "Stopwatch.h"
#include <map>
#include <deque>
#include "ResourceManager.h"
#include "Label.h"

using std::map;
using std::string;

namespace Furiosity
{
    class Benchmark
    {
        typedef std::deque<double> Results;
    
        map<const string, Results> history;
        map<const string, double> start;
        map<const string, Label*> labels;
    
        float baseline;
        float hw;
        float hh;
        
        unsigned frame;
        
        float fontSize;
        
        Vector2 offset;
        
    public:
        Benchmark()
        : baseline(1.0f / 60.0f)
        , hw(gGeneralManager.GetDevice().GetScreenWidth() * 0.5f)
        , hh(gGeneralManager.GetDevice().GetScreenHeight() * 0.5f)
        , frame(0)
        , fontSize(20)
        , offset(10, -20)
        {
            // Retain the font.
            gResourceManager.LoadFont("/SharedResources/Fonts/Roboto-Regular-subset.ttf");
        }
        
        void Start(const string& key)
        {
            start[key] = GetTiming();
            
            if(labels.find(key) == labels.end()) {
                labels[key] = gResourceManager.LoadLabel(key, "/SharedResources/Fonts/Roboto-Regular-subset.ttf", fontSize);
            }
            
        }
        
        void End(const string& key)
        {
            float delta = GetTiming() - start[key];
            
            auto stats = history.find(key);
            
            // First time!
            if(stats == history.end())
                stats = history.insert(make_pair(key, Results())).first;
            
            (*stats).second.push_back(delta);
            
            // TODO: circular buffer.
            if((*stats).second.size() > 100) {
                (*stats).second.pop_front();
            }
        }
        
        void Render(SpriteRender& rend)
        {
            Matrix33 m(1);
            m.SetTranslation(Vector2(-hw, hh) + offset);
            
            for(auto pair : labels)
            {
                Label* label  = pair.second;
                const string& title = pair.first;
                
                auto it = history.find(title);
                
                if(it != history.end() && ! it->second.empty())
                {
                    
                        
                    // Update every 2^4 frames.
                    if(frame % 10 == 1) {
                        float value = 0;
                        for(const float& v : it->second)
                            value += v;
                
                        value /= it->second.size();
                    
                        char buff[50];
                        
                        int len = snprintf(buff, sizeof(buff), "| %03.0f%% | %04.3f | %s",
                            (100.0f / baseline * value),
                            value,
                            title.c_str()
                            //(int) (1.0f / value),
                        );
                        
                        label->SetText(string(buff, len));
                    }
                    
                    rend.DrawQuad(m,
                                label->GetSize().x, label->GetSize().y,
                                label,
                                Vector2(label->GetSize().x * 0.5f, label->GetSize().y * -0.5f),
                                Color::Red,
                                Vector2(0, 0),
                                label->GetUvTo()
                    );
                }
                
                m.m32 -= fontSize * (1.0f / gResourceManager.GetFontDownscale());
            }
        }
        
        void Update(float dt)
        {
            if(frame > 0)
            {
                End("Fps");
            }
            
            Start("Fps");
            
            frame++;
        }
    };
}
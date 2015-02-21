////////////////////////////////////////////////////////////////////////////////
//  GUIElement.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once


// Local
#include "Vector2.h"
#include "Color.h"
#include "SpriteRender.h"
#include "DebugDraw2D.h"
#include "Transformable.h"
#include "Animation.h"
#include "Countable.h"
#include "Colorable.h"
#include "Input.h"

namespace Furiosity
{
    enum class GUIElementState
    {
        ELEMENT_STATE_ENABLED,
        ELEMENT_STATE_DISABLED,
        ELEMENT_STATE_TRANSITIONING_IN,
        ELEMENT_STATE_TRANSITIONING_OUT,
        ELEMENT_STATE_COUNT,
    };
    
    /// Old fashion enums for bit combinations
    enum GUIAnchoring
    {
        ANCHORING_NONE          = 0x000000,
        ANCHORING_TOP           = 0x000001,
        ANCHORING_BOTTOM        = 0x000002,
        ANCHORING_LEFT          = 0x000004,
        ANCHORING_RIGHT         = 0x000008
    };
    
    // Forward declaration
    class GUIContainer;
    
    ///
    /// UIElement - A base class for all UI classes
    ///
    class GUIElement :  public Transformable,               // Position and orientation in 2D
                        public Animatable,                  // Animations to be played
                        public Countable<GUIElement>,       // Count simply how many of these are existing
                        public TouchHandler,                // Be able to respond to touches
                        public Colorable                    // For acces to color animations and such
    {
        // Needs this for parent tracking stuff
        friend class GUIContainer;
        
    protected:

        /// A final, calculated transform in world space.
        /// Registered at the center of its container.
        Matrix33            transform;
        
        /// Local transform
        Matrix33            local;
        
        /// Size, vector from the top left to
        /// the bottom right conrner
        Vector2             size;
        
        /// When enabled it will get updates and touch events
        GUIElementState     state;
        
        /// When visible it will get rendered
        bool                visible;
        
        /// If this is set to true it will capture input from others
        bool                capture;
        
        /// Parent when nesting element
        GUIElement*         parent;
        
        /// Offset in it's own frame
        Vector2             offset;
        
        /// Color to tint this element with
        //        Color               color;
        
        /// Final, calculated tint
        Color               tint;
        
        /// Anchoring flags
        uint                anchoring;
        
        /// Offset from anchor
        Vector2             anchoredPosition;
        
        /// An element can be named
        string              name;
        
        /// Apply the anchoring
        void                AnchorInParent();
        
    public:
        /// Create a new element with a postition and size
        GUIElement(Vector2 postition,
                   Vector2 size);
        
        // Dtor
        virtual ~GUIElement() {}
        
        // Virtual update call
        virtual void Update(float dt);
        
        // Handle a touch
        virtual void HandleTouch(Touch* touch) {}
        
        virtual void HandleTouch(Touch& touch) override {}
    
        virtual void DropTouches() override {}
        
        virtual void UpdateTransform();
    
        void UpdateColor();
        

        // Do nothing default render method
        virtual void Render(SpriteRender& rend) {}
        
        virtual void PlayAnimation(const std::string& name) override;
        
        const Matrix33& World()         { return transform; }

        const Matrix33& Local()         { return local; }
        
        // Size
        Vector2 Size() const            { return size; }

        /// Set the size of the element
        void SetSize(Vector2 size)      { this->size = size; }


        float Width() const             { return size.x; }

        float Height() const            { return size.y; }


        void SetWidth(float w)          { size.x = w; }

        void SetHeight(float h)         { size.y = h; }
    
        // Visibility
        bool Visible() const            { return visible; }
        void SetVisible(bool vis)       { visible = vis; }
        
        // virtual void SetEnabled(bool e) { enabled = e; }
        bool Enabled() const            { return state == GUIElementState::ELEMENT_STATE_ENABLED; }
        void Disable()                  { state = GUIElementState::ELEMENT_STATE_DISABLED; }
        void Enable()                   { state = GUIElementState::ELEMENT_STATE_ENABLED;  }
        
        // Animate coming on screen and leaving
        virtual void Show();
        virtual void Hide();
        // virtual bool InTransition();
        
        
        bool Capture() const      { return capture; }
        void SetCapture(bool top) { capture = top;  }
        
        // Parent
        GUIElement* GetParent() const { return parent; }
        bool        IsRoot()    const { return parent == 0; }
        
        // Render color TODO: Maybe rename this one
        Color       Tint() const        { return tint;  }
                
        Vector2     Offset() const              { return offset; }
        void        SetOffset(const Vector2& o) { offset = o; }
        
        // Calculates a position in local coordinates
        Vector2     LocalCoordinates(const Vector2& vec);
        
        // Checcks if a touch in region
        bool        InRegion(const Touch* touch, float tolerance = 1.0f);
        
        // Anchoring
        bool        TestAnchoring(GUIAnchoring a) { return a & anchoring; }
        void        SetAnchoring(uint a, Vector2 offsetFromAnchor);
        void        SetAnchoring(uint a);
        Vector2     AnchoredPosition(uint a, Vector2 offsetFromAnchor) const;
        //        void        SetAnchoring(GUIAnchoring a);
        
        /// Get the name of this entity
        const std::string&  Name() const        { return name; }
        void SetName(const std::string& name)   { this->name = name; }

#ifdef DEBUG
        virtual void DebugDraw(DebugDraw2D& debugDraw);
#endif
        
    };
}


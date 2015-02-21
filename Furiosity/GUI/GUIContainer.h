////////////////////////////////////////////////////////////////////////////////
//  GUIContainer.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// STL
#include <vector>

// Local
#include "GUIElement.h"

namespace Furiosity
{
    ///
    /// Keeps a collection if child elements and can draw them
    ///
    class GUIContainer : public GUIElement
    {
    protected:
        /// A collection of all the elements
        std::vector<GUIElement*> elements;
        
        ///
        std::vector<GUIElement*> deleteQueue;
        
        /// A mask to only draw inside the container
        static Texture*           mask;
        
        /// Masking flag
        bool                      masking;
        
    public:
        
        /// Create and empty container with a position and size
        GUIContainer(Vector2 position, Vector2 size);
        
        // Dtor - clean all the elements
        virtual ~GUIContainer();
        
        // Virtual update call
        virtual void Update(float dt) override;
        
        // Handle a touch
        virtual void HandleTouch(Touch* touch) override;
        
        // Do nothing default render method
        virtual void Render(SpriteRender& rend) override;
        
        //
        virtual void UpdateTransform() override;
        
        // Adding elements
        void AddElement(GUIElement* child, int layer = 0);
        
        void RemoveElement(GUIElement* child);
        
        void RemoveAndDeleteElement(GUIElement* child);
        
        void ClearContainer();
        
        bool ContainsElement(GUIElement* child);
        
        void SortElements();
        
        // Gets an element by index
        GUIElement*  GetElement(int i) { assert(i<elements.size()); return elements[i]; }
        
        /// Returns true if this element or any of its elements are in transition
        //        virtual bool InTransition() override;
        
        /// 
        virtual void PlayAnimation(const string& name) override;
        
        bool Masking() const { return masking; }
        void SetMasking(bool m) { masking = m; }
        
#ifdef DEBUG
        virtual void DebugDraw(DebugDraw2D& debugDraw) override;
#endif
        
        
    protected:
        
        static bool Compare(GUIElement* lhs, GUIElement* rhs)
        { return lhs->layer < rhs->layer; }
        
        /// Checks if any of its elements are in transition
        bool ElementsInTransition();
    };
}
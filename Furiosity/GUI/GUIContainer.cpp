////////////////////////////////////////////////////////////////////////////////
//  GUIContainer.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUIContainer.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                          - UIContainer -
//
////////////////////////////////////////////////////////////////////////////////

Texture* GUIContainer::mask = 0;

////////////////////////////////////////////////////////////////////////////////
// Ctor
////////////////////////////////////////////////////////////////////////////////
GUIContainer::GUIContainer(Vector2 position, Vector2 size) :
    GUIElement(position, size),
    masking(false)
{}

////////////////////////////////////////////////////////////////////////////////
// Dtor
////////////////////////////////////////////////////////////////////////////////
GUIContainer::~GUIContainer()
{
    // Delete all elements
    for (int i = 0; i < elements.size(); ++i)
        SafeDelete( elements[i] );
    elements.clear();
    
    // Delete all elements
    for (int i = 0; i < deleteQueue.size(); ++i)
        SafeDelete( deleteQueue[i] );
    deleteQueue.clear();
    
    // gResourceManager.ReleaseResource(mask);
}

////////////////////////////////////////////////////////////////////////////////
// Add element
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::AddElement(GUIElement *child, int layer)
{
    // Set parent to this
    child->parent = this;
    child->AnchorInParent();
    child->UpdateTransform();
    child->UpdateColor();
    //
    child->layer = (float)layer;
    
    // Add to list
    elements.push_back(child);

    SortElements();
    
    child->Update(0.0f);
}

////////////////////////////////////////////////////////////////////////////////
// Remove element
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::RemoveElement(GUIElement *child)
{
    // Go over all elements
    vector<GUIElement*>::iterator itr;
    for (itr = elements.begin(); itr < elements.end(); ++itr)
        if( (*itr) == child)
            elements.erase(itr);
}

////////////////////////////////////////////////////////////////////////////////
// Remove and delete element
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::RemoveAndDeleteElement(GUIElement *child)
{
    // Go over all elements and remove now
    vector<GUIElement*>::iterator itr;
    for (itr = elements.begin(); itr < elements.end(); ++itr)
        if( (*itr) == child)
            elements.erase(itr);
    
    // Delete
    deleteQueue.push_back(child);
}


////////////////////////////////////////////////////////////////////////////////
// ClearContainer
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::ClearContainer()
{
    // Go over all elements and remove now
    for (auto el : elements)
        SafeDelete(el);

    elements.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Contains element
////////////////////////////////////////////////////////////////////////////////
bool GUIContainer::ContainsElement(GUIElement *child)
{
    // Go over all elements
    vector<GUIElement*>::iterator itr;
    for (itr = elements.begin(); itr < elements.end(); ++itr)
        if( (*itr) == child)
            return true;
    return false;
}

void GUIContainer::SortElements()
{
    // Sort by layer
    stable_sort( elements.begin(), elements.end(), GUIContainer::Compare );
}

////////////////////////////////////////////////////////////////////////////////
// Propagate update
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::Update(float dt)
{
    GUIElement::Update(dt);
    
    // Propagate to all elements
    for (int i = 0; i < elements.size(); ++i)
    {
        GUIElement* el = elements[i];
        //        if (el->Enabled())
        // Always update
        el->Update(dt);
    }
    
    // Delete queued elements
    for (int i = 0; i < deleteQueue.size(); ++i)
        SafeDelete( deleteQueue[i] );
    deleteQueue.clear();
    
}

////////////////////////////////////////////////////////////////////////////////
// Propagate a touch
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::HandleTouch(Touch* touch)
{
    // if (!Enabled() /* || InTransition() */)
    //  return;
    
    // TODO: This might loose touches
    if(!InRegion(touch))
        return;
    
    // Propagate all elements
    for (int i = ((int)elements.size() - 1); i >= 0; --i)
    {
        GUIElement* el = elements[i];
        if (el->Enabled() /* || el->InTransition() */)
        {
            el->HandleTouch(touch);
            
            // If this element captures input, don't propagate further
            if (el->Capture())
            {
                touch->Handle(el);
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Propagate render
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::Render(SpriteRender& rend)
{
    UpdateTransform();
    
    if(!Visible() || tint.a == 0)
        return;
    
    if(masking)
    {
        glEnable(GL_STENCIL_TEST);                      // Enable Stencil Buffer For "marking" The Floor
        glStencilFunc(GL_NEVER, 1, 1);
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        
#ifdef ANDROID
        // Disable rendering of all colors, otherwise android
        // draws a grey-colored area.
        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
#endif
        rend.DrawQuad(transform,
                      size.x,
                      size.y,
                      mask,
                      offset,
                      Color(255, 255, 255, 255));
        
        glStencilFunc(GL_EQUAL, 1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        
#ifdef ANDROID
        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
#endif
    }
    
    // Draw all elements
    for (int i = 0; i < elements.size(); ++i)
    {
        GUIElement* el = elements[i];
        if (el->visible)
            el->Render(rend);
    }
    
    if(masking) glDisable(GL_STENCIL_TEST);                      // Disable
}

////////////////////////////////////////////////////////////////////////////////
// Propagate update transform
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::UpdateTransform()
{
    GUIElement::UpdateTransform();
    
    // Propagate to all elements
    for (auto el : elements)
        el->UpdateTransform();
}

/*
 ////////////////////////////////////////////////////////////////////////////////
 // Propagate SetEnabled - chance to change state if needed
 ////////////////////////////////////////////////////////////////////////////////
 void GUIContainer::SetEnabled(bool e)
 {
 // Check if alrady set
 if(enabled == e)
 return;
 
 enabled = e;
 //
 for (int i = 0; i < elements.size(); ++i)
 {
 GUIElement* el = elements[i];
 el->SetEnabled(e);
 }
 }
 */
/*
 void GUIContainer::Show()
 {
 GUIElement::Show();
 
 // Propagte to all the elements
 for(auto itr = elements.begin(); itr != elements.end(); ++itr)
 (*itr)->Show();
 }
 
 void GUIContainer::Hide()
 {
 GUIElement::Hide();
 
 // Propagte to all the elements
 for(auto itr = elements.begin(); itr != elements.end(); ++itr)
 (*itr)->Hide();
 }
 */

/*
////////////////////////////////////////////////////////////////////////////////
// InTransition
////////////////////////////////////////////////////////////////////////////////
bool GUIContainer::InTransition()
{
    // If this container itself is in transition, then true
    if( GUIElement::InTransition() )
        return true;
    
    // If any of its elements are in transition, true again
    return ElementsInTransition();
}
*/
 
////////////////////////////////////////////////////////////////////////////////
// Play
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::PlayAnimation(const string& name)
{
    GUIElement::PlayAnimation(name);
    //
    for(auto itr = elements.begin(); itr != elements.end(); ++itr)
        (*itr)->PlayAnimation(name);
}


/*
////////////////////////////////////////////////////////////////////////////////
// ElementsInTransition
////////////////////////////////////////////////////////////////////////////////
bool GUIContainer::ElementsInTransition()
{
    // If any of its elements are in transition, true
    for(auto itr = elements.begin(); itr != elements.end(); ++itr)
        if( (*itr)->InTransition() )
            return true;
    
    // Nothing in transition, false
    return false;
}
*/

#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////////
// DebugDraw
////////////////////////////////////////////////////////////////////////////////
void GUIContainer::DebugDraw(DebugDraw2D& debugDraw)
{
    if(!Enabled())
        return;
    
    GUIElement::DebugDraw(debugDraw);
    
    for(auto itr = elements.begin(); itr != elements.end(); ++itr)
        (*itr)->DebugDraw(debugDraw);
}
#endif

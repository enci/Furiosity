////////////////////////////////////////////////////////////////////////////////
//  GUILabel.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GUILabel.h"

using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
//
//                          - Label -
//
////////////////////////////////////////////////////////////////////////////////

GUILabel::GUILabel(Vector2              origin,
                   const string&        labelText,
                   const std::string&   fontName,
                   uint                 fontSize,
                   float                maxWidth) :
    GUIImage(origin, Vector2())
{
    label = gResourceManager.LoadLabel(labelText, fontName, fontSize, maxWidth);
    label->AddReloadEvent(this, [this](const Resource& res)
    {
        Reloaded(static_cast<const Label&>(res));
    });
    
    texture = label;
    fromUV  = Vector2(0.0f, 0.0f);
    size = label->GetSize();
    toUV = label->GetUvTo();
    offset.y = -label->YOffset() * 0.5f;
}

GUILabel::~GUILabel()
{
    // The texture will relese the resource
    // but we need to remove the event
    label->RemoveReloadEvent(this);
}

/*
void GUILabel::Update(float dt)
{
    GUIImage::Update(dt);
    //
    size = label->GetSize();
    toUV = label->GetUvTo();
    offset.y = -label->YOffset() * 0.5f;
    
//    offset.y = 0;
}
*/

void GUILabel::SetText(const std::string& text)
{
    if(label->OriginalText() == text)
        return;
    
    SetLabel(text, label->FontFile(), label->FontSize());
}

void GUILabel::SetFont( const std::string& fontName )
{
    if(label->FontFile() == fontName)
        return;
    
    SetLabel(label->OriginalText(), fontName, label->FontSize());
}

void GUILabel::SetFontSize( const uint& fontSize )
{
    if(label->FontSize() == fontSize)
        return;
    
    SetLabel(label->OriginalText(), label->FontFile(), fontSize);
}

void GUILabel::SetLabel(const std::string& text,
                     const std::string& fontName,
                     const uint& fontSize,
                     float maxWidth)
{
    // Avoid reloads due to sloppy coding
    if(label->OriginalText() == text &&
       label->FontFile() == fontName &&
       label->FontSize() == fontSize)
        return;
    
    // Copy for deleting
	Label* copy = label;
    
    // Load (create) new label
	label    = gResourceManager.LoadLabel(text, fontName, fontSize, maxWidth);
    label->AddReloadEvent(this, [this](const Resource& res)
    {
      Reloaded(static_cast<const Label&>(res));
    });

    
    // Set the new size
    SetNewSize(label->GetSize());
    
    // Set the textures and rest of params
	texture  = label;
	fromUV   = Vector2(0.0f, 0.0f);
	toUV     = label->GetUvTo();
	offset.y = -label->YOffset();
    
    // Delete copy
    copy->RemoveReloadEvent(this);
	gResourceManager.ReleaseResource(copy);
}

void GUILabel::Reloaded(const Label& res)
{
    SetNewSize(label->GetSize());
    toUV = label->GetUvTo();
    offset.y = -label->YOffset() * 0.5f;
}

void GUILabel::SetNewSize(Vector2 newsize)
{
    Vector2 sizeDiff = newsize - Size();
    
    if(anchoring & ANCHORING_TOP)
        anchoredPosition.y += sizeDiff.y * -0.5f;
    else if(anchoring & ANCHORING_BOTTOM)
        anchoredPosition.y += sizeDiff.y * 0.5f;
    
    if(anchoring & ANCHORING_LEFT)
        anchoredPosition.x += sizeDiff.x * 0.5f;
    else if(anchoring & ANCHORING_RIGHT)
        anchoredPosition.x += sizeDiff.x * -0.5f;

    size = label->GetSize();
}



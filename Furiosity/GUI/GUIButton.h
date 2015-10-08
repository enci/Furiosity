////////////////////////////////////////////////////////////////////////////////
//  GUIButton.h
//  Furiosity
//
//  Created by Bojan Endrovski on 9/27/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

// Local
#include "GUIElement.h"
#include "SoundResource.h"

namespace Furiosity
{
    ///
    /// Button - the single most clicked UI element in a game
    ///
    class GUIButton : public GUIElement
    {
    protected:
        
        // Store event function
        std::function<void(void)> event;
        
        // Store event function that provides the toggle data too
        std::function<void(bool)> toggleEvent;
        
        // A texutre for the button
        Texture*            texture;
        
        // The button connects
        Touch*              touch;
        
        //        SoundResource*    sound;
        
        Sound*              sound;
        
        bool                pressed;
        
        Vector2             texOffset;
        
        bool                isToggle;
        
        bool                toggled;
        
        /// Protect from dumb people
        float               cooldownTime;
        
        /// Protect from dumb people
        float               cooldown;
        
        /// Protect from clumsy people
        float               tolerance;
        
    public:
        
        /// Creates a new button with size and path to the texutre
        ///
        /// @param origin The position of the button in the paren container
        /// @param size The size of the button in GUI units
        /// @param texutrename  The texture to be used. For normal buttons this
        ///                     is the a two grid, for toggle buttons a four grid.
        ///                     Empty name will not render the buttons at all.
        /// @param isToggle Wheter this a statefull (toggle) button. Default is false.
        /// @param mipmap Should the texture be mipmap-ed. Default is false.
        GUIButton(Vector2 origin,
                  Vector2 size,
                  const string& texutrename,
                  bool isToggle = false,
                  bool mipmap   = false);
        
        /// Dtor
        virtual ~GUIButton();
        
        /// Update button state
        virtual void Update(float dt) override;
        
        /// Handle a touch
        virtual void HandleTouch(Touch* touch) override;
        
        /// Needs to drop few things when set do dissabled
        virtual void Hide() override;
        
        /// The texture for the button can be changed usig this method
        virtual void SetTexture(const string& texutrename,
                                bool mipmap = false);
        
        /// Set event for callback
        void SetEvent(std::function<void(void)> e) { event = e; }

        /// Set event for callback
        void SetEvent(std::function<void(bool)> e) { toggleEvent = e; }
        
        /// Set the sound to be played when clicked
        void SetSound(const string& soundname);
        
        /// Set toggled
        void SetToggled(bool tg) { toggled = tg; }
        
        /// Is toggled, only valid when
        bool IsToggled() const { return toggled; }
                
        const float& Toleance() const { return tolerance; }
        
        void SetTolerance(float t) { tolerance = t; }
        
        const float& Cooldown() const { return cooldown; }
        
        void SetCooldown(float cd) { cooldownTime = cd; }
        
        const bool IsHeld() const { return touch != 0; }
        
        // Draw simple button
        virtual void Render(SpriteRender& rend) override;
        
#ifdef DEBUG
        virtual void DebugDraw(DebugDraw2D& debugDraw) override;
#endif
    };
    
    
    ///
    /// Shrink Button - a lazy version of a button
    ///
    class GUIShrinkButton : public GUIButton
    {
        float minSize;
        
        float sizeFix;
        
    public:
        /// Creates a new button with size and a path to the texutre
        GUIShrinkButton(Furiosity::Vector2 origin,
                        Furiosity::Vector2 size,
                        const string&  texutrename,
                        float minSize = 0.9f,
                        bool isToggle = false,
                        bool mipmap   = false);
        
        virtual ~GUIShrinkButton() {};
        
        /// Draw shrink button
        virtual void Render(Furiosity::SpriteRender& rend);
        
        float SizeFix() const { return sizeFix; }
        void SetSizeFix(float f) { sizeFix = f; }
    };

}
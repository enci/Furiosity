////////////////////////////////////////////////////////////////////////////////
//  GeneralManager.cpp
//
//  Created by Bojan Endrovski on 12/30/12.
//  Copyright (c) 2012 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GeneralManager.h"

#include "ResourceManager.h"
#include "Input.h"
#include "GUI.h"
#include "AudioManager.h"

using namespace Furiosity;


GeneralManager* Furiosity::GeneralManager::instance = 0;

// A reference to a global general manager will be created
// when the game launches.
GeneralManager& Furiosity::gGeneralManager = *GeneralManager::Instance();

void GeneralManager::Initialize(const std::string &resourcesPath,
                                const std::string &localPath,
                                const Device& device,
                                float GUISafeWidth,
                                float GUISafeHeight,
                                bool  fitToScreen,
                                const std::string &remotePath)
{
    this->device        = device;
    this->screenWidth   = device.GetScreenWidth();
    this->screenHeight  = device.GetScreenHeight();
    this->fitToScreen   = fitToScreen;
    
    // First init resource manager
    gResourceManager.Initialize(resourcesPath, localPath, remotePath);
    // Next is input
    gInputManager.Initialize();
    // Next is audio
    gAudioManager.Initialize();
    // Next is GUI
    gGUIManager.Initialize(GUISafeWidth, GUISafeHeight);    
}

void GeneralManager::Update(float dt)
{
    // First init resource manager
    // gResourceManager.Update(dt);
    
    // Next is input
    gInputManager.Update(dt);
    
    // Next is audio
    gAudioManager.Update(dt);
    
    // Next is GUI
    gGUIManager.Update(dt);
}

void GeneralManager::Shutdown()
{
    // First init resource manager
    gResourceManager.Shutdown();
    
    // Next is input
    // gInputManager.Shutdown();
    
    // Next is audio
    gAudioManager.Shutdown();
    
    // Next is GUI
    gGUIManager.Shutdown();
}

#ifdef IOS
void GeneralManager::ShowPopUp()
{
    /*
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Preset Saving..." message:@"Describe the Preset\n\n\n" delegate:view cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
    UITextField *textField = [[UITextField alloc] init];
    // textField.delegate = view;
    // textField.borderStyle = UITextBorderStyleLine;
    // textField.frame = CGRectMake(15, 75, 255, 30);
    textField.placeholder = @"Preset Name";
    textField.keyboardAppearance = UIKeyboardAppearanceAlert;
    [textField becomeFirstResponder];
    [alert addSubview:textField];
     */
    
    UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Hello!" message:@"Please enter your name:" delegate:view cancelButtonTitle:@"Continue" otherButtonTitles:nil];
    alert.alertViewStyle = UIAlertViewStylePlainTextInput;
    UITextField * alertTextField = [alert textFieldAtIndex:0];
    alertTextField.keyboardType = UIKeyboardTypeNumberPad;
    alertTextField.placeholder = @"Enter your name";
    [alert show];
    [alert release];
}

void GeneralManager::SetIOSView(GLKViewController *view)
{
    this->view = view;
}

#endif


GeneralManager* GeneralManager::Instance()
{
    if(instance == 0)
        instance = new GeneralManager();
    
    return instance;
}

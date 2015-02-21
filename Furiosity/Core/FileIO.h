////////////////////////////////////////////////////////////////////////////////
//  FileIO.h
//  Furiosity
//
//  Created by Bojan Endrovski on 5/20/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Framework includes
#include <string>

namespace Furiosity
{
    /// Read a text file into a string
    ///
    /// @filename   Full path to the file
    ///
    std::string ReadFile(const std::string& filename);
    
    /// Save text to a file
    /// @filename   Full path to the file
    /// @text       Text to save
    ///
    bool SaveFile(const std::string& filename,
                  const std::string& text);
    
    /// Read an encrypted text file into a string
    ///
    /// @filename   Full path to the file
    /// @key        Encryption key
    ///
    std::string ReadEncryptedFile(const std::string& filename,
                                  const std::string& key);
    
    /// Encrypt and save text to a file. Uses RC4 ecryption
    ///
    /// @filename   Full path to the file
    /// @text       Text to encrypt and save
    /// @key        Encryption key
    ///
    bool SaveEncryptedFile(const std::string& filename,
                           const std::string& text,
                           const std::string& key);
}


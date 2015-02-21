////////////////////////////////////////////////////////////////////////////////
//  RC4.h
//  Furiosity
//
//  Created by Bojan Endrovski on 5/20/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

namespace Furiosity
{
    ///
    /// Simple RC4 Encryption
    ///
    /// Modified version of
    /// http://www.codeproject.com/Articles/35374/Two-Efficient-Classes-for-RC4-and-Base64-Stream-Ci
    ///
    class RC4
    {
    public:
        /// Ctor
        RC4 ();
        
        /// Dtor
        ~RC4 ();
        
        /// Encrypt a string with the a key
        ///
        /// @param text Text to be encrypted
        /// @param key Key for encryption
        std::string Encrypt(const std::string& text, const std::string& key);
        
        /// Decrypt a string with the a key
        ///
        /// @param text Text to be decrypted
        /// @param key Key for decryption
        std::string Decrypt(const std::string& text, const std::string& key);
        
    private:
        // Encryption array
        unsigned char sbox[256];
        
        // Numeric key values
        unsigned char key[256];
        unsigned char k;
        
        // Ambiguously named counters
        size_t  m, n, i, j, ilen;
    };
}

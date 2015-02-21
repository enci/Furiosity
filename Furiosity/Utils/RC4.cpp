////////////////////////////////////////////////////////////////////////////////
//  RC4.cpp
//  Furiosity
//
//  Created by Bojan Endrovski on 5/20/13.
//  Copyright (c) 2013 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "RC4.h"

#include <cstdio>
#include <cstring>

using namespace std;
using namespace Furiosity;

#define SWAP(a, b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))

RC4::RC4 ()
{
    memset(sbox, 0, 256);
    memset(key, 0, 256);
}
    
RC4::~RC4 ()
{
    memset(sbox, 0, 256);  // remove Key traces in memory
    memset(key, 0, 256);
}
    
string RC4::Encrypt(const string& text, const string& key)
{
    i=0, j=0,n = 0;
    ilen = key.size();
    const char* pszKey = key.c_str();
    
    // Initialize the key sequence
    for (m = 0;  m < 256; m++)
    {
        *(this->key + m)= *(pszKey + (m % ilen));
        *(sbox + m) = m;
    }
    for (m=0; m < 256; m++)
    {
        n = (n + *(sbox+m) + *(this->key + m)) &0xff;
        SWAP(*(sbox + m), *(sbox + n));
    }
    
    ilen = text.size();
    const char* pszText = text.c_str();
    string temp(text);
    
    for (m = 0; m < ilen; m++)
    {
        i = (i + 1) &0xff;
        j = (j + *(sbox + i)) &0xff;
        SWAP(*(sbox+i), *(sbox + j));   // randomly Initialize the key sequence
        k = *(sbox + ((*(sbox + i) + *(sbox + j)) &0xff ));
        //
        // avoid '\0' among the encoded text
        if(k == *(pszText + m))
            k = 0;
        //
         temp[m] ^=  k;
    }
    
    return temp;
}

std::string RC4::Decrypt(const std::string& text, const std::string& key)
{
    return Encrypt(text, key) ;  // using the same function as encoding
}

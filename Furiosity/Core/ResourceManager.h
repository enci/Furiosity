////////////////////////////////////////////////////////////////////////////////
//  ResourceManager.h
//
//  Created by Bojan Endrovski on 10/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

// Framework includes
#include <string>
#include <map>
#include <sstream>
#include <vector>

#ifdef ANDROID
#   include <unistd.h>
#	include <android/asset_manager.h>
#endif


namespace Furiosity
{
    class Resource;
    class Shader;
    class Label;
    class ResourcePack;
    class Font;
    class SoundResource;
    class Texture;
    class XmlResource;
    class SvgImage;
    class ModelMesh3D;
    class ModelScene3DResource;

#ifdef IOS
    class ResourceConsole;
#endif
    
    using std::string;
    using std::map;
    using std::vector;
    using std::pair;

    class ResourceManager
    {
        friend class ResourceConsole;
    private:
        /// Where the resources are
        string resourcesPath;
        
        /// Where the local files are (save game and such)
        string localPath;
        
        /// Where can we find the files over the line
        string remotePath;
        
        /// A map of resources
        map<long, Resource*>           resources;
        
        /// A map of reference counters
        map<long, unsigned int>        refCounters;
        
        /// A cache of all string that are share in the game
        map<string, string>  stringCache;
        
        /// Current language in the game
        string                         language;
        
        /// Mapping between wildcards and their respective value
        vector<pair<string, string> > wildcards;
        
        /// Font downscaling for labels.
        int fontDownscale;

#ifdef ANDROID
        AAssetManager* assetManager;
#endif
        
        /// Get resource if loaded, null otherwise. This is designed for
        /// internal use only.
        ///
        /// @param ident The unique identifier for the resource.
        /// @return The requested resource, or 0 if not found.
        Resource* GetResource(const string& ident);
        
        /// Add a new resource into the cache. Sets the default reference
        /// count to 1.  This is designed for internal use only.
        ///
        /// @param ident The unique identifier for the resource.
        /// @param resource The to be stored resource.
        /// @return The same resource as passed to this function. Added
        /// for easy method chaining.
        Resource* AddResource(const string& ident, Resource* resource);
        
        /// Returns a vector with all the resources
        vector<Resource*> GetResources() const;

        /// Reload this 
        // static void ReloadResource(void* sender, void* data);
        
#ifdef DEBUG
        /// key(hashed) -> value(name, loadCount)
        std::map<long, std::pair<std::string, int> > _loadCounter;
#endif

    public: 
        /// Default constructor for static initialization
        ResourceManager() : fontDownscale(1) {
            #ifdef ANDROID
                assetManager = 0;
            #endif
        }

        /// Dtor
        virtual ~ResourceManager();
        
        /// Actual initialization
        void Initialize(const string& resourcesPath,
                        const string& localPath,
                        const string& remotePath = "");

        /// Actual destruction
        virtual void Shutdown();
        
#ifdef ANDROID
        AAssetManager* GetAndroidAssetManager() const;
        void SetAndroidAssetManager(AAssetManager* assetManager);
        bool IsAndroidApkPath(const string& filename) const;
#endif

        /// Returns a usable path on a device        
        string GetPath(const string& resource) const;
        
        
        /// Gets the path to local files
        string GetDocumentPath(const string& file) const;
        
        /// Gets the path to local files
        string GetRemotePath(const string& file) const;
        
        /// Gets the path to local files
        string GetCachedPath(const string& file) const;
        
        /// Checks the cached and resource paths and returns
        /// whichever has the latest version of the file
        string GetLatestPath(const string& file) const;
        
        
        ////////////////////////////////////////////////////////////////////////////////
        //                          Load Resource types
        ////////////////////////////////////////////////////////////////////////////////
        
        /// Loads a texture from file
        Texture* LoadTexture(const string& file, bool genMipMap = false);

        /// Loads a sound data from file
        SoundResource * LoadSoundData(const string&  file);
        
        /// Loads and DOM from an xml file in memory
        XmlResource* LoadXmlResource(const string& file);
        
        /// Load a shader
        Shader* LoadShader(const string& vertexPath, const string& fragmentPath);

        /// Load an label
        Label* LoadLabel(const string& text,
                         const string& fontfile,
                         uint fontsize,
                         float maxWidth = FLT_MAX);

        /// Load a font
        Font* LoadFont(const string& filename);
        
        /// Load a resource pack made out of multiple resources
        ResourcePack* LoadResourcePack(const string& filename);
        
        /// Load a SVG image
        SvgImage* LoadSvgImage(const string& filname);
                
        /// Load a 3D model 
        ModelMesh3D* LoadModel3D(const string&  file);
        
        /// Load a whole 3D scene
        ModelScene3DResource* LoadModelScene3D(const string&  file);
        
        ////////////////////////////////////////////////////////////////////////////////
        //                          Additional Methods
        ////////////////////////////////////////////////////////////////////////////////
        
        /// TODO: consider moving this to some other place (gerjo)
        int GetFontDownscale() { return fontDownscale; }
        
        void SetFontDownscale(int fontDownscale) { this->fontDownscale = fontDownscale; }

        /// It retains a copy of this particular resource
        void RetainResource(Resource* resource);
        
        /// It releases a reference to a source
        bool ReleaseResource(Resource* resource);
        
        /// Prints the current collection of resources kept
        void PrintStatus();
        
        /// Prints debug information such as wildcards and fontscale.
        void PrintInfo();
    
        /// 
        //void CreateResourceConsole();
        
        /// Add a file of localized strings
        bool TestFile(const string& file) const;
        
        /// Checks whether the file path is a URL
        bool IsURL(const string& file) const;
        
        /// Add a file of localized strings
        // void ClearStringCache(const string& file);
        
        /// Add a file of localized strings
        void AddToStringCache(const string& file);
        
        /// Get a localized string from the string cache
        const string& GetString(const string& ID);
        
        /// Set current language.
        /// TODO: consider moving this to some other place (gerjo)
        void SetLanguange(const string& lang) { language = lang; }
        
        string Language() const { return language; }
        
        /// Adds a wildcard, will override any existing wildcards with the same name.
        void AddWildcard(const string& wildcard, const string& value);
        
        string GetWildcard(const string& wildcard);
        
        /// Applies the internal wildcards to a given string.
        string ApplyWildcards(string input) const;
        
        /// Retrieve a file handle to the given filename
        FILE* OpenFile(const string& filename) const;

        /// Reloads all invalid resources. ANDROID ONLY!!
        void Reload();
        
        /// Reloads all resource of a matching type
        void Reload(uint typeflags);
        
        /// Reloads a resource of a matching type
        void Reload(const string& path, bool cached);
        
        vector<string> GetFilesOnLocation(const string& location,
                                          const string& extension = "");
    };
    
    /// Global manager kind of thing
    extern ResourceManager gResourceManager;
}


#endif

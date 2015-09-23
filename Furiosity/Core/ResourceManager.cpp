////////////////////////////////////////////////////////////////////////////////
//  ResourceManager.cpp
//
//  Created by Bojan Endrovski on 10/04/2011.
//  Copyright 2011 Furious Pixels. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

// Frameworks
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

#include "ResourceManager.h"

#include "Resource.h"
#include "Shader.h"
#include "Label.h"
#include "Font.h"
#include "SoundResource.h"
#include "ResourcePack.h"
#include "Utils.h"
#include "FileIO.h"
#include "Defines.h"
#include "Texture.h"
#include "Xml.h"
#include "Utils.h"
#include "Vector2.h"
#include "SvgImage.h"
#include "ModelMesh3D.h"
#include "ModelScene3D.h"

// Just for tracking stats
#include "Animation.h"
#include "GUI.h"
#include "Entity2D.h"


using namespace std;
using namespace Furiosity;

// Global resource manager, needs to be initialized
ResourceManager Furiosity::gResourceManager;


////////////////////////////////////////////////////////////////////////////////
// Dtor
////////////////////////////////////////////////////////////////////////////////
ResourceManager::~ResourceManager()
{
}


////////////////////////////////////////////////////////////////////////////////
// Initizalization method - Acts as a constructor
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::Initialize(const string& resourcesPath,
                                 const string& localPath,
                                 const string& remotePath)
{
    this->resourcesPath = resourcesPath;
    this->localPath     = localPath;
    this->remotePath    = remotePath;
    this->language      = "en";
}

////////////////////////////////////////////////////////////////////////////////
// Shutdown method - Acts as a destructor
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::Shutdown()
{
}

////////////////////////////////////////////////////////////////////////////////
// Reload - all invalid resources
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::Reload()
{

#if defined(ANDROID)
    deque<Resource*> toReload;
    double total = 0;
    int count = 0;
    for(pair<long, Resource*> p : resources) {
        double start = GetTiming();
        bool requireReload = ! p.second->IsValid();
        if(requireReload) {
            toReload.push_back(p.second);
        }

        double delta = GetTiming() - start;
        total += delta;
        ++count;
        //LOG("[%7.6f] require reload [%s]: %s",
        //        delta,
        //        requireReload ? "true" : "false",
        //        p.second->resourcePath.c_str()
        //);
    }

    LOG("[%7.6f] total time spend validating %d resources.", total, count);

    total = 0;
    count = 0;
    for(Resource* resource : toReload) {
    	unsigned name = ((Texture*)resource)->name;
        double start = GetTiming();
        resource->Reload();
        double delta = GetTiming() - start;
        total += delta;
        //LOG("[%7.6f] reloaded: %s", delta, resource->resourcePath.c_str());
        ++count;

        //LOG("Reloading: %s", resource->resourcePath.c_str());
    }

    LOG("[%7.6f] total time spend reloading %d resources.", total, count);

#endif
}


////////////////////////////////////////////////////////////////////////////////
// Reload - all resource of a matching type
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::Reload(uint typeflags)
{
    
    //double outerStart = GetMillies();
    for(pair<long, Resource*> p : resources)
    {
        Resource* r = p.second;
        if( (r->Type() & typeflags) == r->Type() )
        {
            //double start = GetTiming();
            
            // Give the resource a chance to release resources:
            r->Invalidate();
            
            // Forcibly reload!
            r->Reload();
            
            //LOG("Reload took: %f '%s'.\n", GetTiming() - start, r->resourcePath.c_str());
        }
    }
    
    //LOG("Total time spend: %f\n", GetTiming() - outerStart);
}



////////////////////////////////////////////////////////////////////////////////
// Android Asset Manager
////////////////////////////////////////////////////////////////////////////////
#ifdef ANDROID
AAssetManager* ResourceManager::GetAndroidAssetManager() const
{
    return assetManager;
}

void ResourceManager::SetAndroidAssetManager(AAssetManager* assetManager)
{
    this->assetManager = assetManager;
}

bool ResourceManager::IsAndroidApkPath(const string& filename) const
{
    // Test if file is on the SDcard (for fast deployment only)
    const string prefix("/sdcard/");

    // TODO: StringStartsWith
    if(filename.compare(0, prefix.length(), prefix) == 0) {
        return false;
    }

    // Test if file is located in the private storage directory:
    if(filename.compare(0, localPath.length(), localPath) == 0) {
        return false;
    }

    // Neither local storage nor SDcard
    return true;
}
#endif


////////////////////////////////////////////////////////////////////////////////
// GetResource  - gets the resource isf there
////////////////////////////////////////////////////////////////////////////////
Resource* ResourceManager::GetResource(const string& ident)
{
    // Calc hash
    unsigned long hash = StringHash(ident);
    
    auto it = resources.find(hash) ;
    
    // Check cache
    if (it != resources.end())
    {
         ++refCounters[hash];
        return it->second;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// AddResource - adds a generic resource to the manager
////////////////////////////////////////////////////////////////////////////////
Resource* ResourceManager::AddResource(const string& ident, Resource* resource)
{
    unsigned long hash = StringHash(ident);

    resources.insert(make_pair(hash, resource));
    refCounters.insert(make_pair(hash, 1));

    resource->resourceID   = hash;
    resource->resourcePath = ident;
    
    // Take note of frequently loaded resources. This is a persistent
    // counter. I've noted some GUI elements are recreated each frame
    // which causes some issues on Android.
#ifdef DEBUG
	if( _loadCounter.find(hash) == _loadCounter.end() )
	{
		_loadCounter.insert(make_pair(
			hash, make_pair(resource->Path(), 1)
		));
	}
	else
	{
		_loadCounter[hash].second++;
	}

	// Show an error after a threshold. Be gentle about it, use
	// a modulo.
	if(_loadCounter[hash].second > 100 &&
	   _loadCounter[hash].second % 25 == 0
	)
	{
		LOG("Warning: '%s' has been reloaded %d times.", _loadCounter[hash].first.c_str(), _loadCounter[hash].second);
	}

#endif

    // Return again, makes nice one-liners.
    return resource;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a usable path
// Assumes null terminated strings
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::GetPath(const string& resource) const
{
	//LOG("Requested path: %s", resource.c_str());

    string temp(resourcesPath);

    // Make sure the paths contain a slash in between:
    if(resource.length() > 0 && resource.at(0) != '/' &&
       resourcesPath.length() > 0 && resourcesPath.at(resourcesPath.length()-1) != '/')
    {
        temp.append("/");
    }

    temp.append(resource);
    
#ifdef ANDROID
    // Android requires paths relative to the "assets" folder. Strip a prefix
    // slash here. This _may_ trim the slash we added earlier. That is OK.
    if(temp.length() > 0 && temp.at(0) == '/')
    {
        temp = temp.substr(1, temp.length());
    }
#endif
    
    //LOG("Given path: %s", temp.c_str());

    return temp;
}


////////////////////////////////////////////////////////////////////////////////
// Creates a usable path
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::GetDocumentPath(const string& file) const
{
    string temp(localPath);
    
    // Make sure the paths contain a slash in between:
    if(file.length() > 0 && file.at(0) != '/' &&
       localPath.length() > 0 && localPath.at(localPath.length()-1) != '/')
    {
        temp.append("/");
    }

    temp.append(file);
    
    return temp;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a usable remote path
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::GetRemotePath(const string& file) const
{
    string temp(remotePath);
    
    // TODO: Add the slash here
    temp.append(file);
    
    return temp;
}

////////////////////////////////////////////////////////////////////////////////
// Creates a usable remote path
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::GetCachedPath(const string& file) const
{
    string filename = file;
    filename = StringReplace(filename, "/", "_");
    filename[0] = '/';
    //
    return GetDocumentPath(filename);
}


////////////////////////////////////////////////////////////////////////////////
// GetLatestPath
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::GetLatestPath(const string &file) const
{
	string resPath = GetPath(file);

#if defined(IOS)

	// IOS
    string chdPath = GetCachedPath(file);
    //
    struct stat resdata;
    if(stat(resPath.c_str(), &resdata) != 0) {
        ERROR("Stat failed on file: '%s'", resPath.c_str());
        return "";
    }
    //
    struct stat chddata;
    if(stat(chdPath.c_str(), &chddata))
        return resPath;
    //
    if(resdata.st_mtimespec.tv_sec > chddata.st_mtimespec.tv_sec)
        return resPath;
    else
        return chdPath;

#elif defined(ANDROID)
    // Android doesn't use asset reloading through the resource console.
    return resPath;

#endif
}

////////////////////////////////////////////////////////////////////////////////
// Load texture
////////////////////////////////////////////////////////////////////////////////
Texture* ResourceManager::LoadTexture(const string& file, bool genMipMap)
{
    const string path = ApplyWildcards(file);
    
    Resource* r = GetResource(path);
    
    if(r == 0) {
        r = AddResource(path, new Texture(path, genMipMap));
    }

    return static_cast<Texture*>(r);
}


////////////////////////////////////////////////////////////////////////////////
// Load sound
////////////////////////////////////////////////////////////////////////////////
SoundResource * ResourceManager::LoadSoundData(const string& file)
{
    const string path = ApplyWildcards(gResourceManager.GetPath(file));
    
    Resource* r = GetResource(path);
    
    if(r == 0) {
        r = AddResource(path, new SoundResource(path));
    }

    return static_cast<SoundResource *>(r);
}

////////////////////////////////////////////////////////////////////////////////
// Load XML
////////////////////////////////////////////////////////////////////////////////
XmlResource* ResourceManager::LoadXmlResource(const string& file)
{
    const string path = ApplyWildcards(file);
    
    Resource* r = GetResource(path);
    
    if(r == 0) {
        r = AddResource(path, new XmlResource(path));
    }

    return static_cast<XmlResource*>(r);
}

////////////////////////////////////////////////////////////////////////////////
// LoadShader
////////////////////////////////////////////////////////////////////////////////
Shader* ResourceManager::LoadShader(const string& vertexPath, const string& fragmentPath)
{
    string vertexpth = ApplyWildcards(vertexPath);
    string fragmentpth = ApplyWildcards(fragmentPath);
    
    // Ident intentionally doesn't use the full path. This looks nicer
    // in the debugger.
    const string ident = vertexPath + "\n" + fragmentPath;

    Resource* r = GetResource(ident);
    
    if(r == 0) {
        r = AddResource(ident, new Shader(vertexpth, fragmentpth));
    }

    return static_cast<Shader*>(r);
}

////////////////////////////////////////////////////////////////////////////////
// LoadLabel
////////////////////////////////////////////////////////////////////////////////
Label* ResourceManager::LoadLabel(const string& text, const string &fontfile,
                                  uint fontsize, float maxWidth)
{
    // NB: we are intentionally NOT calling "GetPath". The font resource also
    // has a "GetPath" call. We don't want to prefix twice.
    const string fontfilefull = ApplyWildcards(fontfile);


    // Complicated ident. Some charachters are escaped.
    stringstream ss;
    ss  << "label:'"  << text
        << "' font:'" << fontfilefull
        << "' size:'" << fontsize << "'";
    
    const string ident = ss.str();
    
    Resource* r = GetResource(ident);

    if(r == 0) {
        r = AddResource(ident, new Label(text, fontfilefull, fontsize, maxWidth));
    }
    
    return static_cast<Label*>(r);
}

////////////////////////////////////////////////////////////////////////////////
// LoadFont
////////////////////////////////////////////////////////////////////////////////
Font* ResourceManager::LoadFont(const string& filename)
{
    const string path = ApplyWildcards(gResourceManager.GetPath(filename));

    Resource* r = GetResource(path);
    
    if(r == 0) {
        r = AddResource(path, new Font(path));
        
        // Intentionally spam the logs. The programmer should retain an additional
        // font instance if there are multiple uses.
        #ifdef DEBUG
        LOG("ResourceManager::LoadFont() Created new font for file: '%s' \n", filename.c_str());
        #endif
    }
    
    return static_cast<Font*>(r);
}

////////////////////////////////////////////////////////////////////////////////
// LoadResourcePack
////////////////////////////////////////////////////////////////////////////////
ResourcePack* ResourceManager::LoadResourcePack(const string& filename)
{
    // NB: we are intentionally are NOT calling "GetPath" nor "ApplyWildcards".
    // this will be done by the XMLResource.

    Resource* r = GetResource(filename);
    
    if(r == 0) {
        XmlResource* xml = LoadXmlResource(filename);
        
        r = AddResource(filename, new ResourcePack(xml));
        
        // "LoadXmlResource" incremented count. The ResourceManager no longer
        // needs it, so reduce the reference count:
        ReleaseResource(xml);
    }
    
    return static_cast<ResourcePack*>(r);
}


////////////////////////////////////////////////////////////////////////////////
// LoadSvgImage
////////////////////////////////////////////////////////////////////////////////
SvgImage* ResourceManager::LoadSvgImage(const string& filename)
{
    const string path = ApplyWildcards(gResourceManager.GetPath(filename));

    Resource* r = GetResource(path);
    
    if(r == 0) {
        r = AddResource(path, new SvgImage(path));
    }
    
    return static_cast<SvgImage*>(r);
}


////////////////////////////////////////////////////////////////////////////////
// Load 3D model
////////////////////////////////////////////////////////////////////////////////
ModelMesh3D* ResourceManager::LoadModel3D(const string& file)
{
    const string path = ApplyWildcards(file);
    Resource* r = GetResource(path);
    
    if(r == 0)
        r = AddResource(path, new ModelMesh3D(path));
    
    return static_cast<ModelMesh3D*>(r);
}

#if USE_ASSIMP
////////////////////////////////////////////////////////////////////////////////
// Load 3D model
////////////////////////////////////////////////////////////////////////////////
ModelScene3DResource* ResourceManager::LoadModelScene3D(const string& file)
{
    const string path = ApplyWildcards(file);
    Resource* r = GetResource(path);
    
    if(r == 0)
        r = AddResource(path, new ModelScene3DResource(path));
    
    return static_cast<ModelScene3DResource*>(r);
}
#endif

////////////////////////////////////////////////////////////////////////////////
// Retain a resource by incrementing the internal reference counter.
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::RetainResource(Furiosity::Resource *resource)
{
    // Calc hash
    unsigned long hash = resource->ResourceID();
    
    // Check cache
    if (resources.find(hash) != resources.end())
    {
        // Resource in cache, just ++
        ++refCounters[hash];
    }
    
    // NB: this is secret behaviour!
    else
    {
        ERROR("We shouldn't get here. All resources should be loaded by the manager.");
        resources[hash]  = resource;
        refCounters[hash] = 1;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Release Resource
////////////////////////////////////////////////////////////////////////////////
bool ResourceManager::ReleaseResource(Resource *resource)
{
    if(!resource)
        return false;
    
    unsigned long hash = resource->resourceID;
    //
    // See if resource found
    if( resources.find(hash) != resources.end() )
    {
        if( --refCounters[hash] == 0 )
        {
            // No more references, delete resource
            resources.erase(hash);
            refCounters.erase(hash);
            //
            delete resource;
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Print Status
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::PrintStatus()
{
    int n = (int)resources.size(); _UNSUED(n);
    
    int size = 0;
    LOG("\n_____________________________ Resource Manager Stats _______________________________\n" );
    map<long, Resource*>::iterator itr;
    for (itr = resources.begin(); itr != resources.end(); ++itr)
    {
        Resource* res   = itr->second;
        unsigned int c  = refCounters[res->resourceID]; _UNSUED(c);
        size += res->size;
        
        const char* path = res->resourcePath.c_str();
        
        // Paths may contain the local prefix, which makes them unreadable.
        if(StringStartsWith(res->resourcePath, GetPath("")))
        {
            // Cheap version of substring. Offset with the prefix.
            path += GetPath("").length();
        }
         
        LOG("%ix\t%5.2f KB\t%s\n", c, res->size / 1024.0f, path);
    }
    LOG("Total memory: %i byte   %5.2f KB   %3.2f MB \n\n", size, (float)size / 1024.0f, (float)size / 1048576.0f);
    
    LOG("Number of tracked items: %i\n", n);
    LOG("Number of live items: %i\n",   Resource::Active());
    LOG("Live GUI elements: %i\n",      GUIElement::Active());
    LOG("Live game entities: %i\n",     Entity2D::Active());
    
    #if defined(DEBUG) && defined(KEEP_REGISTER)
    auto cr = Animation::centralRegister;
    LOG("Active animations: %i\n", Animation::Active());
    #endif
    LOG("____________________________________________________________________________________\n");
}

////////////////////////////////////////////////////////////////////////////////
// Print debug details
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::PrintInfo()
{
    LOG("\n___________________________ Resource Manager Information ___________________________\n");
    //
    LOG("Resources Path: %s",   resourcesPath.c_str());
    LOG("Documents Path: %s",   localPath.c_str());
    LOG("Remote Path: %s",      remotePath.c_str());
    //
    for(auto it = wildcards.begin(); it != wildcards.end(); ++it)
    {
        LOG("Wildcard: %s -> %s\n", (*it).first.c_str(), (*it).second.c_str());
    }

    LOG("Fontscale: %i\n", fontDownscale);
    LOG("____________________________________________________________________________________\n");
}

////////////////////////////////////////////////////////////////////////////////
// GetResources	
////////////////////////////////////////////////////////////////////////////////
vector<Resource*> ResourceManager::GetResources() const
{
    vector<Resource*> resourceList;
    for (auto pair : resources)
         resourceList.push_back(pair.second);
    return resourceList;
}


////////////////////////////////////////////////////////////////////////////////
// Adds resouces into a string cache and keeps them there
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::AddToStringCache(const string& file)
{
    XmlResource* res = this->LoadXmlResource(file);
    
    const XMLElement& element   = res->RootElement();
    const XMLElement* stringEl  = element.FirstChildElement("string");
    
    while(stringEl != 0)
    {
        // ID
        assert(stringEl->Attribute("ID"));
        string id   = stringEl->Attribute("ID");

        // Content
        const XMLElement* content = stringEl->FirstChildElement( language.c_str() );
        assert(content);
        string text = "";
        if(content->FirstChild())
            text = content->FirstChild()->Value();
        //
        stringCache[id] = text;
        
        // ++
        stringEl = stringEl->NextSiblingElement("string");
    }
    
    ReleaseResource(res);
}


/// Get a localized string from the string cache
const string& ResourceManager::GetString(const string& ID)
{
    auto itr = stringCache.find(ID);
    
    if(itr != stringCache.end())
        return itr->second;
    else
        return ID;
}



////////////////////////////////////////////////////////////////////////////////
// AddWildcard
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::AddWildcard(const string& wildcard, const string& value)
{
    //remove(wildcards.begin(), wildcards.end(), wildcard);
    
    for(auto it = wildcards.begin(); it != wildcards.end(); ++it)
    {
        if((*it).first.compare(wildcard) == 0)
        {
            wildcards.erase(it);
            break;
        }
    }
    
    wildcards.push_back(make_pair(wildcard, value));
}

////////////////////////////////////////////////////////////////////////////////
// GetWildcard
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::GetWildcard(const string& wildcard)
{
    for(auto it = wildcards.begin(); it != wildcards.end(); ++it)
    {
        if((*it).first.compare(wildcard) == 0)
        {
            return (*it).second;
        }
    }
    
    return "";
}

////////////////////////////////////////////////////////////////////////////////
// ApplyWildcards
////////////////////////////////////////////////////////////////////////////////
string ResourceManager::ApplyWildcards(string input) const
{
    // Make sure the paths contain a slash in between:
    if(input.length() > 0 && input.at(0) != '/' &&
       resourcesPath.length() > 0 && resourcesPath.at(resourcesPath.length()-1) != '/')
    {
        //LOG("Resource: %s doesn't have a / at the start!", input.c_str());
        //input = "/" + input;
    }
    
    // Now apply the wildcards
    for(const pair<const string, string>& p : wildcards)
    {
        if(input.find(p.first) != string::npos)
        {
            input = StringReplace(input, p.first, p.second);
        }
    }
    
    return input;
}


////////////////////////////////////////////////////////////////////////////////
// Open file
////////////////////////////////////////////////////////////////////////////////
FILE* ResourceManager::OpenFile(const string& filename) const {

#ifdef ANDROID

    // Depending on the file path, different loading approached are required.
    // During debugging some assets may be on the sdcard instead of inside
    // the APK. Deploying a large APK is slow.
    if( ! IsAndroidApkPath(filename) )
    {

        // Directly open from the filesystem:
        FILE* file = fopen(filename.c_str(), "rb");

#ifdef DEBUG
        if(file == NULL)
        {
            LOG("ResourceManager::OpenFile(%s) - cannot open file for reading.", filename.c_str());
        }
#endif

        return file;
    }

    // Retrieve a file handle from the APK (only works for not compressed assets
    // such as PNG and MP3. For reading compressed assets, use ReadFile() instead.
	AAsset* asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);

	if (asset)
	{
		off_t offset = 0, length = 0;
		const int fd = AAsset_openFileDescriptor(asset, &offset, &length);

		if (fd > 0)
		{
			FILE* file = fdopen(dup(fd), "rb");
			if(fd > 0)
			{
				fseek(file, offset, SEEK_SET);
				AAsset_close(asset);
				return file;
			}
		}
#ifdef DEBUG
		else
		{
			LOG("ResourceManager::OpenFile(%s) Cannot read asset as it's compressed. Use Furiosity::ReadFile() instead.", filename.c_str());
		}
#endif
		AAsset_close(asset);
	}

	return 0;

#else
	// iOS keeps it simple.
	return fopen(filename.c_str(), "rb");
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Test if a file is there
////////////////////////////////////////////////////////////////////////////////
bool ResourceManager::TestFile(const string& file) const
{
    string wildFile(file);
    wildFile = ApplyWildcards(wildFile);

	// Custom routine to test if files exist inside the APK file
#ifdef ANDROID
	if(IsAndroidApkPath(wildFile))
    {
	    AAsset* asset = AAssetManager_open(assetManager,
                                           wildFile.c_str(),
	                                       AASSET_MODE_UNKNOWN);
	    // If we can open the file, assume it exists.
	    if(asset) {
	    	AAsset_close(asset);
	    	return true;
	    }

	    return false;
	}
#endif

    FILE* f = fopen(wildFile.c_str(), "r");
    if (f)
    {
        fclose(f);
        return true;
    }
    
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// IsURL
////////////////////////////////////////////////////////////////////////////////
bool ResourceManager::IsURL(const string& file) const
{
    string t("http://");
    
    if (file.compare(0, t.length() - 1, t) == 0)
        return true;
    
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Reload - all resource of a matching type
////////////////////////////////////////////////////////////////////////////////
void ResourceManager::Reload(const string& path, bool cached)
{
    // Calc hash
    unsigned long hash = StringHash(path);
    
    auto it = resources.find(hash);
    
    // Check cache
    if (it != resources.end())
        it->second->Reload(cached);
}


////////////////////////////////////////////////////////////////////////////////
// GetFilesOnLocation
////////////////////////////////////////////////////////////////////////////////
vector<string> ResourceManager::GetFilesOnLocation(const string& location,
                                                   const string& extension)
{
	vector<string> files;

// Custom code for Android to parse folders inside the APK.
#ifdef ANDROID
	if(IsAndroidApkPath(location)) {

		std::string apkPath = location;

		// Folders should _never_ end with a slash on Android.
		while(StringEndsWith(apkPath, "/")) {
			apkPath.pop_back();
		}

		//LOG("GetFilesOnLocation(%s, %s)", apkPath.c_str(), extension.c_str());

		// Open directory inside the APK
		AAssetDir* dir = AAssetManager_openDir(assetManager, apkPath.c_str()); //

		// Move to the first file
		AAssetDir_rewind(dir);

		for(const char* name; (name = AAssetDir_getNextFileName(dir)) != NULL;)
		{
			std::string file(name);

			//LOG(" - %s", name);

			if(StringEndsWith(file, extension))
				files.push_back(file);
		}


		AAssetDir_close(dir);

		// Early out.
		return files;
	}
#endif
    
    DIR* dirp = opendir(location.c_str());
    dirent* dp = NULL;
    //
    while((dp = readdir(dirp)) != NULL)
    {
        if (dp->d_name)
        {
            string file(dp->d_name);
            
            if(StringEndsWith(file, extension))
                files.push_back(file);
        }
    }
    
    int err = closedir(dirp);
    ASSERT(err == 0);
    
	return files;
}



// eof

#include <sstream>
#include <sys/time.h>
#include <cmath>
#include <memory>

#include "Utils.h"
#include "Defines.h"

// Includes required for "UrlGetContents"
#ifdef IOS 
#   import <Foundation/Foundation.h>
#endif

using namespace std;
using namespace Furiosity;

////////////////////////////////////////////////////////////////////////////////
// Strig hashing function
// Taken from http://www.cse.yorku.ca/~oz/hash.html (public domain)
////////////////////////////////////////////////////////////////////////////////
unsigned long Furiosity::StringHash(const std::string& str)
{
    unsigned long hash = 0;
    int c;
    const char* cstr = str.c_str();
    
    while ((c = *cstr++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    
    return hash;
}

////////////////////////////////////////////////////////////////////////////////
// GetTiming
// Get the current militime in seconds. Precise enough for benchmarking.
////////////////////////////////////////////////////////////////////////////////
double Furiosity::GetTiming()
{
    timeval now;
    int r = gettimeofday(&now, NULL);
    
    if(r != 0)
    {
        LOG("GetMillies() Unable to retrieve system time, error code: '%i'.", r);
    }
    
    return now.tv_sec + (now.tv_usec / 1000000.0);
}

////////////////////////////////////////////////////////////////////////////////
// StringReplace
// Courtesy of: http://stackoverflow.com/questions/5878775/how-to-find-and-replace-string
////////////////////////////////////////////////////////////////////////////////
string Furiosity::StringReplace(const std::string& subject,
                                const std::string& search,
                                const std::string& replace)
{
    std::string result(subject);
    size_t pos = 0;
    
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        result.replace(pos, search.length(), replace);
        pos += search.length();
    }
    
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// StringStartsWith
// Determine if a string starts with a given string.
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::StringStartsWith(const std::string& subject,
                                 const std::string& prefix)
{
    // Early out, prefix is longer than the subject:
    if(prefix.length() > subject.length())
    {
        return false;
    }
    
    // Compare per character:
    for(int i = 0; i < prefix.length(); ++i)
    {
        if(subject[i] != prefix[i])
        {
            return false;
        }
    }
    
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// StringEndsWith
// Determine if a string ends with a given string.
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::StringEndsWith(const std::string& subject,
                               const std::string& suffix)
{
    
    // Early out test:
    if(suffix.length() > subject.length())
    {
        return false;
    }
    
    // Resort to difficult to read C++ logic:
    return subject.compare(
        subject.length() - suffix.length(),
        suffix.length(),
        suffix
    ) == 0;
}

////////////////////////////////////////////////////////////////////////////////
// StringExplode
// Break a string into smaller strings.
////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> Furiosity::StringExplode(const std::string& subject,
                                                  const std::string& delimiter)
{
    std::vector<std::string> result;
    
    size_t offset = 0;
    
    for(size_t at = 0; (at = subject.find_first_of(delimiter, offset)) != std::string::npos; )
    {
        const std::string str = subject.substr(offset, at - offset);

        result.push_back(str);
        
        offset += str.length() + delimiter.length();
    }
    
    // There may be remnants:
    if(offset < subject.length())
    {
        result.push_back(subject.substr(offset));
    }
    
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// UrlGetContents
// Load HTTP websites into a string.
////////////////////////////////////////////////////////////////////////////////
bool Furiosity::UrlGetContents(const std::string& url, std::string& out, float timeout)
{
    if( ! StringStartsWith(url, "http://"))
    {
        ERROR("UrlGetContents(%s) url does not start with http://", url.c_str());
        return false;
    }

#ifdef IOS
    
    NSString* link          = [NSString stringWithCString:url.c_str() encoding:[NSString defaultCStringEncoding]];
    NSURLRequest* request   = [NSURLRequest requestWithURL:[NSURL URLWithString:link] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:timeout];
    NSURLResponse* response = nil;
    NSError* error    = nil;
    NSData* data      = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
    NSString* content = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];

    if(error != nil || content == NULL) {
        std::string message([[error localizedDescription] UTF8String]);
        
        // Log the error, rather than assert failure.
        LOG("UrlGetContents(%s) remote request failed. %s", url.c_str(), message.c_str());
        return false;
    }
    
    // Append to "output"
    out += std::string([content UTF8String]);

    return true;
#else
    // Implementing this without the use of Java may require a tremendous
    // amount of code.
    ERROR("UrlGetContents(%s) is not implemented for Android.", url.c_str());
    return false;
#endif

}

////////////////////////////////////////////////////////////////////////////////
// StringFormat
////////////////////////////////////////////////////////////////////////////////
std::string Furiosity::StringFormat(const std::string fmt_str, ...)
{
    int final_n, n = (int)fmt_str.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
    std::string str;
    char* formatted = nullptr;

    va_list ap;
    while(1) {
    	SafeDelete(formatted);
    	formatted = new char[n];

    	strncpy(formatted, fmt_str.c_str(), fmt_str.size());
        va_start(ap, fmt_str);
        final_n = vsnprintf(formatted, n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    std::string asString(formatted);

    SafeDelete(formatted);

    return asString;
}

////////////////////////////////////////////////////////////////////////////////
// StringTime
////////////////////////////////////////////////////////////////////////////////
std::string Furiosity::StringTime(float seconds)
{
	const unsigned int time = (unsigned int)seconds;
	stringstream ss;
	string s;
    string timestring;
    
    uint secs   = time % 60;
    uint mins   = (time / 60) % 60;
    // uint hrs    = (time / 60 / 60) % 24;
    
    if(secs > 9)
        timestring.insert(0, ToString(secs));
    else if (secs > 0)
        timestring.insert(0, "0" + ToString(secs));
    else
        timestring.insert(0, "00");
    
    if (mins > 9)
        timestring.insert(0, ToString(mins) + ":");
    else if (mins > 0)
        timestring.insert(0, "0" + ToString(mins) + ":");
    else
        timestring.insert(0, "00:");
    
    /*
    if (hrs > 0)
        timestring.insert(0, ToString(hrs) + ":");
     */
    
    /*

	ss << time / 60 / 60 / 24;
	s = ss.str();
	if( s.length() == 1 )
		s.insert( 0, "0" );
	s += " days ";
	timestring.insert( 0, s );
    */
    
    return timestring;
}

////////////////////////////////////////////////////////////////////////////////
// AppVersion
////////////////////////////////////////////////////////////////////////////////
std::string Furiosity::AppVersion()
{
#if defined(IOS)
    NSString* version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    NSString* build = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
    return  string([version cStringUsingEncoding:NSUTF8StringEncoding]) +
            " - Build " +
            string([build cStringUsingEncoding:NSUTF8StringEncoding]);
#elif defined(ANDROID)
    // This needs to be game code, not engine code (for android)
    return "0.5 - hardcoded.";
#else
    return "";
#endif
}




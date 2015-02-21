#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "logging.h"


namespace Furiosity
{

    /// Replace all occurrences of the search string with the replacement string.
    ///
    /// @param subject The string being searched and replaced on, otherwise known as the haystack.
    /// @param search The value being searched for, otherwise known as the needle.
    /// @param replace The replacement value that replaces found search values.
    /// @return a new string with all occurances replaced.
    ///
    std::string StringReplace(const std::string& subject,
                              const std::string& search,
                              const std::string& replace);
    
    /// String hash using sdbm algorithm
    unsigned long StringHash(const std::string& str);


    /// Retrieve the time since a fixed point in time. The output will
    /// be formatted as seconds. The return value is optimized to be human
    /// readable rather than for performance.
    ///
    /// @return time since fixed point in time.
    ///
    double GetTiming();
    
    /// Determine whether or not a string starts with the given prefix. Does
    /// not create an internal copy.
    ///
    /// @param subject The string being searched in.
    /// @param prefix The string to search for.
    /// @return a boolean indicating if the prefix was found.
    ///
    bool StringStartsWith(const std::string& subject,
                          const std::string& prefix);
    
    /// Determine whether or not a string ends with the given suffix. Does
    /// not create an internal copy.
    ///
    /// @param subject The string being searched in.
    /// @param prefix The string to search for.
    /// @return a boolean indicating if the suffix was found.
    ///
    bool StringEndsWith(const std::string& subject,
                        const std::string& suffix);
    
    /// Explode a string into smaller chunks. The chunk will not contain the
    /// delimiter.
    ///
    /// @param subject The string to explode
    /// @param delimiter The string to explode with.
    /// @return a boolean indicating if the suffix was found.
    ///
    std::vector<std::string> StringExplode(const std::string& subject,
                                           const std::string& delimiter);
    
    /// Load the contents of a URL into a string. Great for loading XML files
    /// from a remote server during development. This is a blocking call, without
    /// any sensible error checking - not usefull for production.
    ///
    /// @param url A string indicating the URL.
    /// @param out The output reference. The URL content will be loaded into this.
    /// @param timeout Delay in seconds which this call is allowed to block for.
    /// @return a boolean indicating if the URL was retrieved successfully.
    ///
    bool UrlGetContents(const std::string& url, std::string& out, float timeout = 0.2f);
    
    
    /// Generic to string method for various numerical types
    ///
    /// @param A numeric value to be converted to a string
    /// @return a string representation of the value
    template <class T>
    std::string ToString(const T& val)
    {
        std::stringstream ss;
        ss << val;
        std::string str = ss.str();
        return str;
    }
    
    /// Format a string as if using printf
    ///
    /// @param fmt_str Input string with formating
    /// @return Formated string
    std::string StringFormat(const std::string fmt_str, ...);
    
    /// Format string nicely for print
    ///
    /// @param Seconds total time in seconds
    /// @return hh:mm:ss
    std::string StringTime(float seconds);
    
    /// Returns the current version of the app
    std::string AppVersion();
}

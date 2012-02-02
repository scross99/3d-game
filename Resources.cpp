#include <string>
#include <Ogre.h>
#include "Resources.hpp"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Game3D {

	// This function will locate the path to our application on OS X,
	// unlike windows you can not rely on the curent working directory
	// for locating your configuration files and resources.
	std::string macBundlePath() {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		assert(mainBundle);
		
		CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
		assert(mainBundleURL);
		
		CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
		assert(cfStringRef);
		
		CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
		
		CFRelease(mainBundleURL);
		CFRelease(cfStringRef);
		
		return std::string(path);
#else
		return "";
#endif
	}
	
	// Provide a nice cross platform solution for locating the configuration files
	// On windows files are searched for in the current working directory, on OS X however
	// you must provide the full path, the helper function macBundlePath does this for us.
	std::string getResourcePath() {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		return macBundlePath() + "/Contents/Resources/";
#else
		return "";
#endif
	}
	
}


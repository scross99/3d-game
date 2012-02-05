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
	
	void loadResources() {
		// Load resource paths from config file.
		Ogre::ConfigFile cf;
		cf.load(getResourcePath() + "resources.cfg");
		
		// Go through all sections & settings in the file.
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
		
		Ogre::String secName, typeName, archName;
		
		while(seci.hasMoreElements()) {
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			
			for(i = settings->begin(); i != settings->end(); ++i) {
				typeName = i->first;
				archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
				// OS X does not set the working directory relative to the app,
				// In order to make things portable on OS X we need to provide
				// the loading with it's own bundle path location
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				    String(macBundlePath() + "/" + archName), typeName, secName);
#else
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				    archName, typeName, secName);
#endif
			}
		}
		
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}
	
}


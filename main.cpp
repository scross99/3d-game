#include <iostream>
#include <Ogre.h>
#include "Application.hpp"
#include "Resources.hpp"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char** argv)
#endif
{
	Game3D::Application app;
	
	Game3D::loadResources();
	
	try {
		app.go();
	} catch(Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
	}
	
	return 0;
}

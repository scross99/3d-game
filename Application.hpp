#ifndef GAME3D_APPLICATION_HPP
#define GAME3D_APPLICATION_HPP

#include <Ogre.h>
#include "FrameListener.hpp"

namespace Game3D {

	class Application {
		public:
			Application();
			
			~Application();
			
			void go();
			
		protected:
			bool setup();
			
			void createScene();
			
			void destroyScene();
			
			void setupResources();
			
			void createResourceListener();
			
		private:
			Ogre::Root* mRoot;
			Ogre::Camera* mCamera;
			Ogre::SceneManager* mSceneMgr;
			FrameListener* mFrameListener;
			Ogre::RenderWindow* mWindow;
			Ogre::String mResourcePath;
			
	};
	
}

#endif

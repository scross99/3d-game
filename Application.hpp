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
			
		private:
			Ogre::Root* root_;
			Ogre::SceneManager* sceneManager_;
			FrameListener* frameListener_;
			Ogre::RenderWindow* window_;
			
	};
	
}

#endif

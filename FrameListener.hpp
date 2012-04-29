#ifndef GAME3D_FRAMELISTENER_HPP
#define GAME3D_FRAMELISTENER_HPP

#include <boost/shared_ptr.hpp>

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

#include "Camera.hpp"
#include "World.hpp"

namespace Game3D {

	class FrameListener: public Ogre::FrameListener, public Ogre::WindowEventListener {
		public:
			FrameListener(Ogre::RenderWindow* window, World& world);
			
			void windowResized(Ogre::RenderWindow* rw);
			
			void windowClosed(Ogre::RenderWindow* rw);
			
			bool frameStarted(const Ogre::FrameEvent& evt);
			
			bool frameRenderingQueued(const Ogre::FrameEvent& evt);
			
			bool frameEnded(const Ogre::FrameEvent& evt);
			
		protected:
			World& world_;
			Ogre::RenderWindow* window_;
			//OIS Input devices
			OIS::InputManager* inputManager_;
			OIS::Mouse*    mouse_;
			OIS::Keyboard* keyboard_;
	};
	
}

#endif

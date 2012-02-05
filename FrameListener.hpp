#ifndef GAME3D_FRAMELISTENER_HPP
#define GAME3D_FRAMELISTENER_HPP

#include <boost/shared_ptr.hpp>

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

#include "Camera.hpp"

namespace Game3D {

	class FrameListener: public Ogre::FrameListener, public Ogre::WindowEventListener {
		public:
			FrameListener(Ogre::RenderWindow* win, const boost::shared_ptr<Camera>& camera, Ogre::SceneManager* sceneManager);
			
			~FrameListener();
			
			void windowResized(Ogre::RenderWindow* rw);
			
			void windowClosed(Ogre::RenderWindow* rw);
			
			bool processUnbufferedKeyInput(const Ogre::FrameEvent& evt);
			
			bool processUnbufferedMouseInput(const Ogre::FrameEvent& evt);
			
			void moveCamera();
			
			bool frameRenderingQueued(const Ogre::FrameEvent& evt);
			
			bool frameEnded(const Ogre::FrameEvent& evt);
			
		protected:
			Ogre::SceneManager* sceneManager_;
			
			boost::shared_ptr<Camera> camera_;
			
			Ogre::Vector3 translateVector_;
			Ogre::Real currentSpeed_;
			Ogre::RenderWindow* window_;
			
			float moveScale_;
			float speedLimit_;
			Ogre::Degree rotateScale_;
			Ogre::Radian rotX_, rotY_;
			
			Ogre::Real moveSpeed_;
			Ogre::Degree rotateSpeed_;
			
			//OIS Input devices
			OIS::InputManager* inputManager_;
			OIS::Mouse*    mouse_;
			OIS::Keyboard* keyboard_;
	};
	
}

#endif

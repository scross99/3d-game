#ifndef GAME3D_FRAMELISTENER_HPP
#define GAME3D_FRAMELISTENER_HPP

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

namespace Game3D {

	class FrameListener: public Ogre::FrameListener, public Ogre::WindowEventListener {
		public:
			FrameListener(Ogre::RenderWindow* win, Ogre::Camera* camera, Ogre::SceneManager* sceneManager);
			
			~FrameListener();
			
			void windowResized(Ogre::RenderWindow* rw);
			
			void windowClosed(Ogre::RenderWindow* rw);
			
			bool processUnbufferedKeyInput(const Ogre::FrameEvent& evt);
			
			bool processUnbufferedMouseInput(const Ogre::FrameEvent& evt);
			
			void moveCamera();
			
			bool frameRenderingQueued(const Ogre::FrameEvent& evt);
			
			bool frameEnded(const Ogre::FrameEvent& evt);
			
		protected:
			Ogre::Camera* camera_;
			Ogre::SceneManager* sceneManager_;
			Ogre::SceneNode* cameraNode_;
			Ogre::SceneNode* cameraYawNode_;
			Ogre::SceneNode* cameraPitchNode_;
			Ogre::SceneNode* cameraRollNode_;
			
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

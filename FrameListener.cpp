#include <math.h>

#include <boost/shared_ptr.hpp>

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

#include "Camera.hpp"
#include "FrameListener.hpp"

namespace Game3D {

	FrameListener::FrameListener(Ogre::RenderWindow* window, World& world) :
		world_(world), window_(window),
		inputManager_(0), mouse_(0), keyboard_(0) {
		
		Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
		OIS::ParamList pl;
		
		size_t windowHnd = 0;
		window->getCustomAttribute("WINDOW", &windowHnd);
		
		std::ostringstream windowHndStr;
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		
		inputManager_ = OIS::InputManager::createInputSystem(pl);
		
		const bool bufferedKeys = false, bufferedMouse = false;
		
		keyboard_ = static_cast<OIS::Keyboard*>(inputManager_->createInputObject(OIS::OISKeyboard, bufferedKeys));
		mouse_ = static_cast<OIS::Mouse*>(inputManager_->createInputObject(OIS::OISMouse, bufferedMouse));
	}
	
	// Adjust mouse clipping area.
	void FrameListener::windowResized(Ogre::RenderWindow* window) {
		unsigned int width, height, depth;
		int left, top;
		window->getMetrics(width, height, depth, left, top);
		
		const OIS::MouseState& ms = mouse_->getMouseState();
		ms.width = width;
		ms.height = height;
	}
	
	// Unattach OIS before window shutdown (very important under Linux).
	void FrameListener::windowClosed(Ogre::RenderWindow* window) {
		// Only close for window that created OIS.
		if(window == window_) {
			if(inputManager_) {
				inputManager_->destroyInputObject(mouse_);
				inputManager_->destroyInputObject(keyboard_);
				OIS::InputManager::destroyInputSystem(inputManager_);
				inputManager_ = 0;
			}
		}
	}
	
	bool FrameListener::frameStarted(const Ogre::FrameEvent& evt) {
		Event event(Event::FRAME_START, evt, *keyboard_, *mouse_);
		keyboard_->capture();
		mouse_->capture();
		
		world_.onEvent(event);
		return true;
	}
	
	bool FrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt) {
		if(window_->isClosed())	{
			return false;
		}
		
		keyboard_->capture();
		mouse_->capture();
		
		assert(!keyboard_->buffered());
		
		if(keyboard_->isKeyDown(OIS::KC_ESCAPE) || keyboard_->isKeyDown(OIS::KC_Q)) {
			return false;
		}
		
		// Advance the animation.
		world_.getSceneManager().getEntity("thing")->getAnimationState("my_animation")->addTime(evt.timeSinceLastFrame);
		
		Event event(Event::FRAME_RENDERING, evt, *keyboard_, *mouse_);
		world_.onEvent(event);
		return true;
	}
	
	bool FrameListener::frameEnded(const Ogre::FrameEvent& evt) {		
		Ogre::SceneNode* busNode = world_.getSceneManager().getSceneNode("bus");
		busNode->translate(-0.1, 0.0, 0.0);
		
		keyboard_->capture();
		mouse_->capture();
		
		Event event(Event::FRAME_END, evt, *keyboard_, *mouse_);		
		world_.onEvent(event);
		return true;
	}
	
}


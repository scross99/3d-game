#include "FrameListener.hpp"

namespace Game3D {

	FrameListener::FrameListener(Ogre::RenderWindow* window, Ogre::Camera* camera, Ogre::SceneManager* sceneManager) :
		camera_(camera), sceneManager_(sceneManager),
		translateVector_(Ogre::Vector3::ZERO), currentSpeed_(0), window_(window),
		moveScale_(0.0f), rotateScale_(0.0f),
		moveSpeed_(200), rotateSpeed_(36),
		inputManager_(0), mouse_(0), keyboard_(0) {
		
		Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		
		window->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		
		inputManager_ = OIS::InputManager::createInputSystem(pl);
		
		const bool bufferedKeys = false, bufferedMouse = false;
		
		keyboard_ = static_cast<OIS::Keyboard*>(inputManager_->createInputObject(OIS::OISKeyboard, bufferedKeys));
		mouse_ = static_cast<OIS::Mouse*>(inputManager_->createInputObject(OIS::OISMouse, bufferedMouse));
		
		windowResized(window_);
		
		Ogre::WindowEventUtilities::addWindowEventListener(window_, this);
		
		cameraNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode();
		cameraNode_->setPosition(50, 50, 50);
		
		cameraYawNode_ = cameraNode_->createChildSceneNode();
		
		cameraPitchNode_ = cameraYawNode_->createChildSceneNode();
		
		cameraRollNode_ = cameraPitchNode_->createChildSceneNode();
		cameraRollNode_->attachObject(camera_);
	}
	
	FrameListener::~FrameListener() {
		// Remove ourself as a Window listener.
		Ogre::WindowEventUtilities::removeWindowEventListener(window_, this);
		windowClosed(window_);
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
	
	bool FrameListener::processUnbufferedKeyInput(const Ogre::FrameEvent& evt) {
		if(keyboard_->isKeyDown(OIS::KC_W)) {
			translateVector_.z = -(moveScale_);
		}
		
		if(keyboard_->isKeyDown(OIS::KC_S)) {
			translateVector_.z = moveScale_ * 0.25;
		}
		
		if(keyboard_->isKeyDown(OIS::KC_A)) {
			translateVector_.x = -(moveScale_ * 0.5);
		}
		
		if(keyboard_->isKeyDown(OIS::KC_D)) {
			translateVector_.x = moveScale_ * 0.5;
		}
		
		if(keyboard_->isKeyDown(OIS::KC_ESCAPE) || keyboard_->isKeyDown(OIS::KC_Q)) {
			return false;
		}
		
		return true;
	}
	
	bool FrameListener::processUnbufferedMouseInput(const Ogre::FrameEvent& evt) {
		const OIS::MouseState& ms = mouse_->getMouseState();
		
		rotX_ = Ogre::Degree(-ms.X.rel * 0.26);
		rotY_ = Ogre::Degree(-ms.Y.rel * 0.26);
		
		return true;
	}
	
	void FrameListener::moveCamera() {
		cameraYawNode_->yaw(rotX_);
		
		cameraPitchNode_->pitch(rotY_);
		
		cameraNode_->translate(cameraYawNode_->getOrientation() *
		                       translateVector_,
		                       Ogre::SceneNode::TS_LOCAL);
		                       
		// Angle of rotation around the X-axis.
		const Ogre::Real pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(cameraPitchNode_->getOrientation().w)).valueDegrees());
		
		// Determines the sign of the angle.
		const Ogre::Real pitchAngleSign = cameraPitchNode_->getOrientation().x;
		
		const double maxAngle = 60.0;
		
		// Limit the pitch.
		if(pitchAngle > maxAngle) {
			if(pitchAngleSign > 0) {
				cameraPitchNode_->setOrientation(Ogre::Quaternion(Ogre::Degree(maxAngle), Ogre::Vector3::UNIT_X));
			} else if(pitchAngleSign < 0) {
				cameraPitchNode_->setOrientation(Ogre::Quaternion(Ogre::Degree(-maxAngle), Ogre::Vector3::UNIT_X));
			}
		}
	}
	
	bool FrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt) {
		if(window_->isClosed())	{
			return false;
		}
		
		keyboard_->capture();
		mouse_->capture();
		
		const Ogre::Vector3 lastMotion = translateVector_;
		
		if(!mouse_->buffered() || !keyboard_->buffered()) {
			moveScale_ = moveSpeed_ * evt.timeSinceLastFrame;
			rotateScale_ = rotateSpeed_ * evt.timeSinceLastFrame;
			
			rotX_ = 0;
			rotY_ = 0;
			translateVector_ = Ogre::Vector3::ZERO;
		}
		
		if(!keyboard_->buffered())
			if(processUnbufferedKeyInput(evt) == false) {
				return false;
			}
			
		if(!mouse_->buffered())
			if(processUnbufferedMouseInput(evt) == false) {
				return false;
			}
			
		if(translateVector_ == Ogre::Vector3::ZERO) {
			currentSpeed_ -= evt.timeSinceLastFrame * 0.3;
			translateVector_ = lastMotion;
		} else {
			currentSpeed_ += evt.timeSinceLastFrame;
			
		}
		
		// Limit speed.
		if(currentSpeed_ > 1.0) {
			currentSpeed_ = 1.0;
		} else if(currentSpeed_ < 0.0) {
			currentSpeed_ = 0.0;
		}
		
		translateVector_ *= currentSpeed_;
		
		
		if(!mouse_->buffered() || !keyboard_->buffered()) {
			moveCamera();
		}
		
		return true;
	}
	
	bool FrameListener::frameEnded(const Ogre::FrameEvent& evt) {
		Ogre::SceneNode* node = sceneManager_->getSceneNode("ball0");
		node->roll(Ogre::Degree(1.0));
		
		const double PI = 3.141592654;
		const double radius = 25.0;
		node->translate(-((1.0 / 360.0) * 2.0 * PI * radius), 0.0, 0.0);
		
		Ogre::SceneNode* node1 = sceneManager_->getSceneNode("ball1");
		node1->pitch(Ogre::Degree(-1.0));
		
		node1->translate(0.0, 0.0, -((1.0 / 360.0) * 2.0 * PI * radius));
		
		Ogre::SceneNode* busNode = sceneManager_->getSceneNode("bus");
		busNode->translate(-0.1, 0.0, 0.0);
		return true;
	}
	
}


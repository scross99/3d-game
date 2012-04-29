#ifndef GAME3D_PLAYER_HPP
#define GAME3D_PLAYER_HPP

#include <Ogre.h>
#include "Camera.hpp"
#include "Node.hpp"
#include "Object.hpp"
#include "World.hpp"

namespace Game3D {

	class Player: public Object {
		private:
			Ogre::Vector3 translateVector_;
			Ogre::Real currentSpeed_;
			
			float moveScale_;
			float speedLimit_;
			Ogre::Degree rotateScale_;
			Ogre::Radian rotX_, rotY_;
			
			Ogre::Real moveSpeed_;
			Ogre::Degree rotateSpeed_;
			CameraPtr camera_;
	
		public:
			inline Player(CameraPtr camera)
				: translateVector_(Ogre::Vector3::ZERO), currentSpeed_(0),
				moveScale_(0.0f), rotateScale_(0.0f),
				moveSpeed_(200), rotateSpeed_(36),
				camera_(camera){ }
		
			inline void onEvent(Node& node, Event& event) {
				switch(event.type) {
					case Event::FRAME_RENDERING: {
						const Ogre::Vector3 lastMotion = translateVector_;
						
						if(!event.mouse.buffered() || !event.keyboard.buffered()) {
							moveScale_ = moveSpeed_ * event.frameEvent.timeSinceLastFrame;
							rotateScale_ = rotateSpeed_ * event.frameEvent.timeSinceLastFrame;
							
							rotX_ = 0;
							rotY_ = 0;
							translateVector_ = Ogre::Vector3::ZERO;
						}
						
						if(!event.keyboard.buffered()){
							processUnbufferedKeyInput(event);
						}
							
						if(!event.mouse.buffered()){
							processUnbufferedMouseInput(event);
						}
							
						if(translateVector_ == Ogre::Vector3::ZERO) {
							currentSpeed_ -= event.frameEvent.timeSinceLastFrame * 0.3;
							translateVector_ = lastMotion;
						} else {
							currentSpeed_ += event.frameEvent.timeSinceLastFrame;
							
						}
						
						// Limit speed.
						if(currentSpeed_ > 1.0) {
							currentSpeed_ = 1.0;
						} else if(currentSpeed_ < 0.0) {
							currentSpeed_ = 0.0;
						}
						
						translateVector_ *= currentSpeed_;
						
						if(!event.mouse.buffered() || !event.keyboard.buffered()) {
							moveCamera();
						}
							
						break;
					}
					default:
					{
						break;
					}
				}
			}
			
			inline void processUnbufferedKeyInput(Event& event) {
				OIS::Keyboard& keyboard = event.keyboard;
				
				if(keyboard.isKeyDown(OIS::KC_W)) {
					translateVector_.z = moveScale_;
				}
				
				if(keyboard.isKeyDown(OIS::KC_S)) {
					translateVector_.z = -(moveScale_ * 0.25);
				}
				
				if(keyboard.isKeyDown(OIS::KC_A)) {
					translateVector_.x = moveScale_ * 0.5;
				}
				
				if(keyboard.isKeyDown(OIS::KC_D)) {
					translateVector_.x = -(moveScale_ * 0.5);
				}
			}
			
			inline void processUnbufferedMouseInput(Event& event) {
				const OIS::MouseState& ms = event.mouse.getMouseState();
				
				rotX_ = Ogre::Degree(-ms.X.rel * 0.26);
				rotY_ = Ogre::Degree(-ms.Y.rel * 0.26);
			}
			
			inline void moveCamera() {
				camera_->rotate(AngleVector(-rotY_.valueDegrees(), rotX_.valueDegrees(), 0.0));
				
				camera_->translate(camera_->getOrientation().yawOrientation * translateVector_);
				
				const double maxPitch = 60.0;
				
				AngleVector rotation = camera_->getRotation();
				const double pitch = rotation.pitch;
				
				// Limit the pitch.
				if(fabs(pitch) > maxPitch) {
					if(pitch > 0.0) {
						rotation.pitch = maxPitch;
						camera_->setRotation(rotation);
					} else {
						rotation.pitch = -maxPitch;
						camera_->setRotation(rotation);
					}
				}
			}
			
	};
	
}

#endif

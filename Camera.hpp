#ifndef GAME3D_CAMERA_HPP
#define GAME3D_CAMERA_HPP

#include <string>

#include <Ogre.h>

#include "Vector.hpp"

namespace Game3D{

	struct CameraInfo{
		std::string name;
		double nearClipDistance, farClipDistance;
		Vector initialPosition;
		double initialPitch, initialYaw, initialRoll;
		double aspectRatio;
		Ogre::SceneNode * parentNode;
		
		inline CameraInfo()
			: name("camera"),
			nearClipDistance(0), farClipDistance(1000.0),
			initialPosition(0.0, 0.0, 0.0),
			initialPitch(0.0), initialYaw(0.0), initialRoll(0.0),
			aspectRatio(16.0/9.0),
			parentNode(0){ }
	};
	
	struct AngleVector{
		// Rotation around x axis.
		double pitch;
		
		// Rotation around y axis.
		double yaw;
		
		// Rotation around z axis.
		double roll;
		
		inline AngleVector(double p = 0.0, double y = 0.0, double r = 0.0)
			: pitch(p), yaw(y), roll(r){ }
	};
	
	struct OrientationVector{
		Ogre::Quaternion pitchOrientation, yawOrientation, rollOrientation;
	};
	
	class Camera{
		public:
			Camera(const CameraInfo& info, Ogre::SceneManager * sceneManager);
			
			Ogre::Camera * getCamera();
			
			void setAspectRatio(double aspectRatio);
			
			void rotate(const AngleVector& angles);
			
			void setRotation(const AngleVector& angles);
			
			AngleVector getRotation() const;
			
			OrientationVector getOrientation() const;
			
			void translate(const Vector& translateVector);
			
		private:
			Ogre::Camera* camera_;
			Ogre::SceneManager* sceneManager_;
			Ogre::SceneNode* cameraNode_;
			Ogre::SceneNode* cameraYawNode_;
			Ogre::SceneNode* cameraPitchNode_;
			Ogre::SceneNode* cameraRollNode_;
		
	};

}

#endif

#include "Camera.hpp"

namespace Game3D{

	Camera::Camera(const CameraInfo& info, Ogre::SceneManager * sceneManager, NodePtr node)
		: sceneManager_(sceneManager){
		
		camera_ = sceneManager_->createCamera(info.name);
		
		camera_->lookAt(Ogre::Vector3(0, 0, 1));
		camera_->setNearClipDistance(info.nearClipDistance);
		camera_->setFarClipDistance(info.farClipDistance);
		
		cameraNode_ = node->sceneNode.createChildSceneNode();
		cameraNode_->setPosition(info.initialPosition);
		
		cameraYawNode_ = cameraNode_->createChildSceneNode();
		
		cameraPitchNode_ = cameraYawNode_->createChildSceneNode();
		
		cameraRollNode_ = cameraPitchNode_->createChildSceneNode();
		cameraRollNode_->attachObject(camera_);
	}
			
	Ogre::Camera * Camera::getCamera(){
		return camera_;
	}
	
	void Camera::setAspectRatio(double aspectRatio){
		camera_->setAspectRatio(aspectRatio);
	}
	
	void Camera::rotate(const AngleVector& angles){
		cameraPitchNode_->pitch(Ogre::Degree(angles.pitch));
		cameraYawNode_->yaw(Ogre::Degree(angles.yaw));
		cameraRollNode_->roll(Ogre::Degree(angles.roll));
	}
			
	void Camera::setRotation(const AngleVector& angles){
		cameraPitchNode_->resetOrientation();
		cameraYawNode_->resetOrientation();
		cameraRollNode_->resetOrientation();
		rotate(angles);
	}
	
	Ogre::Vector3 Camera::getPosition() const{
		return cameraNode_->getPosition();
	}
			
	AngleVector Camera::getRotation() const{
		AngleVector rotation;
		rotation.pitch = cameraPitchNode_->getOrientation().getPitch().valueDegrees();
		rotation.yaw = cameraYawNode_->getOrientation().getYaw().valueDegrees();
		rotation.roll = cameraRollNode_->getOrientation().getRoll().valueDegrees();
		return rotation;
	}
	
	OrientationVector Camera::getOrientation() const{
		OrientationVector orientation;
		orientation.pitchOrientation = cameraPitchNode_->getOrientation();
		orientation.yawOrientation = cameraYawNode_->getOrientation();
		orientation.rollOrientation = cameraRollNode_->getOrientation();
		return orientation;
	}
	
	void Camera::translate(const Vector& translateVector){
		cameraNode_->translate(translateVector, Ogre::SceneNode::TS_LOCAL);
	}

}


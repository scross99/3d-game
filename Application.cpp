#include <math.h>

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <Ogre.h>
#include <OgreConfigFile.h>

#include "Application.hpp"
#include "Camera.hpp"
#include "FrameListener.hpp"
#include "Node.hpp"
#include "Object.hpp"
#include "Player.hpp"
#include "Resources.hpp"
#include "World.hpp"

namespace Game3D {

	double sqr(double v) {
		return v * v;
	}
	
	Ogre::SceneNode* CreateWall(Ogre::SceneManager* sceneManager, const std::string& materialName, Ogre::Vector3 p0, Ogre::Vector3 p1) {
		Ogre::Entity* entity = sceneManager->createEntity("square_mesh");
		Ogre::SceneNode* entityNode = sceneManager->getRootSceneNode()->createChildSceneNode();
		entityNode->attachObject(entity);
		entity->setMaterialName(materialName);
		entity->setCastShadows(true);
		entityNode->lookAt(Ogre::Vector3(p1.x - p0.x, 0.0, p1.z - p0.z).normalisedCopy(), Ogre::SceneNode::TS_LOCAL, Ogre::Vector3::UNIT_X);
		entityNode->translate(p0.x, p0.y, p0.z);
		entityNode->setScale(sqrt(sqr(p1.x - p0.x) + sqr(p1.z - p0.z)), fabs(p1.y - p0.y), 1.0);
		return entityNode;
	}
	
	Ogre::SceneNode* CreateFloor(Ogre::SceneManager* sceneManager, const std::string& materialName, Ogre::Vector3 p0, Ogre::Vector3 p1) {
		Ogre::Entity* entity = sceneManager->createEntity("floor");
		Ogre::SceneNode* entityNode = sceneManager->getRootSceneNode()->createChildSceneNode();
		entityNode->attachObject(entity);
		entity->setMaterialName(materialName);
		entityNode->translate(p0.x, p0.y, p1.z);
		entityNode->setScale(fabs(p1.x - p0.x), 1.0, fabs(p1.z - p0.z));
		entity->setCastShadows(false);
		return entityNode;
	}
	
	Ogre::SceneNode* AttachMesh(Ogre::SceneManager* sceneManager, Ogre::SceneNode* node, const std::string& parentName, const std::string& meshName) {
		Ogre::SceneNode* childNode = node->createChildSceneNode(parentName + "_" + meshName);
		childNode->attachObject(sceneManager->createEntity(meshName));
		return childNode;
	}
	
	Ogre::SceneNode* CreateBus(Ogre::SceneManager* sceneManager, const std::string& name) {
		Ogre::SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode(name);
		
		Ogre::SceneNode* busNode = node->createChildSceneNode();
		
		const std::string targetPath("./Media/bus");
		const boost::regex filter(".*\\.mesh");
		
		boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end.
		
		for(boost::filesystem::directory_iterator i(targetPath); i != end_itr; ++i) {
			// Skip if not a file.
			if(!boost::filesystem::is_regular_file(i->status())) {
				continue;
			}
			
			boost::smatch what;
			
			// Skip if no match.
			if(!boost::regex_match(i->leaf(), what, filter)) {
				continue;
			}
			
			// File matches.
			AttachMesh(sceneManager, busNode, name, i->leaf());
		}
		
		busNode->roll(Ogre::Degree(79.6));
		busNode->setPosition(Ogre::Vector3(0.0, 2.15, 0.0));
		
		return node;
	}
	
	Application::Application() {
		frameListener_ = 0;
		
		root_ = OGRE_NEW Ogre::Root(getResourcePath() + "plugins.cfg",
		                            getResourcePath() + "ogre.cfg", getResourcePath() + "Ogre.log");
	}
	
	Application::~Application() {
		if(frameListener_) {
			Ogre::WindowEventUtilities::removeWindowEventListener(window_, frameListener_);
			frameListener_->windowClosed(window_);
			delete frameListener_;
		}
		
		if(world_){
			delete world_;
		}
		
		if(root_) {
			OGRE_DELETE root_;
		}
	}
	
	void Application::go() {
		if(!setup()) {
			return;
		}
		
		const double MaxFPS = 30.0;
		
		while(true) {
			// Pump window events.
			Ogre::WindowEventUtilities::messagePump();
			
			if(!root_->renderOneFrame()) {
				break;
			}
			
			// Sleep to let the CPU relax.
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 / MaxFPS));
		}
	}
	
	bool Application::setup() {
		if(!root_->showConfigDialog()) {
			return false;
		}
		
		window_ = root_->initialise(true);
		
		sceneManager_ = root_->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
		
		world_ = new World(*sceneManager_);
		
		// Set default mipmap level.
		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);
		
		CameraInfo cameraInfo;
		cameraInfo.name = "camera";
		cameraInfo.nearClipDistance = 2.0;
		cameraInfo.initialPosition = Vector(50.0, 50.0, 50.0);
		
		NodePtr playerNode = world_->getRootNode()->addChild("player_node");
		
		CameraPtr camera(new Camera(cameraInfo, sceneManager_, playerNode));
		
		playerNode->object = ObjectPtr(new Player(camera));
		
		Ogre::Viewport* vp = window_->addViewport(camera->getCamera());
		vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
		
		camera->setAspectRatio((vp->getActualWidth() == 3840.0 ? 1920.0 : vp->getActualWidth()) / vp->getActualHeight());
		
		// Create the scene
		createScene();
		
		frameListener_ = new FrameListener(window_, *world_);
		root_->addFrameListener(frameListener_);
		frameListener_->windowResized(window_);
		
		Ogre::WindowEventUtilities::addWindowEventListener(window_, frameListener_);
		
		return true;
		
	}
	
	void Application::createScene() {
		{
			Ogre::ManualObject* manual = sceneManager_->createManualObject();
			
			manual->setDynamic(false);
			
			manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
			
			manual->position(0.0, 0.0, 0.0);
			manual->normal(0.0, 0.0, 1.0);
			manual->textureCoord(0.0, 1.0);
			
			manual->position(1.0, 0.0, 0.0);
			manual->normal(0.0, 0.0, 1.0);
			manual->textureCoord(1.0, 1.0);
			
			manual->position(0.0, 1.0, 0.0);
			manual->normal(0.0, 0.0, 1.0);
			manual->textureCoord(0.0, 0.0);
			
			manual->position(1.0, 1.0, 0.0);
			manual->normal(0.0, 0.0, 1.0);
			manual->textureCoord(1.0, 0.0);
			
			manual->index(0);
			manual->index(1);
			manual->index(2);
			manual->index(3);
			
			manual->end();
			
			manual->convertToMesh("square_mesh");
		}
		
		{
			Ogre::ManualObject* manual = sceneManager_->createManualObject();
			
			manual->setDynamic(false);
			
			manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
			
			manual->position(0.0, 0.0, 0.0);
			manual->textureCoord(0.0, 1.0);
			manual->normal(0.0, 1.0, 0.0);
			
			manual->position(1.0, 0.0, 0.0);
			manual->textureCoord(1.0, 1.0);
			manual->normal(0.0, 1.0, 0.0);
			
			manual->position(0.0, 0.0, -1.0);
			manual->textureCoord(0.0, 0.0);
			manual->normal(0.0, 1.0, 0.0);
			
			manual->position(1.0, 0.0, -1.0);
			manual->textureCoord(1.0, 0.0);
			manual->normal(0.0, 1.0, 0.0);
			
			manual->index(0);
			manual->index(1);
			manual->index(2);
			manual->index(3);
			
			manual->end();
			
			manual->convertToMesh("floor");
		}
		
		sceneManager_->setAmbientLight(Ogre::ColourValue(0, 0, 0));
		sceneManager_->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		
		{
			Ogre::Light* pointLight = sceneManager_->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(200.0, 100.0, 200.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		{
			Ogre::Light* pointLight = sceneManager_->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(-200.0, 100.0, -200.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		{
			Ogre::Light* pointLight = sceneManager_->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(-200.0, 200.0, 200.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		{
			Ogre::Light* pointLight = sceneManager_->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(0.0, 100.0, 450.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		Ogre::Light* directionLight = sceneManager_->createLight("directional_light");
		directionLight->setType(Ogre::Light::LT_DIRECTIONAL);
		directionLight->setDiffuseColour(0.5, 0.5, 0.5);
		directionLight->setSpecularColour(0.7, 0.7, 0.7);
		directionLight->setDirection(Ogre::Vector3(0, -1, 1));
		
		CreateWall(sceneManager_, "brick_wall", Ogre::Vector3(0.0, 0.0, 0.0), Ogre::Vector3(100.0, 100.0, 0.0));
		CreateWall(sceneManager_, "brick_wall", Ogre::Vector3(100.0, 0.0, 0.0), Ogre::Vector3(200.0, 100.0, 0.0));
		CreateWall(sceneManager_, "brick_wall", Ogre::Vector3(0.0, 0.0, 100.0), Ogre::Vector3(0.0, 100.0, 0.0));
		
		for(int i = -10; i < 0; i++) {
			for(int j = -10; j < 10; j++) {
				CreateFloor(sceneManager_, "mud", Ogre::Vector3(i * 100.0, 0.0, j * 100.0), Ogre::Vector3((i + 1) * 100.0, 0.0, (j + 1) * 100.0));
			}
		}
		
		for(int i = 0; i < 10; i++) {
			for(int j = -10; j < 10; j++) {
				CreateFloor(sceneManager_, "concrete_floor", Ogre::Vector3(i * 100.0, 0.0, j * 100.0), Ogre::Vector3((i + 1) * 100.0, 0.0, (j + 1) * 100.0));
			}
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(sceneManager_, "brick_wall", Ogre::Vector3(i * 100.0, 0.0, -1000.0), Ogre::Vector3((i + 1) * 100.0, 100.0, -1000.0));
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(sceneManager_, "brick_wall", Ogre::Vector3((i + 1) * 100.0, 0.0, 1000.0), Ogre::Vector3(i * 100.0, 100.0, 1000.0));
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(sceneManager_, "brick_wall", Ogre::Vector3(-1000.0, 0.0, (i + 1) * 100.0), Ogre::Vector3(-1000.0, 100.0, i * 100.0));
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(sceneManager_, "brick_wall", Ogre::Vector3(1000.0, 0.0, i * 100.0), Ogre::Vector3(1000.0, 100.0, (i + 1) * 100.0));
		}
		
		{
			Ogre::SceneNode* sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode("ball0");
			Ogre::Entity* entity_ = sceneManager_->createEntity(Ogre::SceneManager::PT_SPHERE);
			entity_->setMaterialName("concrete_floor");
			sceneNode_->attachObject(entity_);
			sceneNode_->setPosition(Ogre::Vector3(200.0, 25.0, 200.0));
			sceneNode_->setScale(Ogre::Vector3(0.5, 0.5, 0.5)); // Radius, in theory.
			entity_->setCastShadows(true);
		}
		
		{
			Ogre::SceneNode* sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode("ball1");
			Ogre::Entity* entity_ = sceneManager_->createEntity(Ogre::SceneManager::PT_SPHERE);
			entity_->setMaterialName("concrete_floor");
			sceneNode_->attachObject(entity_);
			sceneNode_->setPosition(Ogre::Vector3(200.0, 25.0, 250.0));
			sceneNode_->setScale(Ogre::Vector3(0.5, 0.5, 0.5)); // Radius, in theory.
			entity_->setCastShadows(true);
		}
		
		{
			Ogre::SceneNode* sceneNode_ = sceneManager_->getRootSceneNode()->createChildSceneNode("planet");
			Ogre::Entity* entity_ = sceneManager_->createEntity(Ogre::SceneManager::PT_SPHERE);
			entity_->setMaterialName("concrete_floor");
			sceneNode_->attachObject(entity_);
			sceneNode_->setPosition(Ogre::Vector3(0.0, 5000.0, 0.0));
			sceneNode_->setScale(Ogre::Vector3(10.0, 10.0, 10.0)); // Radius, in theory.
			entity_->setCastShadows(true);
		}
		
		Ogre::SceneNode* thingNode = sceneManager_->getRootSceneNode()->createChildSceneNode("thing");
		Ogre::Entity* thingEntity = sceneManager_->createEntity("thing", "Character.mesh");
		Ogre::AnimationState* animation = thingEntity->getAnimationState("my_animation");
		animation->setLoop(true);
		animation->setEnabled(true);
		
		thingNode->attachObject(thingEntity);
		thingNode->setScale(Ogre::Vector3(10.0, 10.0, 10.0));
		thingNode->translate(0.0, 50.0, -500.0);
		
		Ogre::SceneNode* busNode = CreateBus(sceneManager_, "bus");
		busNode->setScale(Ogre::Vector3(40.0, 40.0, 40.0));
		busNode->translate(0.0, 0.0, 500.0);
	}
	
}


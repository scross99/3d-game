#include <math.h>

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>

#include <Ogre.h>
#include <OgreConfigFile.h>

#include "Application.hpp"
#include "FrameListener.hpp"
#include "Resources.hpp"

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
		mFrameListener = 0;
		mRoot = 0;
		mResourcePath = getResourcePath();
	}
	
	Application::~Application() {
		if(mFrameListener) {
			delete mFrameListener;
		}
		
		if(mRoot) {
			OGRE_DELETE mRoot;
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
			
			if(!mRoot->renderOneFrame()) {
				break;
			}
			
			// Sleep to let the CPU relax.
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000.0 / MaxFPS));
		}
		
		destroyScene();
	}
	
	bool Application::setup() {
	
		Ogre::String pluginsPath;
		// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
		pluginsPath = mResourcePath + "plugins.cfg";
#endif
		
		mRoot = OGRE_NEW Ogre::Root(pluginsPath,
		                            mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");
		                            
		setupResources();
		
		if(!mRoot->showConfigDialog()) {
			return false;
		}
		
		mWindow = mRoot->initialise(true);
		
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
		
		mCamera = mSceneMgr->createCamera("camera");
		
		mCamera->lookAt(Ogre::Vector3(0, 0, -1));
		mCamera->setNearClipDistance(2);
		
		Ogre::Viewport* vp = mWindow->addViewport(mCamera);
		vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
		
		mCamera->setAspectRatio((vp->getActualWidth() == 3840.0 ? 1920.0 : vp->getActualWidth()) / vp->getActualHeight());
		
		// Set default mipmap level (NB some APIs ignore this)
		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		
		// Create any resource listeners (for loading screens)
		createResourceListener();
		
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		
		// Create the scene
		createScene();
		
		mFrameListener = new FrameListener(mWindow, mCamera, mSceneMgr);
		mRoot->addFrameListener(mFrameListener);
		
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);
		
		return true;
		
	}
	
	void Application::createScene() {
		{
			Ogre::ManualObject* manual = mSceneMgr->createManualObject();
			
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
			Ogre::ManualObject* manual = mSceneMgr->createManualObject();
			
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
		
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		
		{
			Ogre::Light* pointLight = mSceneMgr->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(200.0, 100.0, 200.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		{
			Ogre::Light* pointLight = mSceneMgr->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(-200.0, 100.0, -200.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		{
			Ogre::Light* pointLight = mSceneMgr->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(-200.0, 200.0, 200.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		{
			Ogre::Light* pointLight = mSceneMgr->createLight();
			pointLight->setType(Ogre::Light::LT_POINT);
			pointLight->setPosition(Ogre::Vector3(0.0, 100.0, 450.0));
			pointLight->setDiffuseColour(0.8, 0.8, 0.8);
			pointLight->setSpecularColour(0.9, 0.9, 0.9);
		}
		
		Ogre::Light* directionLight = mSceneMgr->createLight("directional_light");
		directionLight->setType(Ogre::Light::LT_DIRECTIONAL);
		directionLight->setDiffuseColour(0.5, 0.5, 0.5);
		directionLight->setSpecularColour(0.7, 0.7, 0.7);
		directionLight->setDirection(Ogre::Vector3(0, -1, 1));
		
		CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3(0.0, 0.0, 0.0), Ogre::Vector3(100.0, 100.0, 0.0));
		CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3(100.0, 0.0, 0.0), Ogre::Vector3(200.0, 100.0, 0.0));
		CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3(0.0, 0.0, 100.0), Ogre::Vector3(0.0, 100.0, 0.0));
		
		for(int i = -10; i < 0; i++) {
			for(int j = -10; j < 10; j++) {
				CreateFloor(mSceneMgr, "mud", Ogre::Vector3(i * 100.0, 0.0, j * 100.0), Ogre::Vector3((i + 1) * 100.0, 0.0, (j + 1) * 100.0));
			}
		}
		
		for(int i = 0; i < 10; i++) {
			for(int j = -10; j < 10; j++) {
				CreateFloor(mSceneMgr, "concrete_floor", Ogre::Vector3(i * 100.0, 0.0, j * 100.0), Ogre::Vector3((i + 1) * 100.0, 0.0, (j + 1) * 100.0));
			}
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3(i * 100.0, 0.0, -1000.0), Ogre::Vector3((i + 1) * 100.0, 100.0, -1000.0));
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3((i + 1) * 100.0, 0.0, 1000.0), Ogre::Vector3(i * 100.0, 100.0, 1000.0));
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3(-1000.0, 0.0, (i + 1) * 100.0), Ogre::Vector3(-1000.0, 100.0, i * 100.0));
		}
		
		for(int i = -10; i < 10; i++) {
			CreateWall(mSceneMgr, "brick_wall", Ogre::Vector3(1000.0, 0.0, i * 100.0), Ogre::Vector3(1000.0, 100.0, (i + 1) * 100.0));
		}
		
		{
			Ogre::SceneNode* mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ball0");
			Ogre::Entity* mEntity = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
			mEntity->setMaterialName("concrete_floor");
			mSceneNode->attachObject(mEntity);
			mSceneNode->setPosition(Ogre::Vector3(200.0, 25.0, 200.0));
			mSceneNode->setScale(Ogre::Vector3(0.5, 0.5, 0.5)); // Radius, in theory.
			mEntity->setCastShadows(true);
		}
		
		{
			Ogre::SceneNode* mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ball1");
			Ogre::Entity* mEntity = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
			mEntity->setMaterialName("concrete_floor");
			mSceneNode->attachObject(mEntity);
			mSceneNode->setPosition(Ogre::Vector3(200.0, 25.0, 250.0));
			mSceneNode->setScale(Ogre::Vector3(0.5, 0.5, 0.5)); // Radius, in theory.
			mEntity->setCastShadows(true);
		}
		
		{
			Ogre::SceneNode* mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("planet");
			Ogre::Entity* mEntity = mSceneMgr->createEntity(Ogre::SceneManager::PT_SPHERE);
			mEntity->setMaterialName("concrete_floor");
			mSceneNode->attachObject(mEntity);
			mSceneNode->setPosition(Ogre::Vector3(0.0, 5000.0, 0.0));
			mSceneNode->setScale(Ogre::Vector3(10.0, 10.0, 10.0)); // Radius, in theory.
			mEntity->setCastShadows(true);
		}
		
		Ogre::SceneNode* busNode = CreateBus(mSceneMgr, "bus");
		busNode->setScale(Ogre::Vector3(40.0, 40.0, 40.0));
		busNode->translate(0.0, 0.0, 500.0);
	}
	
	void Application::destroyScene() {}
	
	void Application::setupResources() {
		// Load resource paths from config file.
		Ogre::ConfigFile cf;
		cf.load(mResourcePath + "resources.cfg");
		
		// Go through all sections & settings in the file.
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
		
		Ogre::String secName, typeName, archName;
		
		while(seci.hasMoreElements()) {
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			
			for(i = settings->begin(); i != settings->end(); ++i) {
				typeName = i->first;
				archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
				// OS X does not set the working directory relative to the app,
				// In order to make things portable on OS X we need to provide
				// the loading with it's own bundle path location
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				    String(macBundlePath() + "/" + archName), typeName, secName);
#else
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				    archName, typeName, secName);
#endif
			}
		}
	}
	
	void Application::createResourceListener() {
	
	}
	
}


#ifndef GAME3D_WORLD_HPP
#define GAME3D_WORLD_HPP

#include <boost/shared_ptr.hpp>
#include <Ogre.h>
#include "Node.hpp"
#include "Object.hpp"

namespace Game3D {

	class World{
		public:
			inline World(Ogre::SceneManager& sceneManager)
				: sceneManager_(sceneManager),
				rootNode_(
					new Node(
						ObjectPtr(),
						*sceneManager_.getRootSceneNode()->createChildSceneNode()
					)
				){ }
			
			inline NodePtr getRootNode(){
				return rootNode_;
			}
			
			inline Ogre::SceneManager& getSceneManager(){
				return sceneManager_;
			}
			
			inline void onEvent(Event& event){
				rootNode_->onEvent(event);
			}
			
		private:
			Ogre::SceneManager& sceneManager_;
			NodePtr rootNode_;
		
	};

}

#endif

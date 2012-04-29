#ifndef GAME3D_NODE_HPP
#define GAME3D_NODE_HPP

#include <map>
#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <Ogre.h>
#include "Object.hpp"

namespace Game3D {

	struct Node;
	typedef boost::shared_ptr<Node> NodePtr;
	
	class Node {
		private:
			ObjectPtr object_;
			Ogre::SceneNode* sceneNode_;
			std::map<std::string, NodePtr> children_;
			
		public:
			inline Node() { }
			
			inline Node(ObjectPtr object, Ogre::SceneNode& sceneNode)
				: object_(object), sceneNode_(&sceneNode) { }
				
			inline NodePtr createChild(const std::string& name) {
				NodePtr newNode(new Node(ObjectPtr(), *(sceneNode_->createChildSceneNode())));
				attachChild(name, newNode);
				return newNode;
			}
			
			inline void attachChild(const std::string& name, const NodePtr& node) {
				typedef std::map<std::string, NodePtr>::iterator ItType;
				std::size_t i = 0;
				std::string childName(name);
				
				while(true) {
					std::pair<ItType, bool> result = children_.insert(std::make_pair(childName, node));
					
					if(result.second) {
						return;
					}
					
					std::ostringstream stream;
					stream << name << (i++);
					childName = stream.str();
				}
			}
			
			inline NodePtr getChild(const std::string& name) {
				typedef std::map<std::string, NodePtr>::iterator ItType;
				ItType it = children_.find(name);
				assert(it != children_.end());
				return it->second;
			}
			
			inline Ogre::SceneNode& getSceneNode() {
				return *sceneNode_;
			}
			
			inline void setObject(ObjectPtr object) {
				object_ = object;
			}
			
			inline void onEvent(Event& event) {
				if(object_) {
					object_->onEvent(*this, event);
				}
				
				typedef std::map<std::string, NodePtr>::iterator ItType;
				
				for(ItType it = children_.begin(); it != children_.end(); ++it) {
					it->second->onEvent(event);
				}
			}
	};
	
}

#endif

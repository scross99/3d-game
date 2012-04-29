#ifndef GAME3D_NODE_HPP
#define GAME3D_NODE_HPP

#include <map>
#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <Ogre.h>
#include "Object.hpp"

namespace Game3D{

	struct Node;
	typedef boost::shared_ptr<Node> NodePtr;
	
	struct Node{
		ObjectPtr object;
		Ogre::SceneNode& sceneNode;
		std::map<std::string, NodePtr> children;
		
		inline Node(ObjectPtr o, Ogre::SceneNode& s)
			: object(o), sceneNode(s){ }
			
		inline NodePtr addChild(const std::string& name){
			typedef std::map<std::string, NodePtr>::iterator ItType;
			NodePtr newNode(new Node(ObjectPtr(), *(sceneNode.createChildSceneNode())));
			std::size_t i = 0;
			std::string childName(name);
			
			while(true){
				std::pair<ItType, bool> result = children.insert(std::make_pair(childName, newNode));
				if(result.second){
					return newNode;
				}
				
				std::ostringstream stream;
				stream << name << (i++);
				childName = stream.str();
			}
		}
		
		inline NodePtr getChild(const std::string& name){
			typedef std::map<std::string, NodePtr>::iterator ItType;
			ItType it = children.find(name);
			assert(it != children.end());
			return it->second;
		}
		
		inline void onEvent(Event& event){
			if(object) object->onEvent(*this, event);
			
			typedef std::map<std::string, NodePtr>::iterator ItType;
			for(ItType it = children.begin(); it != children.end(); ++it){
				it->second->onEvent(event);
			}
		}
	};

}

#endif

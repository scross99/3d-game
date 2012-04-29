#ifndef GAME3D_OBJECT_HPP
#define GAME3D_OBJECT_HPP

#include <boost/shared_ptr.hpp>
#include <Ogre.h>
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

namespace Game3D {

	struct Node;
	
	struct Event{
		enum Type{
			FRAME_START,
			FRAME_END,
			FRAME_RENDERING,
			KEY_PRESSED,
			KEY_RELEASED
		} type;
		
		const Ogre::FrameEvent& frameEvent;
		OIS::Keyboard& keyboard;
		OIS::Mouse& mouse;
		
		inline Event(Type t, const Ogre::FrameEvent& f, OIS::Keyboard& k, OIS::Mouse& m)
			: type(t), frameEvent(f), keyboard(k), mouse(m){ }
	};

	class Object{
		public:
			virtual void onEvent(Node& node, Event& event) = 0;
			
			virtual ~Object(){ }
		
	};
	
	typedef boost::shared_ptr<Object> ObjectPtr;

}

#endif

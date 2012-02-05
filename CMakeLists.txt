cmake_minimum_required (VERSION 2.8)

project(Game3D)

file(COPY Media ogre.cfg plugins.cfg resources.cfg DESTINATION .)

if(WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif(WIN32)
 
if(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

	elseif(EXISTS "/usr/lib/OGRE/cmake")

	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")

	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX)

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)

SET(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-g -Wall")

include_directories(${OIS_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS})

add_executable(game3D main.cpp Application.cpp Camera.cpp FrameListener.cpp Resources.cpp)
target_link_libraries(game3D ${OGRE_LIBRARIES} ${OIS_LIBRARIES} boost_filesystem boost_regex boost_thread boost_system)


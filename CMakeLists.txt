project(ShipRace)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/ext)
#file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)
#file(GLOB_RECURSE HEADER ${CMAKE_SOURCE_DIR}/*.h)
set(SOURCES main.cpp Camera.cpp Water.cpp GerstnerWater.cpp Mesh.cpp Renderer.cpp Entity.cpp BuoyantEntity.cpp)
set(HEADER Camera.h Water.h GerstnerWater.h Mesh.h Renderer.h Entity.h BuoyantEntity.h)
file(GLOB_RECURSE VERTEXSHADER ./Shader/*.vert)
file(GLOB_RECURSE FRAGMENTSHADER ./Shader/*.frag)

if (NOT APPLE)

find_package(assimp REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})
link_directories(${ASSIMP_LIBRARY_DIRS})


find_package(glfw REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER} ${VERTEXSHADER} ${FRAGMENTSHADER})
target_link_libraries (${PROJECT_NAME}  IL ${GLEW_LIBRARY} GL glfw2 ${ASSIMP_LIBRARIES} boost_system GLU)
ENDIF()

SET(Eigen_DIR /usr/include/eigen3 /usr/local/include/eigen3 ${CMAKE_SOURCE_DIR}/ext/eigen3)

include_directories(${Eigen_DIR})


IF(APPLE)

   find_library(COCOA_LIBRARY Cocoa)
   find_library(GLUT_LIBRARY GLUT )
   find_library(OpenGL_LIBRARY OpenGL )
   find_library(IOKIT NAMES IOKit)
   MARK_AS_ADVANCED (COCOA_LIBRARY GLUT_LIBRARY OpenGL_LIBRARY)

   SET(EXTRA_LIBS 
	${COCOA_LIBRARY} 
	${GLUT_LIBRARY} 
	${OpenGL_LIBRARY} 
	${IOKIT}
	${CMAKE_SOURCE_DIR}/MacLibs/libglfw.a
	${CMAKE_SOURCE_DIR}/MacLibs/libGLEW.a
	${CMAKE_SOURCE_DIR}/MacLibs/libboost_system-mt.a
	${CMAKE_SOURCE_DIR}/MacLibs/libboost_thread-mt.a)

   add_library( assimp STATIC IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.a )

   add_library( devil STATIC IMPORTED )
   set_target_properties( devil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libIL.a )

   add_library( dyassimp SHARED IMPORTED )
   set_target_properties( assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libassimp.3.0.0.dylib )

   add_library( dydevil SHARED IMPORTED )
   set_target_properties( devil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MacLibs/libIL.1.dylib )

   		
   target_link_libraries(ShipRace assimp)
   target_link_libraries(ShipRace devil)
   
    set(EXT_LIBS ${CMAKE_SOURCE_DIR}/MacLibs)
    target_link_libraries(ShipRace ${EXTRA_LIBS})
endif()

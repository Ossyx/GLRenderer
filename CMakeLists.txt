cmake_minimum_required(VERSION 2.8.9)
project (GLRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)
endif()

#dependencies
find_package(GLFW3 REQUIRED)
#CImg is a dependency
find_package(CImg REQUIRED)
#glm is a dependency
find_package(glm REQUIRED)
#glad is a dependency
find_package(glad CONFIG REQUIRED)
#JSon is a dependency
find_package(JsonCpp REQUIRED)
#MeshLib is a dependency
find_package(MeshLib REQUIRED)
#libPNG for CImg to have png full support
find_package(libpng REQUIRED)
#libJPEG for CImg to have jpeg full support
find_package(JPEG REQUIRED)
#Assimp is a dependency
find_package(Assimp REQUIRED)
find_package(FFTWater REQUIRED)

set(GLFW_LIBRARY glfw)
set(Glad_LIBRARIES glad::glad)
set(JsonCpp_LIBRARY jsoncpp_object)
get_target_property(GLAD_INCLUDE_DIR glad::glad INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(GLFW_INCLUDE_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(GLM_INCLUDE_DIRS glm::glm INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(JsonCpp_INCLUDE_DIR jsoncpp_object INTERFACE_INCLUDE_DIRECTORIES) 
message("GLAD INCLUDE DIR :" ${GLAD_INCLUDE_DIR})
message("GLFW3 INCLUDE DIR :" ${GLFW_INCLUDE_DIR})
message("GLM INCLUDE DIR :" ${GLM_INCLUDE_DIRS})
message("JSONCPP INCLUDE DIR :" ${JsonCpp_INCLUDE_DIR})

#Exclude libnoise from windows build for now
if(NOT WIN32)
  find_package(LibNoise CONFIG REQUIRED)
endif()

set(IMGUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/imgui;${CMAKE_SOURCE_DIR}/imgui/backends")


option(MESHLIB_INSTALL_DIR "MeshLib installation directory" "")
set(MESHLIB_INCLUDE_DIR "${MESHLIB_INSTALL_DIR}/include/MeshLib")

if(NOT WIN32)
  set(MESHLIB_LIBRARY "${MESHLIB_INSTALL_DIR}/lib/libMeshLib.so")
else()
  #add_library(MeshLib SHARED IMPORTED)
  #set_property(TARGET MeshLib APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
  #set_target_properties(MeshLib PROPERTIES
  #  IMPORTED_IMPLIB_DEBUG "${MESHLIB_INSTALL_DIR}/lib/MeshLib.lib"
  #  IMPORTED_LOCATION_DEBUG "${MESHLIB_INSTALL_DIR}/bin/MeshLib.dll"
  #)
  set(MESHLIB_LIBRARY MeshLib)
endif()

message("MEHSH LIB : " ${MESHLIB_LIBRARY})
set(OCEANSURFACE_INCLUDE "C:/lib/include/OceanSurface")
if(NOT WIN32)
  set(OCEANSURFACE_LIBRARY "${MESHLIB_INSTALL_DIR}/lib/libOceanSurface.so")
else()
  set(OCEANSURFACE_LIBRARY OceanSurface)
endif()

#Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)

file(GLOB_RECURSE srcs
  ${CMAKE_SOURCE_DIR}/src/*.cxx
  ${CMAKE_SOURCE_DIR}/include/*.hxx)

if(NOT WIN32)
  add_custom_target(VeraStyle ALL
    vera++
    --warning
    ${srcs})
endif()

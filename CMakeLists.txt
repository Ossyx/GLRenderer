cmake_minimum_required(VERSION 2.8.9)
project (GLRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

#dependencies
find_package(GLFW REQUIRED)
#CImg is a dependency
find_package(CImg REQUIRED)
#glm is a dependency
find_package(glm REQUIRED)
#glad is a dependency
find_package(GLAD REQUIRED)
#JSon is a dependency
find_package(JsonCpp REQUIRED)

find_package(LibNoise)

set(IMGUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/imgui")


option(MESHLIB_INSTALL_DIR "MeshLib installation directory" "")

set(MESHLIB_INCLUDE_DIR "${MESHLIB_INSTALL_DIR}/include")
set(MESHLIB_LIBRARY "${MESHLIB_INSTALL_DIR}/lib/libMeshLib.so")

set(OCEANSURFACE_INCLUDE "/home/bertrand/Work/FFTWater/OceanSurface")
set(OCEANSURFACE_LIBRARY "/home/bertrand/Work/FFTWater/build/libOceanSurface.so")

#Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)

file(GLOB_RECURSE srcs
  ${CMAKE_SOURCE_DIR}/src/*.cxx
  ${CMAKE_SOURCE_DIR}/include/*.hxx)
add_custom_target(VeraStyle ALL
  vera++
  --warning
  ${srcs})

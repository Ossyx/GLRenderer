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

include_directories(${CMAKE_SOURCE_DIR}/include/common
${CMAKE_SOURCE_DIR}/include/resources
${CMAKE_SOURCE_DIR}/include/event
${CMAKE_SOURCE_DIR}/include/gui
${CMAKE_SOURCE_DIR}/include/render
${CMAKE_SOURCE_DIR}/include/scene
${CMAKE_SOURCE_DIR}/include/terrain
${CMAKE_SOURCE_DIR}/include/resources
${GLFW_INCLUDE_DIR}
${Glad_INCLUDE_DIR}
${GLM_INCLUDE_DIRS}
${MESHLIB_INCLUDE_DIR}
${OCEANSURFACE_INCLUDE}
${CImg_INCLUDE_DIR}
${LIBNOISE_INCLUDE_DIR}
${IMGUI_INCLUDE_DIR}
${JsonCpp_INCLUDE_DIR})

#Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

#Libraries
add_library(Common SHARED
  ${CMAKE_SOURCE_DIR}/src/common/Shader.cxx
  ${CMAKE_SOURCE_DIR}/src/common/MathTools.cxx)
  
add_library(Resource SHARED
  ${CMAKE_SOURCE_DIR}/src/resources/ResourceDescription.cxx
  ${CMAKE_SOURCE_DIR}/src/resources/ResourcesHolder.cxx
  ${CMAKE_SOURCE_DIR}/src/resources/ResourcesLoader.cxx)
  
add_library(Scene SHARED
  ${CMAKE_SOURCE_DIR}/src/scene/Camera.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/FrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/SceneGraph.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/SceneGraphLoader.cxx)
  
add_library(Event SHARED
  ${CMAKE_SOURCE_DIR}/src/event/EventDispatcher.cxx
  ${CMAKE_SOURCE_DIR}/src/event/EventInterface.cxx)

add_executable(GLRenderer
  ${CMAKE_SOURCE_DIR}/src/main.cxx
  ${CMAKE_SOURCE_DIR}/src/render/DrawableItem.cxx
  ${CMAKE_SOURCE_DIR}/src/render/EnvironmentMap.cxx
  ${CMAKE_SOURCE_DIR}/src/render/SceneRenderer.cxx
  ${CMAKE_SOURCE_DIR}/src/render/GeometryHandle.cxx
  ${CMAKE_SOURCE_DIR}/src/render/MaterialTexturesHandle.cxx
  ${CMAKE_SOURCE_DIR}/src/render/Renderable.cxx
  ${CMAKE_SOURCE_DIR}/src/render/SimpleRenderer.cxx
  ${CMAKE_SOURCE_DIR}/src/render/GLAbstraction.cxx
  ${CMAKE_SOURCE_DIR}/src/render/Postprocess.cxx
  ${CMAKE_SOURCE_DIR}/src/terrain/TerrainLOD.cxx
  ${CMAKE_SOURCE_DIR}/src/gui/TerrainGenGui.cxx
  ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp)

add_executable(TestFrustumCulling
  ${CMAKE_SOURCE_DIR}/test/testFrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/FrustumCulling.cxx)

add_executable(testResourceLoader
  ${CMAKE_SOURCE_DIR}/src/tests/resourceLoaderTest.cxx)
  
add_executable(commonTests
  ${CMAKE_SOURCE_DIR}/src/tests/commonTests.cxx)
  
add_executable(sceneGraphLoaderTest
  ${CMAKE_SOURCE_DIR}/src/tests/sceneGraphLoaderTest.cxx)

add_executable(generateHeightmap
  ${CMAKE_SOURCE_DIR}/test/generateHeightmap.cxx
  ${CMAKE_SOURCE_DIR}/src/terrain/HeightmapGenerator.cxx)
  
add_executable(testSimpleRenderer
  ${CMAKE_SOURCE_DIR}/test/simpleRendererTest.cxx
  ${CMAKE_SOURCE_DIR}/src/render/GLAbstraction.cxx
  ${CMAKE_SOURCE_DIR}/src/render/DrawableItem.cxx
  ${CMAKE_SOURCE_DIR}/src/render/GeometryHandle.cxx
  ${CMAKE_SOURCE_DIR}/src/render/MaterialTexturesHandle.cxx
  ${CMAKE_SOURCE_DIR}/src/render/Renderable.cxx
  ${CMAKE_SOURCE_DIR}/src/render/EnvironmentMap.cxx
  ${CMAKE_SOURCE_DIR}/src/render/SimpleRenderer.cxx
  ${CMAKE_SOURCE_DIR}/src/render/Postprocess.cxx)


target_compile_definitions(GLRenderer PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(Common
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
)

target_link_libraries(Event
${GLFW_LIBRARY}
)

target_link_libraries(Resource
Common
${MESHLIB_LIBRARY}
${JsonCpp_LIBRARY}
)

target_link_libraries(Scene
Common
Event
Resource
${JsonCpp_LIBRARY}
)

#CMAKE_DL_LIBS is needed by glad
target_link_libraries(GLRenderer
${GLFW_LIBRARY}
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
${CImg_LIBRARIES}
${OCEANSURFACE_LIBRARY}
Common
Resource
Scene
Event)

target_link_libraries(testSimpleRenderer
${GLFW_LIBRARY}
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
${CImg_LIBRARIES}
Common
Resource
Scene
Event)

target_link_libraries(TestFrustumCulling
${MESHLIB_LIBRARY})

target_link_libraries(generateHeightmap
${CImg_LIBRARIES}
${LIBNOISE_LIBRARIES})

target_link_libraries(testResourceLoader
Common
Resource
)

target_link_libraries(commonTests
Common
)

target_link_libraries(sceneGraphLoaderTest
Common
Scene
Resource
)

file(GLOB_RECURSE srcs
  ${CMAKE_SOURCE_DIR}/src/*.cxx
  ${CMAKE_SOURCE_DIR}/include/*.hxx)
add_custom_target(VeraStyle ALL
  vera++
  --warning
  ${srcs})

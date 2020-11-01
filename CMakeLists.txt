cmake_minimum_required(VERSION 2.8.9)
project (GLRenderer)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

#dependencies
find_package(GLFW REQUIRED)
#CImg is a dependency
find_package(CImg REQUIRED)
#glm is a dependency
find_package(glm REQUIRED)
#glad is a dependency
find_package(GLAD REQUIRED)

find_package(LibNoise)

set(IMGUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/imgui")


option(MESHLIB_INSTALL_DIR "MeshLib installation directory" "")

set(MESHLIB_INCLUDE "${MESHLIB_INSTALL_DIR}/include")
set(MESHLIB_LIBRARY "${MESHLIB_INSTALL_DIR}/lib/libMeshLib.so")

set(OCEANSURFACE_INCLUDE "/home/bertrand/Work/FFTWater/OceanSurface")
set(OCEANSURFACE_LIBRARY "/home/bertrand/Work/FFTWater/build/libOceanSurface.so")

include_directories(${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/src/gui
${CMAKE_SOURCE_DIR}/src/terrain
${GLFW_INCLUDE_DIR}
${Glad_INCLUDE_DIR}
${GLM_INCLUDE_DIRS}
${MESHLIB_INCLUDE}
${OCEANSURFACE_INCLUDE}
${CImg_INCLUDE_DIR}
${LIBNOISE_INCLUDE_DIR}
${IMGUI_INCLUDE_DIR})

#Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})


add_executable(GLRenderer
  ${CMAKE_SOURCE_DIR}/src/main.cxx
  ${CMAKE_SOURCE_DIR}/src/EventInterface.cxx
  ${CMAKE_SOURCE_DIR}/src/EventDispatcher.cxx
  ${CMAKE_SOURCE_DIR}/src/Camera.cxx
  ${CMAKE_SOURCE_DIR}/src/Shader.cxx
  ${CMAKE_SOURCE_DIR}/src/DrawableItem.cxx
  ${CMAKE_SOURCE_DIR}/src/SceneRenderer.cxx
  ${CMAKE_SOURCE_DIR}/src/FrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/TerrainLOD.cxx
  ${CMAKE_SOURCE_DIR}/src/gui/TerrainGenGui.cxx
  ${CMAKE_SOURCE_DIR}/src/SceneGraph.cxx
  ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp)

add_executable(TestFrustumCulling
  ${CMAKE_SOURCE_DIR}/test/testFrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/FrustumCulling.cxx)

# add_executable(TestTerrainLOD
#   ${CMAKE_SOURCE_DIR}/test/testTerrainLOD.cxx
#   ${CMAKE_SOURCE_DIR}/src/TerrainLOD.cxx)

add_executable(generateHeightmap
  ${CMAKE_SOURCE_DIR}/test/generateHeightmap.cxx
  ${CMAKE_SOURCE_DIR}/src/HeightmapGenerator.cxx)


target_compile_definitions(GLRenderer PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

#CMAKE_DL_LIBS is needed by glad
target_link_libraries(GLRenderer
${GLFW_LIBRARY}
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
${CImg_LIBRARIES}
${OCEANSURFACE_LIBRARY})

target_link_libraries(TestFrustumCulling
${MESHLIB_LIBRARY})

target_link_libraries(generateHeightmap
${CImg_LIBRARIES}
${LIBNOISE_LIBRARIES})

# target_link_libraries(TestTerrainLOD
# ${MESHLIB_LIBRARY})


file(GLOB_RECURSE srcs
  ${CMAKE_SOURCE_DIR}/src/*.cxx
  ${CMAKE_SOURCE_DIR}/include/*.hxx)
add_custom_target(VeraStyle ALL
  vera++
  --warning
  ${srcs})

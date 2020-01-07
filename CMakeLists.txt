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

option(MESHLIB_INSTALL_DIR "MeshLib installation directory" "")

set(MESHLIB_INCLUDE "${MESHLIB_INSTALL_DIR}/include")
set(MESHLIB_LIBRARY "${MESHLIB_INSTALL_DIR}/lib/libMeshLib.so")

include_directories(${CMAKE_SOURCE_DIR}/include ${GLFW_INCLUDE_DIR} ${Glad_INCLUDE_DIR} ${glm_DIR} ${MESHLIB_INCLUDE})

#Copy shaders to build directory
configure_file(${CMAKE_SOURCE_DIR}/shaders/renderVS.shader ${CMAKE_BINARY_DIR}/renderVS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/renderFS.shader ${CMAKE_BINARY_DIR}/renderFS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/gbufferFS.shader ${CMAKE_BINARY_DIR}/gbufferFS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/gbufferVS.shader ${CMAKE_BINARY_DIR}/gbufferVS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/shadowMapFS.shader ${CMAKE_BINARY_DIR}/shadowMapFS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/shadowMapVS.shader ${CMAKE_BINARY_DIR}/shadowMapVS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/renderTerrainFS.shader ${CMAKE_BINARY_DIR}/renderTerrainFS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/renderTerrainVS.shader ${CMAKE_BINARY_DIR}/renderTerrainVS.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/terrainTC.shader ${CMAKE_BINARY_DIR}/terrainTC.shader COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/terrainTE.shader ${CMAKE_BINARY_DIR}/terrainTE.shader COPYONLY)

add_executable(GLRenderer
  ${CMAKE_SOURCE_DIR}/src/main.cxx
  ${CMAKE_SOURCE_DIR}/src/EventInterface.cxx
  ${CMAKE_SOURCE_DIR}/src/EventDispatcher.cxx
  ${CMAKE_SOURCE_DIR}/src/Camera.cxx
  ${CMAKE_SOURCE_DIR}/src/Shader.cxx
  ${CMAKE_SOURCE_DIR}/src/DrawableItem.cxx
  ${CMAKE_SOURCE_DIR}/src/SceneRenderer.cxx
  ${CMAKE_SOURCE_DIR}/src/FrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/TerrainLOD.cxx)

add_executable(TestFrustumCulling
  ${CMAKE_SOURCE_DIR}/test/testFrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/FrustumCulling.cxx)

# add_executable(TestTerrainLOD
#   ${CMAKE_SOURCE_DIR}/test/testTerrainLOD.cxx
#   ${CMAKE_SOURCE_DIR}/src/TerrainLOD.cxx)

#CMAKE_DL_LIBS is needed by glad
target_link_libraries(GLRenderer
${GLFW_LIBRARY}
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
${CImg_LIBRARIES})

target_link_libraries(TestFrustumCulling
${MESHLIB_LIBRARY})

# target_link_libraries(TestTerrainLOD
# ${MESHLIB_LIBRARY})


file(GLOB_RECURSE srcs
  ${CMAKE_SOURCE_DIR}/src/*.cxx
  ${CMAKE_SOURCE_DIR}/include/*.hxx)
add_custom_target(VeraStyle ALL
  vera++
  --warning
  ${srcs})

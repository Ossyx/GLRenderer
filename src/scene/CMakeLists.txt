include_directories(${CMAKE_SOURCE_DIR}/include/scene
${CMAKE_SOURCE_DIR}/include/common
${CMAKE_SOURCE_DIR}/include/event
${CMAKE_SOURCE_DIR}/include/resources
${JsonCpp_INCLUDE_DIR}
${GLM_INCLUDE_DIRS}
${MESHLIB_INCLUDE_DIR})

add_library(Scene SHARED
  ${CMAKE_SOURCE_DIR}/src/scene/Camera.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/FrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/SceneGraph.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/SceneGraphLoader.cxx)
  
target_link_libraries(Scene
Common
Event
Resource
${JsonCpp_LIBRARY}
)

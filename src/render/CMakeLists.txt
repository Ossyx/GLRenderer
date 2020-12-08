include_directories(${CMAKE_SOURCE_DIR}/include/render
${CMAKE_SOURCE_DIR}/include/resources
${CMAKE_SOURCE_DIR}/include/common
${MESHLIB_INCLUDE_DIR}
${GLM_INCLUDE_DIRS})

add_library(Render SHARED
  ${CMAKE_SOURCE_DIR}/src/render/DrawableItem.cxx
  ${CMAKE_SOURCE_DIR}/src/render/EnvironmentMap.cxx
  ${CMAKE_SOURCE_DIR}/src/render/GeometryHandle.cxx
  ${CMAKE_SOURCE_DIR}/src/render/MaterialTexturesHandle.cxx
  ${CMAKE_SOURCE_DIR}/src/render/Renderable.cxx
  ${CMAKE_SOURCE_DIR}/src/render/GLAbstraction.cxx
  ${CMAKE_SOURCE_DIR}/src/render/Postprocess.cxx
  ${CMAKE_SOURCE_DIR}/src/render/TextureDisplay.cxx)
  
target_link_libraries(Render
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

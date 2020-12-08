include_directories(${CMAKE_SOURCE_DIR}/include/renderer
${CMAKE_SOURCE_DIR}/include/render
${CMAKE_SOURCE_DIR}/include/resources
${CMAKE_SOURCE_DIR}/include/common
${CMAKE_SOURCE_DIR}/include/scene
${CMAKE_SOURCE_DIR}/include/event
${MESHLIB_INCLUDE_DIR}
${GLM_INCLUDE_DIRS})

add_library(Renderer SHARED
${CMAKE_SOURCE_DIR}/src/renderer/Renderer.cxx
${CMAKE_SOURCE_DIR}/src/renderer/RendererFactory.cxx
${CMAKE_SOURCE_DIR}/src/renderer/SimpleRenderer.cxx)
  
target_link_libraries(Renderer
${GLFW_LIBRARY}
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
${CImg_LIBRARIES}
${OCEANSURFACE_LIBRARY}
Common
Resource
Scene
Event
Render)

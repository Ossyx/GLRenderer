add_subdirectory(common)
add_subdirectory(event)
add_subdirectory(resources)
add_subdirectory(scene)
add_subdirectory(render)
add_subdirectory(renderer)
add_subdirectory(tests)

include_directories(${CMAKE_SOURCE_DIR}/include/renderer
${CMAKE_SOURCE_DIR}/include/render
${CMAKE_SOURCE_DIR}/include/common
${CMAKE_SOURCE_DIR}/include/scene
${CMAKE_SOURCE_DIR}/include/event
${CMAKE_SOURCE_DIR}/include/terrain
${CMAKE_SOURCE_DIR}/include/gui
${OCEANSURFACE_INCLUDE}
${GLM_INCLUDE_DIRS}
${MESHLIB_INCLUDE_DIR}
${IMGUI_INCLUDE_DIR})

add_executable(GLRenderer
  ${CMAKE_SOURCE_DIR}/src/main.cxx
  ${CMAKE_SOURCE_DIR}/src/renderer/SceneRenderer.cxx
  ${CMAKE_SOURCE_DIR}/src/terrain/TerrainLOD.cxx
  ${CMAKE_SOURCE_DIR}/src/gui/TerrainGenGui.cxx
  ${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/imgui/imgui_impl_opengl3.cpp)
  
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
Event
Render)

target_compile_definitions(GLRenderer PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

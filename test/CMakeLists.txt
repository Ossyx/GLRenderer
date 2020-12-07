include_directories(${CMAKE_SOURCE_DIR}/include/renderer)

add_executable(TestFrustumCulling
  ${CMAKE_SOURCE_DIR}/test/testFrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/FrustumCulling.cxx)

add_executable(generateHeightmap
  ${CMAKE_SOURCE_DIR}/test/generateHeightmap.cxx
  ${CMAKE_SOURCE_DIR}/src/terrain/HeightmapGenerator.cxx)
  
add_executable(testSimpleRenderer
  ${CMAKE_SOURCE_DIR}/test/simpleRendererTest.cxx)
  
target_link_libraries(testSimpleRenderer
${GLFW_LIBRARY}
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
${CImg_LIBRARIES}
Common
Resource
Scene
Event
Render
Renderer)

target_link_libraries(TestFrustumCulling
${MESHLIB_LIBRARY})

target_link_libraries(generateHeightmap
${CImg_LIBRARIES}
${LIBNOISE_LIBRARIES})

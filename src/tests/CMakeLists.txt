include_directories(
${CMAKE_SOURCE_DIR}/include/renderer
${CMAKE_SOURCE_DIR}/include/event
${CMAKE_SOURCE_DIR}/include/resources
${CMAKE_SOURCE_DIR}/include/scene
${CMAKE_SOURCE_DIR}/include/common
${CMAKE_SOURCE_DIR}/include/render
${CMAKE_SOURCE_DIR}/include/terrain
${MESHLIB_INCLUDE_DIR}
${GLM_INCLUDE_DIRS})

#ResourceLoader
add_executable(testResourceLoader
  ${CMAKE_SOURCE_DIR}/src/tests/resourceLoaderTest.cxx)  
target_link_libraries(testResourceLoader Common Resource)

#Common
add_executable(commonTests
  ${CMAKE_SOURCE_DIR}/src/tests/commonTests.cxx)  
target_link_libraries(commonTests Common)

#SceneGraphLoader
add_executable(sceneGraphLoaderTest
  ${CMAKE_SOURCE_DIR}/src/tests/sceneGraphLoaderTest.cxx)  
target_link_libraries(sceneGraphLoaderTest Common Scene Resource)

add_executable(TestFrustumCulling
  ${CMAKE_SOURCE_DIR}/src/tests/testFrustumCulling.cxx
  ${CMAKE_SOURCE_DIR}/src/scene/FrustumCulling.cxx)
target_link_libraries(TestFrustumCulling
${MESHLIB_LIBRARY})

add_executable(generateHeightmap
  ${CMAKE_SOURCE_DIR}/src/tests/generateHeightmap.cxx
  ${CMAKE_SOURCE_DIR}/src/terrain/HeightmapGenerator.cxx)
target_link_libraries(generateHeightmap
${CImg_LIBRARIES}
${LIBNOISE_LIBRARIES})
  
add_executable(testSimpleRenderer
  ${CMAKE_SOURCE_DIR}/src/tests/simpleRendererTest.cxx)
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

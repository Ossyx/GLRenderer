add_executable(testResourceLoader
  ${CMAKE_SOURCE_DIR}/src/tests/resourceLoaderTest.cxx)
  
add_executable(commonTests
  ${CMAKE_SOURCE_DIR}/src/tests/commonTests.cxx)
  
add_executable(sceneGraphLoaderTest
  ${CMAKE_SOURCE_DIR}/src/tests/sceneGraphLoaderTest.cxx)
  
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

include_directories(${CMAKE_SOURCE_DIR}/include/resources
${CMAKE_SOURCE_DIR}/include/common
${CMAKE_SOURCE_DIR}/include/render
${MESHLIB_INCLUDE_DIR}
${JsonCpp_INCLUDE_DIR}
${GLM_INCLUDE_DIRS})

add_library(Resource SHARED
  ${CMAKE_SOURCE_DIR}/src/resources/ResourceDescription.cxx
  ${CMAKE_SOURCE_DIR}/src/resources/ResourcesHolder.cxx
  ${CMAKE_SOURCE_DIR}/src/resources/ResourcesLoader.cxx)
  
target_link_libraries(Resource
Common
${MESHLIB_LIBRARY}
${JsonCpp_LIBRARY}
)

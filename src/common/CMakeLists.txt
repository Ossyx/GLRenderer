include_directories(${CMAKE_SOURCE_DIR}/include/common
${GLM_INCLUDE_DIRS}
${MESHLIB_INCLUDE_DIR})

#Libraries
add_library(Common SHARED
  ${CMAKE_SOURCE_DIR}/src/common/Shader.cxx
  ${CMAKE_SOURCE_DIR}/src/common/MathTools.cxx
  ${CMAKE_SOURCE_DIR}/src/common/MaterialShader.cxx
  ${CMAKE_SOURCE_DIR}/src/common/MaterialShaderLoader.cxx
  ${CMAKE_SOURCE_DIR}/src/common/GLAbstraction.cxx)
  
target_link_libraries(Common
${Glad_LIBRARIES}
${CMAKE_DL_LIBS}
${MESHLIB_LIBRARY}
)

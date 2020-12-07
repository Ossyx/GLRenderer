include_directories(${GLFW_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/include/event)

add_library(Event SHARED
  ${CMAKE_SOURCE_DIR}/src/event/EventDispatcher.cxx
  ${CMAKE_SOURCE_DIR}/src/event/EventInterface.cxx)
  
target_link_libraries(Event
${GLFW_LIBRARY}
)

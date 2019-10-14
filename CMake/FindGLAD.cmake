# - Try to find CIMG
# Once done, this will define
#
#  CIMG_FOUND - system has CIMG
#  CIMG_INCLUDE_DIRS - the CIMG include directories
#

find_path(GLAD_INCLUDE_DIR
  NAMES glad/glad.h
  PATHS /usr/local/include
)

find_library(GLAD_LIBRARY
  NAMES
      glad
  PATHS
      /usr/lib64
      /usr/lib
      /usr/local/lib64
      /usr/local/lib
      /sw/lib
      /opt/local/lib
      /usr/lib/x86_64-linux-gnu
      NO_DEFAULT_PATH
      DOC "The GLAD library")

set(Glad_INCLUDE_DIR ${GLAD_INCLUDE_DIR})
set(Glad_LIBRARIES ${GLAD_LIBRARY})

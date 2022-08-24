#ifndef GLRENDEREREXPORT_HXX
#define GLRENDEREREXPORT_HXX

#if defined (WIN32)
  #define GLRenderer_EXPORT __declspec(dllexport)
#else
  #define GLRenderer_EXPORT
#endif

#endif

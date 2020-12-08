#ifndef RENDERER_HXX
#define RENDERER_HXX

#include "ResourcesHolder.hxx"
#include "SceneGraph.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer
{
public:
  Renderer();
  virtual ~Renderer();
  
  virtual void Initialize(rx::SceneGraphPtr pSceneGraph,
    rx::ResourcesHolderPtr pResourcesHolder) = 0;
  virtual void Render(GLFWwindow* pWindow) = 0;
  
};
#endif

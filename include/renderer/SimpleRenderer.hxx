#ifndef SIMPLERENDERER_HXX
#define SIMPLERENDERER_HXX

#include "Renderer.hxx"
#include "ResourcesHolder.hxx"
#include "SceneGraph.hxx"

#include "DrawableItem.hxx"
#include "EnvironmentMap.hxx"
#include "Renderable.hxx"
#include "Postprocess.hxx"
#include "TextureDisplay.hxx"
#include "GLAbstraction.hxx"

#include <chrono>

class SimpleRenderer : public Renderer
{
public:
  
  SimpleRenderer();
  ~SimpleRenderer();
  
  void Initialize(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder) override;
  void InitFbo();
  
  void Render(GLFWwindow* pWindow) override;
  void RenderSceneGraph(GLFWwindow* pWindow);
  void BuildEnvMap();
  
private:

  EnvironmentMap* envMap;
  Postprocess* mPostProcess;  
  TextureDisplay* mSSFinalRender;
  FramebufferObject mFBO;
};

#endif

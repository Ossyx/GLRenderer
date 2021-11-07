#ifndef DEFERREDRENDERER_HXX
#define DEFERREDRENDERER_HXX

#include "Renderer.hxx"
#include "GLAbstraction.hxx"
#include "TextureDisplay.hxx"
#include "Postprocess.hxx"
#include "EnvironmentMap.hxx"

class DeferredRenderer : public Renderer
{
public:
  DeferredRenderer();
  ~DeferredRenderer();
  
  void Initialize(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder) override;
  void Render(GLFWwindow* pWindow) override;
  void GBufferPass(GLFWwindow* pWindow);
  void LightingPass(GLFWwindow* pWindow);
  
private: 
  
  FramebufferObject mGBuffer;
  FramebufferObject mLighting;
  TextureDisplay* mGBufferDebug;
  TextureDisplay* mLightingPP;
  TextureDisplay* mFinalRender;
  unsigned int renderStencil;

  EnvironmentMapBasePtr mEnvMap;
  
  glm::mat4 mInvProjection;
  glm::mat4 mNormalMatrix;
  glm::mat4 mInvView;
  glm::mat4 mView;
};
#endif

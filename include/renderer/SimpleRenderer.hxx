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

#include "Camera.hxx"
#include <chrono>

class SimpleRenderer : public Renderer
{
public:
  
  SimpleRenderer();
  ~SimpleRenderer();
  
  void Initialize(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder) override;
  void InitS(rx::ResourcesHolderPtr pResourcesHolder);
  void InitPostprocess(rx::ResourcesHolderPtr pResourcesHolder);
  void InitFbo();
  
  void Render(GLFWwindow* pWindow) override;
  void RenderSceneGraph(GLFWwindow* pWindow);
  
private:
  
  //Material -> Texture handle
  using MaterialTextureHandleMap = std::unordered_map<std::string, MaterialTextureHandlePtr>;
  
  //Mesh -> Geometry Handle (VBOs)
  using GeometryHandleMap = std::unordered_map<std::string, GeometryHandlePtr>;
  
  rx::ResourcesHolderPtr mHolder;
  rx::SceneGraphPtr mSceneGraph;
  Camera mCamera;
  std::shared_ptr<Shader> mCustomShader;
  
  MaterialTextureHandleMap mMatTextureHandles;
  GeometryHandleMap mGeoHandles;
  
  std::vector<Renderable*> mRenderables;
  
  EnvironmentMap* envMap;
  Postprocess* mPostProcess;
  std::chrono::steady_clock::time_point mTime;
  
  TextureDisplay* mSSFinalRender;
  FramebufferObject mFBO;
  unsigned int mRenderTarget;
  unsigned int mFbo;
};

#endif

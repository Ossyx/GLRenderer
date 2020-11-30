#ifndef SIMPLERENDERER_HXX
#define SIMPLERENDERER_HXX

#include "ResourcesHolder.hxx"
#include "SceneGraph.hxx"

#include "DrawableItem.hxx"
#include "EnvironmentMap.hxx"
#include "Renderable.hxx"

#include "Camera.hxx"
#include <chrono>

class SimpleRenderer
{
public:
  
  SimpleRenderer();
  ~SimpleRenderer();
  
  void InitS(rx::ResourcesHolderPtr pResourcesHolder);
  void Init(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder);
  void Render(GLFWwindow* pWindow);
  
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
  std::chrono::steady_clock::time_point mTime;
};

#endif

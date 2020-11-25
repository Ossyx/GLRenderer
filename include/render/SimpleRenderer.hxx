#ifndef SIMPLERENDERER_HXX
#define SIMPLERENDERER_HXX

#include "ResourcesHolder.hxx"
#include "SceneGraph.hxx"

#include "DrawableItem.hxx"
#include "EnvironmentMap.hxx"

#include "Camera.hxx"
#include <chrono>

class SimpleRenderer
{
public:
  
  SimpleRenderer();
  ~SimpleRenderer();
  
  void Init(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder);
  void Render(GLFWwindow* pWindow);
  
private:
  
  rx::ResourcesHolderPtr mHolder;
  rx::SceneGraphPtr mSceneGraph;
  Camera mCamera;
  std::shared_ptr<Shader> mCustomShader;
  
  //Collection drawable  
  std::vector<DrawableItem*> mItems;
  std::vector<rx::MaterialPtr> mMaterials;
  
  EnvironmentMap* envMap;
  std::chrono::steady_clock::time_point mTime;
};

#endif

#ifndef RENDERER_HXX
#define RENDERER_HXX

#include "ResourcesHolder.hxx"
#include "SceneGraph.hxx"
#include "MaterialTexturesHandle.hxx"
#include "GeometryHandle.hxx"
#include "Camera.hxx"
#include "Renderable.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLRendererExport.hxx"

class GLRenderer_EXPORT Renderer
{
public:
  Renderer();
  virtual ~Renderer();
  
  virtual void Initialize(rx::SceneGraphPtr pSceneGraph,
    rx::ResourcesHolderPtr pResourcesHolder) = 0;
  virtual void Render(GLFWwindow* pWindow) = 0;
  
protected:
  //Material -> Texture handle
  using MaterialTextureHandleMap = std::unordered_map<std::string, MaterialTextureHandlePtr>;
  
  //Mesh -> Geometry Handle (VBOs)
  using GeometryHandleMap = std::unordered_map<std::string, GeometryHandlePtr>;
  
  virtual void BuildRenderables();
  
  MaterialTextureHandleMap mMatTextureHandles;
  GeometryHandleMap mGeoHandles;
  
  rx::SceneGraphPtr mSceneGraph;
  rx::ResourcesHolderPtr mHolder;
  
  Camera mCamera;
  std::chrono::steady_clock::time_point mTime;
  
  std::vector<Renderable*> mRenderables;  
};
#endif

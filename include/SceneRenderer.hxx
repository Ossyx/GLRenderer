#ifndef SCENERENDERER_HXX
#define SCENERENDERER_HXX

#include "ModelLoader.hxx"
#include "DrawableItem.hxx"
#include "Shader.hxx"
#include "Camera.hxx"
#include "EventDispatcher.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum GBufferShaderFlag
{
    albedoColor = 0x1,
    albedoTexture = 0x2,
    specularColor = 0x4,
    specularTexture = 0x8,
    normalTexture = 0x10,
    uvCoords = 0x20
};

class SceneRenderer
{
public:

  SceneRenderer(GLFWwindow* p_window);

  ~SceneRenderer();

  void AddModel();

  void RenderDummy(GLFWwindow* p_window);

  void Render(GLFWwindow* p_window);

  void RenderGBufferDebug(GLFWwindow* p_window);

  typedef std::map<std::string, unsigned int> UintMap;

private:

  //Disable copy
  SceneRenderer(SceneRenderer const& p_other);

  void GenerateGBufferShader(Mesh* p_mesh, Material* p_material);

  std::string GeneratePreprocessorDefine(unsigned int p_gBufferFlags);

  void PrepareTwoTrianglesBuffer();

  std::vector<DrawableItem> m_drawableItems;

  typedef std::map<unsigned int, Shader> GBufferShaderMap;
  GBufferShaderMap m_gbufferShaders;

  UintMap m_shaderForMaterial;

  std::vector<Material*> m_materialPtrs;

  Camera m_mainCamera;

  Shader m_renderShader;

  Shader m_dummyShader;

  //For 2 triangles support render
  unsigned int m_vBufferId;
  unsigned int m_iBufferId;
  unsigned int m_renderVertexArray;

  //Gbuffer render target
  unsigned int m_gbufferFBO;
  unsigned int m_renderTarget1;
  unsigned int m_renderTarget2;
  unsigned int m_renderTarget3;

  unsigned int m_renderTarget1_read;
  unsigned int m_renderTarget2_read;
};

#endif

#include "SceneRenderer.hxx"
#include "Primitives.hxx"
#include "TerrainGenGui.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

SceneRenderer::SceneRenderer(GLFWwindow* p_window):
m_mainCamera(glm::vec3(10.0f, 0.0f, 0.0f)),
m_terrainGUI(p_window)
{
  EventDispatcher* dispatcher = EventDispatcher::Get();
  dispatcher->AddEventListener("MainCamera", &m_mainCamera);
  dispatcher->AddEventListener("SceneRenderer", this);

  m_renderShader.SetVertexShaderSrc("shaders/deferred/renderVS.shader");
  m_renderShader.SetFragmentShaderSrc("shaders/deferred/renderFS.shader");
  m_renderShader.LinkProgram();
  PrepareTwoTrianglesBuffer();

  m_shadowMapShader.SetVertexShaderSrc("shaders/shadow/shadowMapVS.shader");
  m_shadowMapShader.SetFragmentShaderSrc("shaders/shadow/shadowMapFS.shader");
  m_shadowMapShader.LinkProgram();

  ShaderPtr terrainRendering = std::make_shared<Shader>();
  terrainRendering->SetVertexShaderSrc("shaders/terrain/renderTerrainVS.shader");
  terrainRendering->SetFragmentShaderSrc("shaders/terrain/renderTerrainFS.shader");
  terrainRendering->SetTessControlSrc("shaders/terrain/terrainTC.shader");
  terrainRendering->SetTessEvalSrc("shaders/terrain/terrainTE.shader");
  terrainRendering->LinkProgram();
  
  rx::MaterialPtr terrainMaterial = std::make_shared<rx::Material>();
  terrainMaterial->SetName("CubeMaterial");
  terrainMaterial->SetData("Ka", glm::vec3(0.2,0.0,0.0));
  terrainMaterial->SetData("Ks", glm::vec3(1.0,0.0,0.0));
  terrainMaterial->SetData("Kd", glm::vec3(1.0,0.0,0.0));
  terrainMaterial->SetUniformData("ambient_color", "Ka");
  terrainMaterial->SetUniformData("specular_color", "Ks");
  terrainMaterial->SetUniformData("diffuse_color", "Kd");
  
  m_terrain = std::make_shared<TerrainLOD>(nullptr, terrainMaterial, terrainRendering);
  m_terrain->SetSize(6.4f);

  ShaderPtr waterRendering = std::make_shared<Shader>();
  waterRendering->SetVertexShaderSrc("shaders/terrain/renderTerrainVS.shader");
  waterRendering->SetFragmentShaderSrc("shaders/water/renderWaterFS.shader");
  waterRendering->SetTessControlSrc("shaders/terrain/terrainTC.shader");
  waterRendering->SetTessEvalSrc("shaders/water/waterTE.shader");
  waterRendering->LinkProgram();
  m_water = std::make_shared<TerrainLOD>(nullptr, terrainMaterial, waterRendering);
  m_water->SetSize(6.401f);
  m_surf.PrecomputeFields();

  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);

  PrepareGBufferFrameBufferObject(width, height);
  PrepareShadowMapFrameBufferObject(width, height);

  m_sunLightDirection = glm::normalize(glm::vec3(10.0, 3.0, 10.0));

  m_renderParam = RenderingDebugInfo::Get();
}

SceneRenderer::~SceneRenderer()
{
  for(unsigned i = 0; i < m_drawableItems.size(); ++i)
  {
    delete m_drawableItems[i];
  }
}

void SceneRenderer::PrepareTwoTrianglesBuffer()
{
  const float vertex_buffer_data[] =
  {
    -1.0f,  -1.0f,  0.0f,
    1.0f,  -1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
    1.0f,  1.0f,  0.0f
  };

  const unsigned int index_buffer[] = {0, 1, 2, 1, 3, 2};

  unsigned int vpos_location = m_renderShader.GetAttributeLocation("vPos");

  glGenBuffers(1, &m_vBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_vBufferId);
  glBufferData(GL_ARRAY_BUFFER, 4*3*sizeof(float),
    vertex_buffer_data, GL_STATIC_DRAW);

  glGenBuffers(1, &m_iBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*3*sizeof(unsigned int),
    index_buffer, GL_STATIC_DRAW);

  glGenVertexArrays(1, &m_renderVertexArray);
  glBindVertexArray(m_renderVertexArray);

  glEnableVertexAttribArray(vpos_location);
  glBindBuffer(GL_ARRAY_BUFFER, m_vBufferId);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void SceneRenderer::AddModel()
{
  auto myModel = rx::ModelLoader::LoadOBJModel("/home/bertrand/Work/models/sponza/sponza.obj", "sponza");

  for (unsigned int i = 0; i < myModel->GetMeshCount(); ++i)
  {
    
    auto meshPtr = myModel->GetMesh(i);
    rx::MaterialPtr materialPtr = myModel->GetMaterialForMesh(i);
    assert(meshPtr != NULL && materialPtr != NULL);

    //find the shader for this mesh
    GenerateGBufferShader(*meshPtr, materialPtr);
    UintMap::iterator itShaderId = m_shaderForMaterial.find(materialPtr->GetName());
    if (itShaderId != m_shaderForMaterial.end())
    {
      GBufferShaderMap::iterator itShader = m_gbufferShaders.find(itShaderId->second);
      assert(itShader != m_gbufferShaders.end());
      DrawableItem* item = new DrawableItem(meshPtr, materialPtr, itShader->second);
      item->PrepareBuffer();
      m_drawableItems.push_back(item);
      m_materialPtrs.push_back(materialPtr);
    }
    else
    {
       rxLogError("No shader "<< materialPtr->GetShaderName()
         <<" found for material of mesh " << i);
       assert(false);
    }
  }
}

void SceneRenderer::AddTerrain()
{
  m_terrain->PrepareBufferQuad(m_mainCamera);
  m_water->PrepareBufferQuad(m_mainCamera);

  //Build texture for water surface
  glGenTextures(1, &m_watersurfTex);
  glBindTexture(GL_TEXTURE_2D, m_watersurfTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  float dummy[128*128];
  for(int i=0; i < 128*128; ++i) dummy[i] = 0.5;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 128, 128, 0, GL_RED, GL_FLOAT, &dummy);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void SceneRenderer::Render(GLFWwindow* p_window)
{
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  auto frameTimer = std::chrono::steady_clock::now();
  while (glfwWindowShouldClose(p_window) == false)
  {
    auto newTime = std::chrono::steady_clock::now();
    float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - frameTimer).count();
    m_renderParam->m_fps = 1000.0f / millis;

    m_terrainGUI.InitGuiFrame();

    UpdateCamera(millis);
    //renderer.RenderShadowMap(window);
    //renderer.Render(window);
    RenderTerrain(p_window);
    RenderGBufferDebug(p_window);
    m_terrainGUI.RenderGui();

    glfwSwapBuffers(p_window);
    glfwPollEvents();

    frameTimer = newTime;
  }

  glfwDestroyWindow(p_window);
  glfwTerminate();
}

void SceneRenderer::RenderObjects(GLFWwindow* p_window)
{
  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),
    (float)width / (float)height, 0.001f, 1000.f);

  //float time = glfwGetTime();
  glm::mat4 identity = glm::mat4(1.0f);
  glm::mat4 view = identity;

  view = glm::rotate(view, m_mainCamera.GetElevation(), glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, m_mainCamera.GetAzimuth(), glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::translate(view, m_mainCamera.GetPosition());

  m_invProjMatrix = glm::inverse(projection);
  m_invViewMatrix = glm::inverse(view);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbufferFBO);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, buffers);

  glViewport(0, 0, width, height);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glm::mat4 model = identity;
  model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));

  for (unsigned int i = 0; i < m_drawableItems.size(); ++i)
  {
    rx::MaterialPtr materialPtr = m_materialPtrs[i];
    UintMap::iterator itShaderId = m_shaderForMaterial.find(materialPtr->GetName());
    if (itShaderId != m_shaderForMaterial.end())
    {
      ShaderPtr shader = m_gbufferShaders[itShaderId->second];
      glUseProgram(shader->GetProgram());
      m_drawableItems[i]->SetTransform(model);
      m_drawableItems[i]->Draw(view, projection, model, m_sunLightDirection, 
        m_mainCamera.GetPosition());
    }

  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glfwPollEvents();
}

void SceneRenderer::RenderTerrain(GLFWwindow* p_window)
{
  if(m_mainCamera.m_wireframe)
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }

  bool surfaceMode = m_terrain->ComputeDistanceToSurface(m_mainCamera.GetPosition()) < 0.01f;
  float scale = 1.0f;
  glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
  if(surfaceMode)
  {
    scale = 1000.0f;
    origin = m_terrain->sphereProjectedPosition(m_mainCamera.GetPosition());
  }
  m_terrain->SetScale(scale);
  m_terrain->SetOrigin(origin);
  m_water->SetScale(scale);
  m_water->SetOrigin(origin);

  m_surf.ComputeHeightmap(glfwGetTime());
  ComputeNearFarProjection();

  m_mainCamera.SetNear(m_mainCamera.GetNear() * scale);
  m_mainCamera.SetFar(m_mainCamera.GetFar() * scale);
  m_renderParam->m_near = m_mainCamera.GetNear();
  m_renderParam->m_far = m_mainCamera.GetFar();

  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),
    (float)width / (float)height, m_mainCamera.GetNear(), m_mainCamera.GetFar());

  m_mainCamera.SetProjectionMatrix(projection);

  glm::mat4 view = m_mainCamera.GetViewMatrix();
  m_invProjMatrix = glm::inverse(projection);
  m_invViewMatrix = glm::inverse(view);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbufferFBO);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, buffers);

  glViewport(0, 0, width, height);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 model = glm::mat4(1.0f);

  if( m_mainCamera.m_treeRecompute )
  {
    m_terrain->RecomputeTree(m_mainCamera.GetPosition());
    m_water->RecomputeTree(m_mainCamera.GetPosition());
  }
  m_terrain->PrepareBufferQuad(m_mainCamera);
  m_terrain->DrawTerrain(m_mainCamera, model, m_sunLightDirection);

  if( m_renderParam->m_renderWater )
  {
    m_water->PrepareBufferQuad(m_mainCamera);
    m_water->DrawWater(m_mainCamera, model, m_sunLightDirection, m_surf, m_watersurfTex);
  }

  rxLogInfo(m_mainCamera.GetPosition().x<<" "<<m_mainCamera.GetPosition().y<<" "<<m_mainCamera.GetPosition().z);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glfwPollEvents();

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
}

void SceneRenderer::RenderShadowMap(GLFWwindow* p_window)
{
  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);
  //glm::mat4 projection = glm::perspective(glm::radians(45.0f),
  //  (float)width / (float)height, 0.5f, 3000.f);

  glm::mat4 projection = glm::ortho<float>(-200.0, 200.0, -200.0, 200.0, -300.0, 300.0);

  glm::mat4 identity = glm::mat4(1.0f);
  float time = glfwGetTime();
  auto rotYMat = glm::rotate(identity, time/50000.0f, glm::vec3(0.0, 1.0, 0.0));
  //m_sunLightDirection = glm::vec3(rotYMat * glm::vec4(m_sunLightDirection, 1.0f));
  m_sunLightDirection = glm::normalize(m_sunLightDirection);
  glm::mat4 view = glm::lookAt(m_sunLightDirection * 100.0f,
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  m_shadowMapMatrix = projection * view;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffers);

  glViewport(0, 0, 4000, 4000);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 model = identity;
  model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));

  for (unsigned int i = 0; i < m_drawableItems.size(); ++i)
  {
      glUseProgram(m_shadowMapShader.GetProgram());
      m_drawableItems[i]->SetTransform(model);
      m_drawableItems[i]->DrawSimple(m_shadowMapShader, m_shadowMapMatrix, view, projection, model);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glfwPollEvents();

}

void SceneRenderer::RenderGBufferDebug(GLFWwindow* p_window)
{
  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glUseProgram(m_renderShader.GetProgram());
  glViewport(0, 0, width, height);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget1);
  unsigned int rt1_location = m_renderShader.GetUniformLocation("render_target_one");
  glUniform1i(rt1_location, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget2);
  unsigned int rt2_location = m_renderShader.GetUniformLocation("render_target_two");
  glUniform1i(rt2_location, 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget3);
  unsigned int rt3_location = m_renderShader.GetUniformLocation("render_target_three");
  glUniform1i(rt3_location, 2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget4);
  unsigned int rt4_location = m_renderShader.GetUniformLocation("render_target_four");
  glUniform1i(rt4_location, 3);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, m_shadowMapTextureId1);
  unsigned int rt5_location = m_renderShader.GetUniformLocation("render_target_five");
  glUniform1i(rt5_location, 4);

  glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );


  unsigned int lightLoc = m_renderShader.GetUniformLocation("light");
  unsigned int camLoc = m_renderShader.GetUniformLocation("cameraPos");
  unsigned int shadowMapMatrixLoc = m_renderShader.GetUniformLocation("shadowMapMatrix");
  unsigned int invertProjMatrixLoc = m_renderShader.GetUniformLocation("invertProjMatrix");
  unsigned int invertViewMatrixLoc = m_renderShader.GetUniformLocation("invertViewMatrix");
  unsigned int biasMatrixLoc = m_renderShader.GetUniformLocation("biasMatrix");

  glUniform3fv(lightLoc, 1,  glm::value_ptr(m_sunLightDirection));
  glUniform3fv(camLoc, 1,  glm::value_ptr(m_mainCamera.GetPosition()));
  glUniformMatrix4fv(shadowMapMatrixLoc, 1, GL_FALSE, glm::value_ptr(m_shadowMapMatrix));
  glUniformMatrix4fv(invertProjMatrixLoc, 1, GL_FALSE, glm::value_ptr(m_invProjMatrix));
  glUniformMatrix4fv(invertViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(m_invViewMatrix));
  glUniformMatrix4fv(biasMatrixLoc, 1, GL_FALSE, glm::value_ptr(biasMatrix));


  glBindVertexArray(m_renderVertexArray);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBufferId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneRenderer::UpdateCamera(float p_elapsedMs)
{
  //Calc elapsed time
  //m_mainCamera.SmoothMovement(1.0f/1000.0f);
  m_mainCamera.MoveCamera(p_elapsedMs/1000.0f, 0.05);
}

void SceneRenderer::GenerateGBufferShader(rx::Mesh const& p_mesh, rx::MaterialPtr p_material)
{
  unsigned int gBufferFlags = 0;

  rx::Material::StringMap const& uniforms = p_material->GetUniforms();

  if (uniforms.find("map_diffuse") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::albedoTexture;
  }
  if (uniforms.find("map_specular") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::specularTexture;
  }
  if (uniforms.find("map_normal") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::normalTexture;
  }
  if (uniforms.find("map_roughness") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::roughnessTexture;
  }
  if (uniforms.find("map_ao") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::aoTexture;
  }
  if (uniforms.find("diffuse_color") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::albedoColor;
  }
  if (uniforms.find("specular_color") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::specularColor;
  }
  if (uniforms.find("roughness_color") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::roughnessColor;
  }
  if (uniforms.find("map_displacement") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::displacementTexture;
  }
  if (p_mesh.HasUVCoords())
  {
    gBufferFlags |= GBufferShaderFlag::uvCoords;
  }

  std::string preprocSrc = GeneratePreprocessorDefine(gBufferFlags);
  GBufferShaderMap::iterator itShader = m_gbufferShaders.find(gBufferFlags);
  //Do not generate a new shader for a configuration that already exists
  if (itShader == m_gbufferShaders.end())
  {
    rxLogInfo("Creating shader for material : "<< p_material->GetName());
    ShaderPtr shader = std::make_shared<Shader>();
    m_gbufferShaders[gBufferFlags] = shader;
    shader->SetName("GBufferGenerationShader");
    shader->SetVertexShaderSrc("shaders/deferred/gbufferVS.shader");
    shader->SetFragmentShaderSrc("shaders/deferred/gbufferFS.shader");
    shader->SetPreprocessorConfig(preprocSrc);
    shader->LinkProgram();
  }

  m_shaderForMaterial[p_material->GetName()] = gBufferFlags;
}

std::string SceneRenderer::GeneratePreprocessorDefine(unsigned int p_gBufferFlags)
{
  std::stringstream result;
  if (p_gBufferFlags & GBufferShaderFlag::albedoColor
    && !(p_gBufferFlags & GBufferShaderFlag::albedoTexture))
  {
    result << "#define ALBEDO_COLOR" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::albedoTexture )
  {
    result << "#define ALBEDO_TEXTURE" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::specularColor
    && !(p_gBufferFlags & GBufferShaderFlag::specularTexture))
  {
    result << "#define SPECULAR_COLOR" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::specularTexture )
  {
    result << "#define SPECULAR_TEXTURE" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::normalTexture )
  {
    result << "#define NORMAL_TEXTURE" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::aoTexture )
  {
    result << "#define AO_TEXTURE" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::roughnessColor
    && !(p_gBufferFlags & GBufferShaderFlag::roughnessTexture))
  {
    result << "#define ROUGHNESS_COLOR" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::roughnessTexture )
  {
    result << "#define ROUGHNESS_TEXTURE" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::uvCoords )
  {
    result << "#define HAS_UVCOORDS" <<std::endl;
  }
  if (p_gBufferFlags & GBufferShaderFlag::displacementTexture )
  {
    result << "#define DISPLACEMENT_TEXTURE" <<std::endl;
  }
  return result.str();
}

void SceneRenderer::PrepareGBufferFrameBufferObject(int p_width, int p_height)
{
  //Prepare render target for GBuffer
  glGenFramebuffers(1, &m_gbufferFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbufferFBO);

  glGenTextures(1, &m_renderTarget1);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTarget1, 0);

  glGenTextures(1, &m_renderTarget2);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_renderTarget2, 0);

  glGenTextures(1, &m_renderTarget3);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget3);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_renderTarget3, 0);

  glGenTextures(1, &m_renderTarget4);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, p_width, p_height,
    0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_renderTarget4, 0);

//   GLuint depthrenderbuffer;
//   glGenRenderbuffers(1, &depthrenderbuffer);
//   glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, p_width, p_height);
//   glBindRenderbuffer(GL_RENDERBUFFER, 0);
//   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//     GL_RENDERBUFFER, depthrenderbuffer);

  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("Gbuffer framebuffer incomplete ! ");
    assert(false);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneRenderer::PrepareShadowMapFrameBufferObject(int p_width, int p_height)
{
  glGenFramebuffers(1, &m_shadowMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);

  glGenTextures(1, &m_shadowMapTextureId1);
  glBindTexture(GL_TEXTURE_2D, m_shadowMapTextureId1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 4000, 4000,
    0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_TEXTURE_2D, m_shadowMapTextureId1, 0);

  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("ShadowMap framebuffer incomplete ! ");
    assert(false);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneRenderer::ComputeNearFarProjection()
{
  if( m_renderParam->m_renderWater )
  {
    m_mainCamera.SetNear(std::min(m_terrain->GetNear(), m_water->GetNear()));
    m_mainCamera.SetFar(std::min(m_terrain->GetFar(), m_water->GetFar()));
  }
  else
  {
    m_mainCamera.SetNear(m_terrain->GetNear());
    m_mainCamera.SetFar(m_terrain->GetFar());
  }
}

void SceneRenderer::HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS && key == GLFW_KEY_P)
  {
    EventDispatcher* dispatcher = EventDispatcher::Get();
    if(m_terrainGUI.showParameters == false)
    {
      dispatcher->RemoveEventListener("MainCamera");
    }
    else
    {
      dispatcher->AddEventListener("MainCamera", &m_mainCamera);
    }
    m_terrainGUI.showParameters = !m_terrainGUI.showParameters;
  }
}

void SceneRenderer::HandleCursorEvent(double p_xpos, double p_ypos, double p_deltaX, double p_deltaY)
{
}

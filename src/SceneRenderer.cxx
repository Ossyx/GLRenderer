#include "SceneRenderer.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

SceneRenderer::SceneRenderer(GLFWwindow* p_window):
m_mainCamera()
{
  EventDispatcher* dispatcher = EventDispatcher::Get();
  dispatcher->AddEventListener("MainCamera", &m_mainCamera);

  m_renderShader.SetVertexShaderSrc("renderVS.shader");
  m_renderShader.SetFragmentShaderSrc("renderFS.shader");
  m_renderShader.LinkProgram();
  PrepareTwoTrianglesBuffer();

  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);

  //Prepare render target for GBuffer
  glGenFramebuffers(1, &m_gbufferFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbufferFBO);

  glGenTextures(1, &m_renderTarget1);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderTarget1, 0);

  glGenTextures(1, &m_renderTarget2);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_renderTarget2, 0);

  glGenTextures(1, &m_renderTarget3);
  glBindTexture(GL_TEXTURE_2D, m_renderTarget3);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_renderTarget3, 0);

  GLuint depthrenderbuffer;
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, depthrenderbuffer);

  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("Framebuffer incomplete ! ");
    assert(false);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

SceneRenderer::~SceneRenderer()
{
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
  rx::ModelLoader loader;
  loader.LoadOBJModel("/home/bertrand/Work/models/sponza", "sponza.obj", "sponza");
  rx::Model* myModel = loader.FindModel("sponza");

  for (unsigned int i = 0; i < myModel->GetMeshCount(); ++i)
  {
    DrawableItem item;
    rx::Mesh* meshPtr = myModel->GetMesh(i);
    rx::Material* materialPtr = myModel->GetMaterialForMesh(i);
    assert(meshPtr != NULL && materialPtr != NULL);

    //find the shader for this mesh
    GenerateGBufferShader(meshPtr, materialPtr);
    UintMap::iterator itShaderId = m_shaderForMaterial.find(materialPtr->GetName());
    if (itShaderId != m_shaderForMaterial.end())
    {
      GBufferShaderMap::iterator itShader = m_gbufferShaders.find(itShaderId->second);
      assert(itShader != m_gbufferShaders.end());
      item.PrepareBuffer(*meshPtr, *materialPtr, itShader->second);
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

void SceneRenderer::Render(GLFWwindow* p_window)
{
  auto theTime = std::chrono::steady_clock::now();
  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),
    (float)width / (float)height, 1.0f, 3000.f);

  float time = glfwGetTime();
  glm::mat4 identity = glm::mat4(1.0f);
  glm::mat4 view = identity;

  view = glm::rotate(view, m_mainCamera.GetElevation(), glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, m_mainCamera.GetAzimuth(), glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::translate(view, m_mainCamera.GetPosition());
  glm::mat4 VP = projection * view;

  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbufferFBO);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, buffers);

  glViewport(0, 0, width, height);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glm::mat4 model = identity;
  model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
  //model = glm::rotate(model, time, glm::vec3(0.f,1.f,0.f));
  model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

  //rotate light
  auto rotYMat = glm::rotate(identity, time/5000.0f, glm::vec3(0.0, 0.0, 1.0));
  glm::vec3 light(1.0, 1.0, 1.0);
  light = glm::vec3(rotYMat * glm::vec4(light, 1.0f));

  for (unsigned int i = 0; i < m_drawableItems.size(); ++i)
  {
    rx::Material* materialPtr = m_materialPtrs[i];
    UintMap::iterator itShaderId = m_shaderForMaterial.find(materialPtr->GetName());
    if (itShaderId != m_shaderForMaterial.end())
    {
      Shader const& shader = m_gbufferShaders[itShaderId->second];
      glUseProgram(shader.GetProgram());
      m_drawableItems[i].SetTransform(model);
      m_drawableItems[i].Draw(shader, VP, *materialPtr, view, projection,
        model, light, m_mainCamera.GetPosition());
    }

  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Calc elapsed time
  auto newTime = std::chrono::steady_clock::now();
  float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - theTime).count();
  theTime = newTime;
  m_mainCamera.SmoothMovement(1.0f/1000.0f);
  glfwPollEvents();

//   if(time > 15)
//   {
//     unsigned char* outBuffer = new unsigned char[width*height*4];
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, m_renderTarget2);
//
//     glGetTexImage(GL_TEXTURE_2D,
//                   0,
//                   GL_RGBA,
//                   GL_UNSIGNED_BYTE,
//                   outBuffer);
//
//     cimg_library::CImg<unsigned char> texCIMG(outBuffer, width, height, 1, 4);
//     cimg_library::CImgDisplay main_disp(texCIMG,"Click a point");
//     while (!main_disp.is_closed())
//     {
//       main_disp.wait();
//     }
//
//     delete [] outBuffer;
//   }
}

void SceneRenderer::RenderGBufferDebug(GLFWwindow* p_window)
{
  int width, height;
  glfwGetFramebufferSize(p_window, &width, &height);
  glDisable(GL_DEPTH_TEST);
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

  unsigned int lightLoc = m_renderShader.GetUniformLocation("light");
  unsigned int camLoc = m_renderShader.GetUniformLocation("cameraPos");
  glm::vec3 light(1.0, 1.0, 1.0);
  glUniform3fv(lightLoc, 1,  glm::value_ptr(light));
  glUniform3fv(camLoc, 1,  glm::value_ptr(m_mainCamera.GetPosition()));

  glBindVertexArray(m_renderVertexArray);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBufferId);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glfwSwapBuffers(p_window);
  glfwPollEvents();

  glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneRenderer::GenerateGBufferShader(rx::Mesh* p_mesh, rx::Material* p_material)
{
  unsigned int gBufferFlags = 0;

  rx::Material::StringMap const& uniforms = p_material->GetUniforms();

  if (uniforms.find("texture1") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::albedoTexture;
  }
  if (uniforms.find("texture3") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::specularTexture;
  }
  if (uniforms.find("texture6") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::normalTexture;
  }
  if (uniforms.find("diffuse_color") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::albedoColor;
  }
  if (uniforms.find("specular_color") != uniforms.cend())
  {
    gBufferFlags |= GBufferShaderFlag::specularColor;
  }
  if (p_mesh->HasUVCoords())
  {
    gBufferFlags |= GBufferShaderFlag::uvCoords;
  }

  std::string preprocSrc = GeneratePreprocessorDefine(gBufferFlags);
  GBufferShaderMap::iterator itShader = m_gbufferShaders.find(gBufferFlags);
  //Do not generate a new shader for a configuration that already exists
  if (itShader == m_gbufferShaders.end())
  {
    Shader& shaderGBuffer = m_gbufferShaders[gBufferFlags];
    shaderGBuffer.SetName("GBufferGenerationShader");
    shaderGBuffer.SetVertexShaderSrc("gbufferVS.shader");
    shaderGBuffer.SetFragmentShaderSrc("gbufferFS.shader");
    shaderGBuffer.SetPreprocessorConfig(preprocSrc);
    shaderGBuffer.LinkProgram();
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
  if (p_gBufferFlags & GBufferShaderFlag::uvCoords )
  {
    result << "#define HAS_UVCOORDS" <<std::endl;
  }
  return result.str();
}

#include "SimpleRenderer.hxx"
#include "EventDispatcher.hxx"

SimpleRenderer::SimpleRenderer():
mCamera(glm::vec3(0.0f, 0.0f, 0.0f)),
envMap(NULL),
mTime(std::chrono::steady_clock::now())
{
  EventDispatcher* dispatcher = EventDispatcher::Get();
  dispatcher->AddEventListener("MainCamera", &mCamera);
}

SimpleRenderer::~SimpleRenderer()
{
  for(unsigned i = 0; i < mRenderables.size(); ++i)
  {
    delete mRenderables[i];
  }
}

void SimpleRenderer::InitFbo()
{
  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
  
  glGenTextures(1, &mRenderTarget);
  glBindTexture(GL_TEXTURE_2D, mRenderTarget);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, 0);
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTarget, 0);
  
  unsigned int depthBuffer;
  glGenTextures(1, &depthBuffer);
  glBindTexture(GL_TEXTURE_2D, depthBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1920, 1080,
    0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
  
  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("Simple renderer frambuffer incomplete! ");
    assert(false);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void SimpleRenderer::InitS(rx::ResourcesHolderPtr pResourcesHolder)
{
  auto geoHandle = std::make_shared<SSPlaneData>();
  auto shader = pResourcesHolder->FindShader("screenspaceplane_shader");
  (*shader)->LinkProgram();
  
  mSSFinalRender = new Renderable(geoHandle, nullptr, *shader, nullptr);
}

void SimpleRenderer::InitPostprocess(rx::ResourcesHolderPtr pResourcesHolder)
{
  auto geoHandle = std::make_shared<SSPlaneData>();
  auto shader = pResourcesHolder->FindShader("tonemapping_shader");
  (*shader)->LinkProgram();
  
  mPostProcess = new Postprocess(geoHandle, *shader, GL_UNSIGNED_BYTE, 1920, 1080);
}

void SimpleRenderer::Init(rx::SceneGraphPtr pSceneGraph,
  rx::ResourcesHolderPtr pResourcesHolder)
{
  mHolder = pResourcesHolder;
  mSceneGraph = pSceneGraph;
  
  //SceneGraphTraversal
  std::list<rx::NodePtr> nodeToVisit;
  nodeToVisit.push_front(mSceneGraph->GetRoot());
  
  while( nodeToVisit.size() > 0 )
  {
    rx::NodePtr cNode = nodeToVisit.front();
    nodeToVisit.pop_front();
    
    //Do something with cNode
    if( cNode->Type().get() == rx::NodeType::Object )
    {
      auto objectNode = std::static_pointer_cast<rx::ObjectNode>(cNode);
      rxLogInfo("Visit object node with model " << objectNode->ModelRef()->GetName()
      << " and shader " <<  objectNode->ShaderRef()->GetName());
    
      auto model = objectNode->ModelRef();
      mCustomShader = objectNode->ShaderRef();
      mCustomShader->LinkProgram();
      for (unsigned int i = 0; i < model->GetMeshCount(); ++i)
      {        
        
        auto meshPtr = model->GetMesh(i);
        rx::MaterialPtr materialPtr = model->GetMaterialForMesh(i);
        assert(meshPtr != NULL && materialPtr != NULL);
        auto geoHandle = std::make_shared<GeometryHandle>(meshPtr);
        mGeoHandles[meshPtr->GetName()] = geoHandle;
        
        auto matTexHandle = std::make_shared<MaterialTextureHandle>(materialPtr);
        mMatTextureHandles[materialPtr->GetName()] = matTexHandle;
        
        Renderable* iRenderable = new Renderable(geoHandle, matTexHandle, mCustomShader, materialPtr);
        mRenderables.push_back(iRenderable);
      }
    }
    
    if( cNode->Type().get() == rx::NodeType::EnvironmentMap )
    {
      auto envMapNode = std::static_pointer_cast<rx::EnvironmentMapNode>(cNode);
      if( auto shader = pResourcesHolder->FindShader("cubemap_shader") )
      {
        (*shader)->LinkProgram();
        rx::MeshPtr baseCube = std::make_shared<rx::Cube>();
        envMap = new EnvironmentMap(baseCube, envMapNode->GetCubeMapMaterial(), *shader);
        envMap->PrepareBuffer();
      }
      else
      {
        rxLogError("No cubemap_shader loaded");
      }
    }
    
    //End do something    
    for(auto child : cNode->Childrens() )
    {
      nodeToVisit.push_front(child);
    }
  }
}

void SimpleRenderer::Render(GLFWwindow* pWindow)
{  
  auto newTime = std::chrono::steady_clock::now();
  float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - mTime).count();
  mCamera.MoveCamera(millis/1000.0f, 0.05);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);

  glm::mat4 projection = glm::perspective(glm::radians(60.0f),
    (float)width / (float)height, 0.001f, 1000.f);

  glm::mat4 view = glm::mat4(1.0f);

  view = glm::rotate(view, mCamera.GetElevation(), glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, mCamera.GetAzimuth(), glm::vec3(0.0f, 1.0f, 0.0f));

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glViewport(0, 0, width, height);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
  
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  
  if( envMap )
  {
    envMap->Draw(view, projection, model, glm::vec3(0.0, -1.0, 0.0),
      mCamera.GetPosition());
  }
  
  view = glm::translate(view, mCamera.GetPosition());
  
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  rx::GLSLTypeStore parameters;
  parameters.Set("Model", model);
  parameters.Set("View", view);
  parameters.Set("Projection", projection);

  for (unsigned int i = 0; i < mRenderables.size(); ++i)
  {    
    mRenderables[i]->Draw(parameters, {});
  }
  
  mTime = newTime;
  //glfwSwapBuffers(pWindow);
  //glfwPollEvents();
}

void SimpleRenderer::RenderToFbo(GLFWwindow* pWindow)
{
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffers);
  
  Render(pWindow);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);
  glViewport(0, 0, width, height);
  
  Postprocess::TextureParameter texParam;
  texParam["input_one"] = mRenderTarget;
  rx::GLSLTypeStore param;  
  mPostProcess->Execute(texParam, param);
  glfwSwapBuffers(pWindow);
  glfwPollEvents();
}





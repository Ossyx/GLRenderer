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
void SimpleRenderer::InitS(rx::ResourcesHolderPtr pResourcesHolder)
{
  auto geoHandle = std::make_shared<SSPlaneData>();
  auto material = pResourcesHolder->FindMaterial("waiting_screen");
  auto shader = pResourcesHolder->FindShader("waiting_shader");
  mCustomShader = *shader;
  mCustomShader->LinkProgram();
  rx::MaterialPtr materialPtr = *material;
  auto matTexHandle = std::make_shared<MaterialTextureHandle>(materialPtr);
  
  Renderable* iRenderable = new Renderable(geoHandle, matTexHandle, mCustomShader, materialPtr);
  mRenderables.push_back(iRenderable);
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

  for (unsigned int i = 0; i < mRenderables.size(); ++i)
  {
    glUseProgram(mCustomShader->GetProgram());
    mRenderables[i]->Draw(view, projection, model);
  }
  
  mTime = newTime;
  glfwSwapBuffers(pWindow);
  glfwPollEvents();
}




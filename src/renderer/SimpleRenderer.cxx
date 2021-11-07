#include "SimpleRenderer.hxx"
#include "EventDispatcher.hxx"

SimpleRenderer::SimpleRenderer():
envMap(NULL)
{
  EventDispatcher* dispatcher = EventDispatcher::Get();
  dispatcher->AddEventListener("MainCamera", &mCamera);
}

SimpleRenderer::~SimpleRenderer()
{  
  if( mSSFinalRender )
  {
    delete mSSFinalRender;
  }
  
  if( mPostProcess )
  {
    delete mPostProcess;
  }
}

void SimpleRenderer::InitFbo()
{
  mFBO.Build();
  mFBO.Bind();
  mFBO.AddRenderTarget("rt1", GL_COLOR_ATTACHMENT0, GL_RGBA32F,
    GL_RGBA, GL_FLOAT, 1920, 1080);
  mFBO.AddRenderTarget("depth", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F,
    GL_DEPTH_COMPONENT, GL_FLOAT, 1920, 1080);
  
  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("Simple renderer frambuffer incomplete! ");
    assert(false);
  }
  mFBO.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);
}

void SimpleRenderer::Initialize(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder)
{
  mHolder = pResourcesHolder;
  mSceneGraph = pSceneGraph;
  InitFbo();
  auto ssShader = mHolder->FindShader("screenspaceplane_shader");
  (*ssShader)->LinkProgram();  
  mSSFinalRender = new TextureDisplay(*ssShader);
  
  auto tmShader = mHolder->FindShader("tonemapping_shader");
  (*tmShader)->LinkProgram();  
  mPostProcess = new Postprocess(*tmShader, GL_UNSIGNED_BYTE, 1920, 1080);
  
  BuildRenderables();
  BuildEnvMap();
}

void SimpleRenderer::BuildEnvMap()
{
  //SceneGraphTraversal
  std::list<rx::NodePtr> nodeToVisit;
  nodeToVisit.push_front(mSceneGraph->GetRoot());
  
  while( nodeToVisit.size() > 0 )
  {
    rx::NodePtr cNode = nodeToVisit.front();
    nodeToVisit.pop_front();
    
    //Do something with cNode    
    if( cNode->Type().get() == rx::NodeType::EnvironmentMap )
    {
      auto envMapNode = std::static_pointer_cast<rx::EnvironmentMapNode>(cNode);
      if( auto shader = mHolder->FindShader("cubemap_shader") )
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

void SimpleRenderer::RenderSceneGraph(GLFWwindow* pWindow)
{  
  auto newTime = std::chrono::steady_clock::now();
  float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - mTime).count();
  mCamera.MoveCamera(millis/1000.0f, 0.05);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);

  glm::mat4 projection = glm::perspective(glm::radians(60.0f),
    (float)width / (float)height, 0.1f, 10000.f);

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
}

void SimpleRenderer::Render(GLFWwindow* pWindow)
{
  mFBO.Bind();
  glDrawBuffers(mFBO.mColorAttachments.size(), mFBO.mColorAttachments.data());  
  RenderSceneGraph(pWindow);  
  mFBO.Unbind();
  
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);
  glViewport(0, 0, width, height);
  
   Renderable::TextureParameter texParam;
   texParam["input_one"] = std::pair(GL_TEXTURE_2D, mFBO.mRenderTargets["rt1"].mId);
   rx::GLSLTypeStore param;
//   mPostProcess->Execute(texParam, param);
  mSSFinalRender->Draw(param, texParam);
  glfwSwapBuffers(pWindow);
  glfwPollEvents();
}





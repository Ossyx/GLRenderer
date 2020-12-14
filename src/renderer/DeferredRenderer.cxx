#include "DeferredRenderer.hxx"
#include "EventDispatcher.hxx"

DeferredRenderer::DeferredRenderer():
Renderer()
{
  EventDispatcher* dispatcher = EventDispatcher::Get();
  dispatcher->AddEventListener("MainCamera", &mCamera);
}

DeferredRenderer::~DeferredRenderer()
{
  if( mFinalRender )
  {
    delete mFinalRender;
  }
  if( mGBufferDebug )
  {
    delete mGBufferDebug;
  }
}

void DeferredRenderer::Initialize(rx::SceneGraphPtr pSceneGraph, rx::ResourcesHolderPtr pResourcesHolder)
{
  mHolder = pResourcesHolder;
  mSceneGraph = pSceneGraph;
  
  //Build list of renderable items
  BuildRenderables();
  
  //Build GBuffer FBO
  mGBuffer.Build();
  mGBuffer.Bind();
  mGBuffer.AddRenderTarget("rt_0", GL_COLOR_ATTACHMENT0, GL_RGBA,
    GL_RGBA, GL_UNSIGNED_BYTE, 1920, 1080);
  mGBuffer.AddRenderTarget("rt_1", GL_COLOR_ATTACHMENT1, GL_RGBA32F,
    GL_RGBA, GL_FLOAT, 1920, 1080);
  mGBuffer.AddRenderTarget("rt_2", GL_COLOR_ATTACHMENT2, GL_RGBA,
    GL_RGBA, GL_UNSIGNED_BYTE, 1920, 1080);
  mGBuffer.AddRenderTarget("rt_depth", GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32F,
    GL_DEPTH_COMPONENT, GL_FLOAT, 1920, 1080);
    
  if ( mGBuffer.CheckCompletion() == false )
  {
    rxLogError("GBuffer frambuffer incomplete! ");
    assert(false);
  }
  mGBuffer.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //Build Lighting FBO
  mLighting.Build();
  mLighting.Bind();
  mLighting.AddRenderTarget("rt_0", GL_COLOR_ATTACHMENT0, GL_RGBA,
    GL_RGBA, GL_UNSIGNED_BYTE, 1920, 1080);
  
  if ( mLighting.CheckCompletion() == false )
  {
    rxLogError("Lighting frambuffere incomplete! ");
    assert(false);
  }
  mGBuffer.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //Final render target
  auto ssShader = mHolder->FindShader("gbufferdebug");
  (*ssShader)->LinkProgram();  
  mGBufferDebug = new TextureDisplay(*ssShader);
  
  //Final render target
  auto lShader = mHolder->FindShader("lighting");
  (*lShader)->LinkProgram();  
  mFinalRender = new TextureDisplay(*lShader);
}

void DeferredRenderer::GBufferPass(GLFWwindow* pWindow)
{
  mGBuffer.Bind();
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, buffers);
  
  auto newTime = std::chrono::steady_clock::now();
  float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - mTime).count();
  mCamera.MoveCamera(millis/1000.0f, 0.05);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);

  glm::mat4 projection = glm::perspective(glm::radians(60.0f),
    (float)width / (float)height, 0.1f, 10000.f);
  
  mInvProjection = glm::inverse(projection);

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, mCamera.GetPosition());
  view = glm::rotate(view, mCamera.GetAzimuth(), glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::rotate(view, mCamera.GetElevation(), glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::inverse(view);

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
  
  mNormalMatrix = view * model;
  mNormalMatrix = glm::inverse(mNormalMatrix);
  mNormalMatrix = glm::transpose(mNormalMatrix);
  
  //For direction to camera space
  mInvView = view;
  mInvView = glm::inverse(mInvView);
  mInvView = glm::transpose(mInvView);
  
  
  rx::GLSLTypeStore parameters;
  parameters.Set("Model", model);
  parameters.Set("View", view);
  parameters.Set("Projection", projection);
  parameters.Set("NormalMatrix", mNormalMatrix);

  for (unsigned int i = 0; i < mRenderables.size(); ++i)
  {    
    mRenderables[i]->Draw(parameters, {});
  }
  
  mTime = newTime;
  mGBuffer.Unbind();
}

void DeferredRenderer::LightingPass(GLFWwindow* pWindow)
{
}

void DeferredRenderer::Render(GLFWwindow* pWindow)
{
  //Render to GBuffer
  GBufferPass(pWindow);
  
  //Combine GBuffer + lights
  //LightingPass(pWindow);
  
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);
  glViewport(0, 0, width, height);  
  
  rx::GLSLTypeStore param;
  param.Set("invProjection", mInvProjection);
  glm::vec4 light  = mInvView * glm::vec4(0.0, 1.0, 0.0, 0.0);
  param.Set("lightDirection", glm::normalize(glm::vec3(light.x, light.y, light.z)));
  
  Postprocess::TextureParameter gbdTexParam;
  gbdTexParam["rt_0"] = mGBuffer.mRenderTargets["rt_0"].mId;
  gbdTexParam["rt_1"] = mGBuffer.mRenderTargets["rt_1"].mId;
  gbdTexParam["rt_2"] = mGBuffer.mRenderTargets["rt_2"].mId;
  gbdTexParam["rt_depth"] = mGBuffer.mRenderTargets["rt_depth"].mId;
  mFinalRender->Draw(param, gbdTexParam);
  glfwSwapBuffers(pWindow);
  glfwPollEvents();
}



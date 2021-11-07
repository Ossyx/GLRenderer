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
//   mGBuffer.AddRenderTarget("rt_3", GL_COLOR_ATTACHMENT3, GL_R32F,
//       GL_R, GL_FLOAT, 1920, 1080);
  
  glGenRenderbuffers(1, &renderStencil); // stencil buffer
  glBindRenderbuffer(GL_RENDERBUFFER, renderStencil);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderStencil);

    
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
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderStencil);
  
  if ( mLighting.CheckCompletion() == false )
  {
    rxLogError("Lighting frambuffer incomplete! ");
    assert(false);
  }
  
  mLighting.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //Final render target
  auto ssShader = mHolder->FindShader("gbufferdebug");
  (*ssShader)->LinkProgram();  
  mGBufferDebug = new TextureDisplay(*ssShader);
  
  //Final render target
  auto lShader = mHolder->FindShader("lighting");
  (*lShader)->LinkProgram();  
  mLightingPP = new TextureDisplay(*lShader);
  
  auto fShader = mHolder->FindShader("screenspaceplane");
  (*fShader)->LinkProgram();  
  mFinalRender = new TextureDisplay(*fShader);
  
  //Load an envmap
  auto matLight = pResourcesHolder->FindMaterial("cubemap_c1");
  if( matLight )
  {
    mEnvMap = std::make_shared<EnvironmentMapBase>(*matLight);
  }
  else
  {
    rxLogError("Cannot find material for cube map ");
  }
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

  glDisable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_ZERO, GL_REPLACE, GL_REPLACE);

  glViewport(0, 0, width, height);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
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
  mLighting.Bind();
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffers);
  
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);
  glViewport(0, 0, width, height);  
  
  rx::GLSLTypeStore param;
  param.Set("invProjection", mInvProjection);
  param.Set("VToW", glm::inverse(mInvView));
  glm::vec4 light  = mInvView * glm::vec4(1.0, 1.0, 1.0, 0.0);
  param.Set("lDir", glm::normalize(glm::vec3(light.x, light.y, light.z)));
  
  Renderable::TextureParameter gbdTexParam;
  gbdTexParam["rt_0"] = std::pair(GL_TEXTURE_2D, mGBuffer.mRenderTargets["rt_0"].mId);
  gbdTexParam["rt_1"] = std::pair(GL_TEXTURE_2D, mGBuffer.mRenderTargets["rt_1"].mId);
  gbdTexParam["rt_2"] = std::pair(GL_TEXTURE_2D, mGBuffer.mRenderTargets["rt_2"].mId);
  gbdTexParam["cubemap"] = std::pair(GL_TEXTURE_CUBE_MAP, mEnvMap->GetTextureIndex());
  mLightingPP->Draw(param, gbdTexParam);  
  mLighting.Unbind();
}

void DeferredRenderer::Render(GLFWwindow* pWindow)
{
  //Render to GBuffer
  GBufferPass(pWindow);
  
  //Combine GBuffer + lights
  LightingPass(pWindow);
  
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glClearColor(0.25, 0.5, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glDisable(GL_STENCIL_TEST);

  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);
  glViewport(0, 0, width, height);  
  
  rx::GLSLTypeStore param;  
  Renderable::TextureParameter gbdTexParam;
  gbdTexParam["input_one"] = std::pair(GL_TEXTURE_2D, mLighting.mRenderTargets["rt_0"].mId);
  mFinalRender->Draw(param, gbdTexParam);
  glfwSwapBuffers(pWindow);
  glfwPollEvents();
}



#include "SimpleRenderer.hxx"
#include "EventDispatcher.hxx"

SimpleRenderer::SimpleRenderer():
mCamera(Camera()),
mTime(std::chrono::steady_clock::now())
{
  EventDispatcher* dispatcher = EventDispatcher::Get();
  dispatcher->AddEventListener("MainCamera", &mCamera);
}

SimpleRenderer::~SimpleRenderer()
{
  for(unsigned i = 0; i < mItems.size(); ++i)
  {
    delete mItems[i];
  }
}

void SimpleRenderer::Init(rx::SceneGraphPtr pSceneGraph,
  rx::ResourcesHolderPtr pResourcesHolder)
{
  mHolder = pResourcesHolder;
  mSceneGraph = pSceneGraph;
  
  if(auto m = mHolder->FindModel("fan"))
  {
    auto model = *m;
    for (unsigned int i = 0; i < model->GetMeshCount(); ++i)
    {
      DrawableItem* item = new DrawableItem();
      auto meshPtr = model->GetMesh(i);
      rx::MaterialPtr materialPtr = model->GetMaterialForMesh(i);
      assert(meshPtr != NULL && materialPtr != NULL);
      
      if(auto s = mHolder->FindShader("fan_custom_shader"))
      {
        mCustomShader = *s;
        mCustomShader->LinkProgram();
        item->PrepareBuffer(*meshPtr, *materialPtr, *mCustomShader);      
        mItems.push_back(item);
        mMaterials.push_back(materialPtr);
      }
    }
  }
  else
  {
    rxLogError("No fan model");
    assert(false);
  }
}

void SimpleRenderer::Render(GLFWwindow* pWindow)
{  
  auto newTime = std::chrono::steady_clock::now();
  float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - mTime).count();
  mCamera.MoveCamera(millis/1000.0f, 0.05);
  
  int width, height;
  glfwGetFramebufferSize(pWindow, &width, &height);
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),
    (float)width / (float)height, 0.001f, 1000.f);

  glm::mat4 view = glm::mat4(1.0f);

  view = glm::rotate(view, mCamera.GetElevation(), glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, mCamera.GetAzimuth(), glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::translate(view, mCamera.GetPosition());

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
  model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
  model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
  
  for (unsigned int i = 0; i < mItems.size(); ++i)
  {
    rx::MaterialPtr materialPtr = mMaterials[i];
    glUseProgram(mCustomShader->GetProgram());
    mItems[i]->SetTransform(model);
    mItems[i]->Draw(*mCustomShader, *materialPtr, view, projection,
      model, glm::vec3(0.0, -1.0, 0.0) , mCamera.GetPosition());
  }  
  mTime = newTime;
  glfwSwapBuffers(pWindow);
  glfwPollEvents();
}




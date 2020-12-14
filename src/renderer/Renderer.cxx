#include "Renderer.hxx"

Renderer::Renderer():
mCamera(glm::vec3(0.0f, 0.0f, 0.0f)),
mTime(std::chrono::steady_clock::now())
{
}

Renderer::~Renderer()
{
  for(unsigned i = 0; i < mRenderables.size(); ++i)
  {
    delete mRenderables[i];
  }
}


void Renderer::BuildRenderables()
{
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
      auto objectShader = objectNode->ShaderRef();
      auto materialShader = objectNode->MaterialShaderRef();
      objectShader->LinkProgram();
      materialShader->BuildDefaultTexture(objectShader);
      for (unsigned int i = 0; i < model->GetMeshCount(); ++i)
      {        
        auto meshPtr = model->GetMesh(i);
        rx::MaterialPtr materialPtr = model->GetMaterialForMesh(i);
        assert(meshPtr != NULL && materialPtr != NULL);
        auto geoHandle = std::make_shared<GeometryHandle>(meshPtr);
        mGeoHandles[meshPtr->GetName()] = geoHandle;
        
        auto matTexHandle = std::make_shared<MaterialTextureHandle>(materialPtr);
        mMatTextureHandles[materialPtr->GetName()] = matTexHandle;
        
        Renderable* iRenderable = new Renderable(geoHandle, matTexHandle, objectShader, materialPtr, materialShader);
        mRenderables.push_back(iRenderable);
      }
    }    
    //End do something    
    for(auto child : cNode->Childrens() )
    {
      nodeToVisit.push_front(child);
    }
  }
}

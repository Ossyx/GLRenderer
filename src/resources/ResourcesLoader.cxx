#include "ResourcesLoader.hxx"
#include <fstream>
#include <filesystem>

namespace rx
{
  
ResourcesLoader::ResourcesLoader()
{
}

ResourcesLoader::~ResourcesLoader()
{  
}

void ResourcesLoader::LoadDescription(std::string const& pResourceIndexPath, ResourcesHolder& pHolder)
{
  std::ifstream inputStreamJsonMap;
  inputStreamJsonMap.open(pResourceIndexPath);
  
  if( inputStreamJsonMap.is_open() == false)
  {
    rxLogError( "Cannot find resources index file " << pResourceIndexPath );
    assert(false);
  }

  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(inputStreamJsonMap, root );
  if (parsingSuccessful == false)
  {
      // report to the user the failure and their locations in the document.
    rxLogError("Fail to parse resources index file : "<<reader.getFormattedErrorMessages());
    assert(false);
  }
  LoadDescription(root, pHolder);
}

void ResourcesLoader::LoadDescription(Json::Value& pJsonDescription, ResourcesHolder& pHolder)
{
  //Instantiate resources in the resources holder
  Json::Value& resourceTab = pJsonDescription["resources"];
  Json::Value::iterator itResource = resourceTab.begin();
  
  for(; itResource != resourceTab.end(); ++itResource)
  {
    Json::Value& resource = *itResource;
    std::string type = resource["type"].asString();
    
    ResourceDescription res(ResourceType(type), resource);
    pHolder.AddResource(res, resource["id"].asString());
  }  
}

void ResourcesLoader::LoadResources(ResourcesHolder& pHolder)
{
  ResourcesHolder::ResourceMapType const& resources = pHolder.GetResources();
  auto itResource = resources.cbegin();
  for( ; itResource != resources.cend(); ++itResource )
  {
    rxLogInfo("Loading resource..");
    ResourceDescription const& res = itResource->second;
    rxLogInfo(res.AsString());
    
    switch(res.mType.get())
    {
      case ResourceType::Model:
        LoadModel(res, pHolder);
        break;
      case ResourceType::Texture:
        LoadTexture(res, pHolder);
        break;
      case ResourceType::ShaderStack:
        LoadShaderStack(res, pHolder);
        break;
      case ResourceType::Material:
        LoadMaterial(res, pHolder);
        break;
      case ResourceType::Unknown:
        rxLogWarning("Try to load Unknown resource type, skipping");
        break;
    }
  }
}

void ResourcesLoader::LoadModel(ResourceDescription const& pDesc, ResourcesHolder& pHolder)
{
  auto model = rx::ModelLoader::LoadOBJModel(pDesc.mData["path"].asString(),
                                        pDesc.mData["name"].asString());
  pHolder.RegisterModel(model, pDesc.mData["id"].asString());
}

void ResourcesLoader::LoadTexture(ResourceDescription const& pDesc, ResourcesHolder& pHolder)
{
  auto tex = std::make_shared<rx::Texture<unsigned char>>();
  rx::ModelLoader::LoadTextureFromFile<unsigned char>(pDesc.mData["path"].asString(), *tex);
}

void ResourcesLoader::LoadShaderStack(const ResourceDescription& pDesc, ResourcesHolder& pHolder)
{
  auto shader = std::make_shared<Shader>(pDesc.mData["name"].asString());
  Json::Value const& shaderSources = pDesc.mData["shader_source"];
  if( shaderSources.isMember("vertex_shader") )
  {
    shader->SetVertexShaderSrc(shaderSources["vertex_shader"].asString());
  }
  if( shaderSources.isMember("fragment_shader") )
  {
    shader->SetFragmentShaderSrc(shaderSources["fragment_shader"].asString());
  }
  if( shaderSources.isMember("tesselationcontrol_shader") )
  {
    shader->SetTessControlSrc(shaderSources["tesselationcontrol_shader"].asString());
  }
  if( shaderSources.isMember("tesselationevaluation_shader") )
  {
    shader->SetTessEvalSrc(shaderSources["tesselationevaluation_shader"].asString());
  }
  //Do I link here ?
  //shader->LinkProgram();
  pHolder.RegisterShader(shader, pDesc.mData["id"].asString());
}

void ResourcesLoader::LoadMaterial(const ResourceDescription& pDesc, ResourcesHolder& pHolder)
{
  auto materials = rx::ModelLoader::LoadMaterialCollection(pDesc.mData["material_file"].asString());
  for(rx::MaterialPtr mat: materials)
  {
    pHolder.RegisterMaterial(mat, mat->GetName());
  }
}

}






#include "ResourcesHolder.hxx"

namespace rx
{
  
ResourcesHolder::ResourcesHolder()
{
}

ResourcesHolder::~ResourcesHolder()
{  
}

ResourcesHolder::ResourceMapType const& ResourcesHolder::GetResources() const
{
  return mResources;
}

void ResourcesHolder::AddResource(const ResourceDescription& pResource, KeyType const& pId)
{
  mResources[pId] = pResource;
}

void ResourcesHolder::RegisterModel(ModelPtr pModel, KeyType const& pId)
{
  mModelMap[pId] = pModel;
}

void ResourcesHolder::RegisterMaterial(MaterialPtr pMaterial, KeyType const& pId)
{
  mMaterials[pId] = pMaterial;
}

void ResourcesHolder::RegisterShader(
  ResourcesHolder::ShaderPtr pShader, KeyType const& pId)
{
  mShaders[pId] = pShader;
}

std::optional<ModelPtr> ResourcesHolder::FindModel(KeyType const& pId)
{
  auto modelPtrIterator = mModelMap.find(pId);
  if( modelPtrIterator != mModelMap.end() )
  {
    return modelPtrIterator->second;
  }
  else
  {
    return {};
  }
}

std::optional<MaterialPtr> ResourcesHolder::FindMaterial(KeyType const& pId)
{
  auto matPtrIt = mMaterials.find(pId);
  if( matPtrIt != mMaterials.end() )
  {
    return matPtrIt->second;
  }
  else
  {
    return {};
  }
}

std::optional<ResourcesHolder::ShaderPtr> ResourcesHolder::FindShader(KeyType const& pId)
{
  auto shaderPtrIt = mShaders.find(pId);
  if( shaderPtrIt != mShaders.end() )
  {
    return shaderPtrIt->second;
  }
  else
  {
    return {};
  }
}

}

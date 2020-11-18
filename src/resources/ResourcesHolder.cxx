#include "ResourcesHolder.hxx"

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

void ResourcesHolder::AddResource(const ResourceDescription& pResource, unsigned int pId)
{
  mResources[pId] = pResource;
}

void ResourcesHolder::RegisterModel(ResourcesHolder::ModelPtr pModel, unsigned int pId)
{
  mModelMap[pId] = pModel;
}

std::optional<ResourcesHolder::ModelPtr> ResourcesHolder::FindModel(unsigned int pId)
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






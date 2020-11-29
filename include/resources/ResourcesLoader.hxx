#ifndef RESOURCESLOADER_HXX
#define RESOURCESLOADER_HXX

#include "ResourceDescription.hxx"
#include "ResourcesHolder.hxx"
#include "ModelLoader.hxx"
#include "Shader.hxx"

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>

namespace rx
{
  
class ResourcesLoader
{
public:  
  enum LoadingStatus
  {
    Unloaded,
    Loading,
    Loaded
  };
  
  ResourcesLoader();
  ~ResourcesLoader();  
  void LoadDescription(std::string const& pResourceIndexPath, ResourcesHolder& pHolder);
  void LoadResources(ResourcesHolder& pHolder, bool pAsync = false);
  LoadingStatus GetStatus();
  
private:
  void SetStatus(LoadingStatus pStatus);
  void LoadDescription(Json::Value& pJsonDescription, ResourcesHolder& pHolder);
  void LoadTexture(ResourceDescription const& pDesc,ResourcesHolder& pHolder);
  void LoadModel(ResourceDescription const& pDesc,ResourcesHolder& pHolder);
  void LoadShaderStack(ResourceDescription const& pDesc,ResourcesHolder& pHolder);
  void LoadMaterial(ResourceDescription const& pDesc,ResourcesHolder& pHolder);
  
  std::mutex mSmut;
  LoadingStatus mStatus;
  std::thread mLoadingThread;
};

}
#endif

#ifndef RESOURCESHOLDER_HXX
#define RESOURCESHOLDER_HXX

#include <unordered_map>
#include <memory>

#include "ResourceDescription.hxx"

#include "Mesh.hxx"
#include "Material.hxx"
#include "Model.hxx"
#include "Shader.hxx"
#include "MaterialShader.hxx"

namespace rx
{
 
class ResourcesHolder
{
public:  
  
  ResourcesHolder();
  ~ResourcesHolder();
  
  using ByteTexturePtr = std::shared_ptr<rx::Texture<unsigned char>>;
  
  using KeyType = std::string;
  
  using ResourceMapType = std::unordered_map<KeyType, ResourceDescription>;
  using MeshMapType = std::unordered_map<KeyType, MeshPtr >;
  using ModelMapType = std::unordered_map<KeyType, ModelPtr >;
  using MaterialMapType = std::unordered_map<KeyType, MaterialPtr >;
  using ShaderMapType = std::unordered_map<KeyType, ShaderPtr >;
  using MaterialShaderMapType = std::unordered_map<KeyType, MaterialShaderPtr >;  
  
  ResourceMapType const& GetResources() const;
  ResourceDescription const& GetResource(KeyType const& pId) const;
  void AddResource(ResourceDescription const& pResource, KeyType const& pId);

  void RegisterModel(ModelPtr pModel, KeyType const& pId);
  void RegisterMaterial(MaterialPtr pMaterial, KeyType const& pId);
  void RegisterShader(ShaderPtr pShader, KeyType const& pId);
  void RegisterMaterialShader(MaterialShaderPtr pMatShader, KeyType const& pId);
  
  std::optional<ModelPtr> FindModel(KeyType const& pId);
  std::optional<MaterialPtr> FindMaterial(KeyType const& pName);
  std::optional<ShaderPtr> FindShader(KeyType const& pId);
  std::optional<MaterialShaderPtr> FindMaterialShader(KeyType const& pId);
      
private:
  
  ResourceMapType mResources;
  
  MeshMapType mMeshMap;
  ModelMapType mModelMap;
  MaterialMapType mMaterials;
  ShaderMapType mShaders;
  MaterialShaderMapType mMaterialShaders;
};

using ResourcesHolderPtr = std::shared_ptr<ResourcesHolder>;

}

#endif

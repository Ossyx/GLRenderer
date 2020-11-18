#ifndef RESOURCESHOLDER_HXX
#define RESOURCESHOLDER_HXX

#include <unordered_map>
#include <memory>

#include "ResourceDescription.hxx"

#include "Mesh.hxx"
#include "Material.hxx"
#include "Model.hxx"
#include "Shader.hxx"

class ResourcesHolder
{
public:  
  
  ResourcesHolder();
  ~ResourcesHolder();
  
  using MeshPtr = std::shared_ptr<rx::Mesh>;
  using ModelPtr = std::shared_ptr<rx::Model>;
  using MaterialPtr = std::shared_ptr<rx::Material>;
  using ShaderPtr = std::shared_ptr<Shader>;
  using ByteTexturePtr = std::shared_ptr<rx::Texture<unsigned char>>;
  
  using ResourceMapType = std::unordered_map<unsigned int, ResourceDescription>;
  using MeshMapType = std::unordered_map<unsigned int, MeshPtr >;
  using ModelMapType = std::unordered_map<unsigned int, ModelPtr >;
  using MaterialMapType = std::unordered_map<unsigned int, MaterialPtr >;
  using ShaderMapType = std::unordered_map<unsigned int, ShaderPtr >;  
  
  ResourceMapType const& GetResources() const;
  ResourceDescription const& GetResource(unsigned int pId) const;
  void AddResource(ResourceDescription const& pResource, unsigned int pId);

  void RegisterModel(ModelPtr pModel, unsigned int pId);
  std::optional<ModelPtr> FindModel(unsigned int pId);
      
private:
  
  ResourceMapType mResources;
  
  MeshMapType mMeshMap;
  ModelMapType mModelMap;
  MaterialMapType mMaterial;
  ShaderMapType mShaders;
};

#endif

#ifndef ENVIRONMENTMAP_HXX
#define ENVIRONMENTMAP_HXX

#include "DrawableItem.hxx"
#include "Material.hxx"
#include "Primitives.hxx"

class EnvironmentMapBase
{
public:
  EnvironmentMapBase(rx::MaterialPtr pMaterial);
  virtual ~EnvironmentMapBase();  
  unsigned int GetTextureIndex();
  
private:
  
  void Load();
  unsigned int mCubeMapTex;
  rx::MaterialPtr mMaterial;
};

using EnvironmentMapBasePtr = std::shared_ptr<EnvironmentMapBase>;

class EnvironmentMap : public DrawableItem
{
public:

  EnvironmentMap(rx::MeshPtr pMesh, rx::MaterialPtr pMaterial, ShaderPtr pShader);
  ~EnvironmentMap(); 
  virtual void Draw(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos);
  
protected:

  virtual void PrepareTextureFromMaterial();
  
private:

  unsigned int mCubeMapTex;
  unsigned int mCubeMapLocation;
};

#endif

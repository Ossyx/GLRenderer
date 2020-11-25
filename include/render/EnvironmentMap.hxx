#ifndef ENVIRONMENTMAP_HXX
#define ENVIRONMENTMAP_HXX

#include "DrawableItem.hxx"
#include "Material.hxx"
#include "Primitives.hxx"

class EnvironmentMap : public DrawableItem
{
public:

  EnvironmentMap(rx::MaterialPtr pCubeMapMaterial, ShaderPtr pShader);
  ~EnvironmentMap(); 
  void InitCubeMap();
  void DrawCubeMap(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model);
  
protected:

  virtual void PrepareTextureFromMaterial(rx::Material const& p_material,
  Shader const& p_shader);
  
private:

  rx::MaterialPtr mCubeMapMaterial;  
  rx::Cube mCubeMesh;
  ShaderPtr mShader;
  unsigned int mCubeMapTex;
  unsigned int mCubeMapLocation;
};

#endif

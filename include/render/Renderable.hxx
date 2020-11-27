#ifndef RENDERABLE_HXX
#define RENDERABLE_HXX

#include "GeometryHandle.hxx"
#include "MaterialTexturesHandle.hxx"
#include "Shader.hxx"

#include <glm/mat4x4.hpp> 

class Renderable
{
public:
  Renderable(GeometryHandlePtr pGeoHandle, MaterialTextureHandlePtr pMatTexHandle,
             ShaderPtr pShader, rx::MaterialPtr pMaterial);
  ~Renderable();
  
  virtual void Draw(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model);

private:
  
  VertexArray mVertexArray;
  GeometryHandlePtr mGeometryHandle;
  MaterialTextureHandlePtr mMaterialTextureHandle;
  ShaderPtr mShader;
  rx::MaterialPtr mMaterial;
};
#endif

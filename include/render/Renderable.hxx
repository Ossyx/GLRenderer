#ifndef RENDERABLE_HXX
#define RENDERABLE_HXX

#include "GeometryHandle.hxx"
#include "MaterialTexturesHandle.hxx"
#include "Shader.hxx"

#include <glm/mat4x4.hpp>

class Renderable
{
public:
  using TextureParameter = std::unordered_map<std::string, unsigned int>;
  
  Renderable(GeometryHandlePtr pGeoHandle, MaterialTextureHandlePtr pMatTexHandle,
             ShaderPtr pShader, rx::MaterialPtr pMaterial);
  virtual ~Renderable();
  
  void SetMaterialUniforms();
  void SetUniforms(rx::GLSLTypeStore const& pParameters);
  void SetTextureUniforms(TextureParameter const& pTexParameters);
  
  virtual void Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters);

protected:
  
  VertexArray mVertexArray;
  GeometryHandlePtr mGeometryHandle;
  MaterialTextureHandlePtr mMaterialTextureHandle;
  ShaderPtr mShader;
  rx::MaterialPtr mMaterial;
  
  int mCurrentIdTexture;
};
#endif

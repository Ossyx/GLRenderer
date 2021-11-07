#ifndef RENDERABLE_HXX
#define RENDERABLE_HXX

#include "GeometryHandle.hxx"
#include "MaterialTexturesHandle.hxx"
#include "Shader.hxx"
#include "MaterialShader.hxx"
#include "GLAbstraction.hxx"

#include <glm/mat4x4.hpp>

class Renderable
{
public:
  using TextureParameter = std::unordered_map<std::string, std::pair<GLenum, unsigned int>>;
  
  Renderable(GeometryHandlePtr pGeoHandle, MaterialTextureHandlePtr pMatTexHandle,
             ShaderPtr pShader, rx::MaterialPtr pMaterial, MaterialShaderPtr pMaterialShader);
  virtual ~Renderable();
  
  bool Validate();
  void SetMaterialUniforms(Shader::UniformMap& pUniforms);
  void SetTextureUniforms(TextureParameter const& pTexParameters);
  void SetParametersUniforms(rx::GLSLTypeStore const& pParameters);
  template <typename T>
  void SetMaterialUniform(std::string const& pUniform, std::string const& pSource,
    rx::GLSLTypeStore const& pDefaultValues);
  
  virtual void Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters);

protected:
  
  VertexArray mVertexArray;
  GeometryHandlePtr mGeometryHandle;
  MaterialTextureHandlePtr mMaterialTextureHandle;
  ShaderPtr mShader;
  rx::MaterialPtr mMaterial;
  MaterialShaderPtr mMaterialShader;
  
  int mCurrentIdTexture;
  
};

template <typename T>
void Renderable::SetMaterialUniform(std::string const& pUniform, std::string const& pSource, 
  rx::GLSLTypeStore const& pDefaultValues)
{
  if( mMaterial->Has<T>(pSource) )
  {
    mShader->SetUniform(pUniform, mMaterial->Get<T>(pSource));
  }
  else if( pDefaultValues.Has<T>(pUniform) )
  {
    mShader->SetUniform(pUniform, pDefaultValues.Get<T>(pUniform));
  }
  else
  {
    rxLogError("No value in material and in default values for uniform "
      << pUniform <<" and source "<< pSource);
    assert(false);
  }
}
#endif

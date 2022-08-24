#ifndef MATERIALSHADER_HXX
#define MATERIALSHADER_HXX

#include "GLSLTypeStore.hxx"
#include "GLAbstraction.hxx"
#include "Shader.hxx"
#include <unordered_map>
#include <memory>
#include "GLRendererExport.hxx"

class GLRenderer_EXPORT MaterialShader
{
public:
  MaterialShader();
  virtual ~MaterialShader();
  //Uniform -> material attribute
  using StringStringMap = std::unordered_map<std::string, std::string>;
  
  std::string GetName() const;
  
  void SetName(std::string const& pName);
  
  void SetUniformSource(std::string const& pUniform, std::string const& pSource);
  
  std::string GetUniformSource(std::string const& pUniform);
  
  bool HasUniformSource(std::string const& pUniform);
  
  template <typename T>
  void SetDefaultValue(std::string const& pUniform, T pValue);
  
  template <typename T>
  T GetDefaultValue(std::string const& pUniform);
  
  template <typename T>
  bool HasDefaultValue(std::string const& pUniform);
  
  rx::GLSLTypeStore const& GetDefaultStore();
    
  void BuildDefaultTexture(ShaderPtr pShader);
  
  bool HasDefaultTexture(std::string const& pUniform);
  
  GLTexture GetDefaultTexture(std::string const& pUniform);
  
private:
  std::string mName;
  rx::GLSLTypeStore mDefaultValues;
  std::unordered_map<std::string, GLTexture> mDefaultTextures;
  StringStringMap mUniformMapping;
};

template <typename T>
void MaterialShader::SetDefaultValue(std::string const& pUniform, T pValue)
{
  mDefaultValues.Set<T>(pUniform, pValue);
}

template <typename T>
T MaterialShader::GetDefaultValue(std::string const& pUniform)
{
  return mDefaultValues.Get<T>(pUniform);
}

template <typename T>
bool MaterialShader::HasDefaultValue(std::string const& pUniform)
{
  return mDefaultValues.Has<T>(pUniform);
}

using MaterialShaderPtr = std::shared_ptr<MaterialShader>;

#endif

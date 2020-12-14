#include "MaterialShader.hxx"
#include <glm/gtc/type_ptr.hpp>

MaterialShader::MaterialShader()
{
}

MaterialShader::~MaterialShader()
{
}

void MaterialShader::SetName(std::string const& pName)
{
  mName = pName;
}


std::string MaterialShader::GetName() const
{
  return mName;
}

std::string MaterialShader::GetUniformSource(std::string const& pUniform)
{
  return mUniformMapping[pUniform];
}

bool MaterialShader::HasUniformSource(std::string const& pUniform)
{
  return mUniformMapping.find(pUniform) != mUniformMapping.end();
}

void MaterialShader::SetUniformSource(std::string const& pUniform, std::string const& pSource)
{
  mUniformMapping[pUniform] = pSource;
}

rx::GLSLTypeStore const& MaterialShader::GetDefaultStore()
{
  return mDefaultValues;
}

void MaterialShader::BuildDefaultTexture(ShaderPtr pShader)
{
  for(auto uniform : mDefaultValues.GetMembers())
  {
    auto itUniform = pShader->GetUniformMap().find(uniform);
    if( itUniform != pShader->GetUniformMap().end() )
    {
      if( itUniform->second.first == GL_SAMPLER_2D )
      {
        rxLogInfo("Default uniform " << uniform << " is a sampler 2d.");
        if( mDefaultValues.Has<glm::vec4>(uniform) )
        {
          rxLogInfo("With default value vec4.");
          GLTexture t;
          t.Build();
          t.Bind();
          t.SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
          t.SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
          t.SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          t.SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glm::vec4 data = mDefaultValues.Get<glm::vec4>(uniform);
          t.SetData(GL_RGBA, GL_RGBA, GL_FLOAT, 1, 1, glm::value_ptr(data));
          mDefaultTextures[uniform] = t;
        }
      }
    }
  }
}

bool MaterialShader::HasDefaultTexture(const std::string& pUniform)
{
  return mDefaultTextures.find(pUniform) != mDefaultTextures.end();
}

GLTexture MaterialShader::GetDefaultTexture(const std::string& pUniform)
{
  return mDefaultTextures[pUniform];
}






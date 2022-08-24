#include "Renderable.hxx"

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderable::Renderable(GeometryHandlePtr pGeoHandle,
  MaterialTextureHandlePtr pMatTexHandle, ShaderPtr pShader, rx::MaterialPtr pMaterial,
  MaterialShaderPtr pMaterialShader):
  mGeometryHandle(pGeoHandle),
  mMaterialTextureHandle(pMatTexHandle),
  mShader(pShader),
  mMaterial(pMaterial),
  mMaterialShader(pMaterialShader)
{
  //Build VAO
  unsigned int vpos_location = mShader->GetAttributeLocation("vPos");
  unsigned int normal_location = mShader->GetAttributeLocation("normal");
  unsigned int uv_location = mShader->GetAttributeLocation("uvcoords");
  
  mVertexArray.Build();
  mVertexArray.Bind();  
  mVertexArray.BindBufferToLocation(mGeometryHandle->mVertex, vpos_location,
    3, GL_FLOAT, GL_FALSE, 0, NULL);
  mVertexArray.BindBufferToLocation(mGeometryHandle->mNormal, normal_location,
    3, GL_FLOAT, GL_FALSE, 0, NULL);

  if (mGeometryHandle->mHasTangents)
  {
    unsigned int tangent_location = mShader->GetAttributeLocation("tangent");
    mVertexArray.BindBufferToLocation(mGeometryHandle->mTangent, tangent_location,
      3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  if (mGeometryHandle->mHasBitangents)
  {
    unsigned int bitangent_location = mShader->GetAttributeLocation("bitangent");
    mVertexArray.BindBufferToLocation(mGeometryHandle->mBitangent, bitangent_location,
      3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  mVertexArray.BindBufferToLocation(mGeometryHandle->mUV, uv_location,
    2, GL_FLOAT, GL_FALSE, 0, NULL);
}

Renderable::~Renderable()
{
}

bool Renderable::Validate()
{
  return true;
}

void Renderable::SetMaterialUniforms(Shader::UniformMap& pUniforms)
{
  auto const& texStore = mMaterialTextureHandle->TextureIds();
  rx::GLSLTypeStore const& defaultStore = mMaterialShader->GetDefaultStore();
  for ( auto& uniform : pUniforms)
  {
    std::string src = mMaterialShader->GetUniformSource(uniform.first);
    GLenum type = uniform.second.first;
    switch ( type )
    {
      case GL_FLOAT:
        SetMaterialUniform<float>(uniform.first, src, defaultStore);        
      break;
      case GL_FLOAT_VEC2:
        SetMaterialUniform<glm::vec2>(uniform.first, src, defaultStore);    
      break;
      case GL_FLOAT_VEC3:
        SetMaterialUniform<glm::vec3>(uniform.first, src, defaultStore);    
      break;
      case GL_FLOAT_VEC4:
        SetMaterialUniform<glm::vec4>(uniform.first, src, defaultStore);    
      break;
      case GL_SAMPLER_2D:
      {
        glActiveTexture(GL_TEXTURE0 + mCurrentIdTexture);
        auto itValue = texStore.find(src);
        if( itValue != texStore.end() )
        {
          glBindTexture(GL_TEXTURE_2D, itValue->second);
        }
        else
        {
          glBindTexture(GL_TEXTURE_2D, mMaterialShader->GetDefaultTexture(uniform.first).mId);
        }
        mShader->SetUniform(uniform.first, mCurrentIdTexture);
        ++mCurrentIdTexture;
      }
      break;
    }  
  }
}

void Renderable::SetTextureUniforms(TextureParameter const& pTexParameters)
{
  for (auto texInfo : pTexParameters )
  {    
    glActiveTexture(GL_TEXTURE0 + mCurrentIdTexture);
    glBindTexture(texInfo.second.first, texInfo.second.second);
    mShader->SetUniform(texInfo.first, mCurrentIdTexture);
    ++mCurrentIdTexture;
  }
}

void Renderable::SetParametersUniforms(rx::GLSLTypeStore const& pParameters)
{
  for(auto const& parameter : pParameters.GetMembers())
  {
    if ( pParameters.Has<glm::mat4>(parameter) )
    {
      mShader->SetUniform(parameter, pParameters.Get<glm::mat4>(parameter));
    }
    else if ( pParameters.Has<glm::vec3>(parameter) )
    {
      mShader->SetUniform(parameter, pParameters.Get<glm::vec3>(parameter));
    }
    else
    {
      rxLogError("Only mat4 as a global shader parameter");
      assert(false);
    }
  }
}

void Renderable::Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters)
{
  glUseProgram(mShader->GetProgram());
  Shader::UniformMap uniforms = mShader->GetUniformMap();
  
  //Global uniforms
  SetParametersUniforms(pParameters);
  for(auto const& parameter : pParameters.GetMembers())
  {
    uniforms.erase(parameter);
  }
  
  mCurrentIdTexture = 0;
  
  //Global textures
  for ( auto& texInfo : pTexParameters)
  {
    glActiveTexture(GL_TEXTURE0 + mCurrentIdTexture);
    glBindTexture(texInfo.second.first, texInfo.second.second);
    mShader->SetUniform(texInfo.first, mCurrentIdTexture);
    ++mCurrentIdTexture;
    uniforms.erase(texInfo.first);
  }
  
  if( mMaterial != nullptr && mMaterialTextureHandle != nullptr && mMaterialShader != nullptr)
  {
    SetMaterialUniforms(uniforms);
  }
  
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);
}

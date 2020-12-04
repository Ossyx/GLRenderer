#include "Renderable.hxx"

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderable::Renderable(GeometryHandlePtr pGeoHandle,
  MaterialTextureHandlePtr pMatTexHandle, ShaderPtr pShader, rx::MaterialPtr pMaterial):
  mGeometryHandle(pGeoHandle),
  mMaterialTextureHandle(pMatTexHandle),
  mShader(pShader),
  mMaterial(pMaterial)
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

void Renderable::SetUniforms(rx::GLSLTypeStore const& pParameters)
{
  for ( auto& uniformMat : mShader->GetUniformMap())
  {
      //Handle various uniform types
      GLenum type = uniformMat.second.first;
      switch ( type )
      {
        case GL_FLOAT:
          if (pParameters.Has<float>(uniformMat.first))
          {
            float unif = pParameters.Get<float>(uniformMat.first);
            mShader->SetUniform(uniformMat.first, unif);
          }
        break;
        case GL_FLOAT_VEC3:        
          if (pParameters.Has<glm::vec3>(uniformMat.first))
          {
            glm::vec3 univec3f = pParameters.Get<glm::vec3>(uniformMat.first);
            mShader->SetUniform(uniformMat.first, univec3f);
          }
        break;
      }
  }
}

void Renderable::SetMaterialUniforms()
{
  for ( auto& uniformMat : mShader->GetUniformMap())
  {
    std::string attributeKey;
    bool exists = mMaterial->GetUniformData(uniformMat.first, attributeKey);
    if (exists == true)
    {
      //Handle various uniform types
      GLenum type = uniformMat.second.first;
      switch ( type )
      {
        case GL_FLOAT:
          if (mMaterial->Has<float>(attributeKey))
          {
            float unif = mMaterial->Get<float>(attributeKey);
            mShader->SetUniform(attributeKey, unif);
          }
        break;
        case GL_FLOAT_VEC3:        
          if (mMaterial->Has<glm::vec3>(attributeKey))
          {
            glm::vec3 univec3f = mMaterial->Get<glm::vec3>(attributeKey);
            mShader->SetUniform(attributeKey, univec3f);
          }
        break;
      }
    }
  }
}

void Renderable::SetTextureUniforms(TextureParameter const& pTexParameters)
{
  for (auto texInfo : pTexParameters )
  {    
    glActiveTexture(GL_TEXTURE0 + mCurrentIdTexture);
    glBindTexture(GL_TEXTURE_2D, texInfo.second);
    mShader->SetUniform(texInfo.first, mCurrentIdTexture);
    ++mCurrentIdTexture;
  }
}

void Renderable::Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters)
{
  glUseProgram(mShader->GetProgram());
  mShader->SetUniform("View", pParameters.Get<glm::mat4>("View"));
  mShader->SetUniform("Projection", pParameters.Get<glm::mat4>("Projection"));
  mShader->SetUniform("Model", pParameters.Get<glm::mat4>("Model"));

  mCurrentIdTexture = 0;
  if( mMaterial != nullptr && mMaterialTextureHandle != nullptr)
  {
    //Set up the uniforms from material that are not textures
    SetMaterialUniforms();
    //Bind our textures
    SetTextureUniforms(mMaterialTextureHandle->TextureIds());
  }
  
  SetTextureUniforms(pTexParameters);
  
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);
}

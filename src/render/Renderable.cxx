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

void Renderable::Draw(const glm::mat4& p_view, const glm::mat4& p_projection, const glm::mat4& p_model)
{
  unsigned int modelLoc = mShader->GetUniformLocation("Model");
  unsigned int viewLoc = mShader->GetUniformLocation("View");
  unsigned int projection = mShader->GetUniformLocation("Projection");
  
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(p_model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(p_view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(p_projection));

  //Set up the uniforms from material that are not textures
  Shader::UniformMap const& shaderUniforms = mShader->GetUniformMap();
  Shader::UniformMap::const_iterator itUniformMat = shaderUniforms.begin();
  for (; itUniformMat != shaderUniforms.end(); ++itUniformMat)
  {
    std::string uniformName = itUniformMat->first;
    Shader::UniformInfo info = itUniformMat->second;
    std::string attributeKey;
    bool exists = mMaterial->GetUniformData(itUniformMat->first, attributeKey);
    if (exists == true)
    {
      //Handle various uniform types
      GLenum type = info.first;
      if (type == GL_FLOAT)
      {
        
        if (mMaterial->Has<float>(attributeKey))
        {
          float unif = mMaterial->Get<float>(attributeKey);
          unsigned int loc = mShader->GetUniformLocation(uniformName);
          glUniform1f(loc, unif);
        }

      }
      else if (type == GL_FLOAT_VEC3)
      {
        
        if (mMaterial->Has<glm::vec3>(attributeKey))
        {
          glm::vec3 univec3f = mMaterial->Get<glm::vec3>(attributeKey);
          unsigned int loc = mShader->GetUniformLocation(uniformName);
          glUniform3fv(loc, 1,  glm::value_ptr(univec3f));
        }
      }
    }
  }

  //Bind our textures
  unsigned int idTex = 0;
  for (auto texInfo : mMaterialTextureHandle->TextureIds() )
  {    
    unsigned int location = mShader->GetUniformLocation(texInfo.first);
    glActiveTexture(GL_TEXTURE0 + idTex);
    glBindTexture(GL_TEXTURE_2D, texInfo.second);
    glUniform1i(location, /*GL_TEXTURE*/ idTex);
    ++idTex;
  }
  
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);
}

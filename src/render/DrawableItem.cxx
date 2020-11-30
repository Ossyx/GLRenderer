#include "DrawableItem.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

std::unordered_map<std::string, DrawableItem::IntIntMap> DrawableItem::m_savedIdsAndLocations = {};

DrawableItem::DrawableItem()
{
  m_transform = glm::mat4(1.0f);
}

DrawableItem::DrawableItem(rx::MeshPtr pMesh, rx::MaterialPtr pMaterial,
  ShaderPtr pShader):
  mMesh(pMesh),
  mMaterial(pMaterial),
  mShader(pShader)
{
  m_transform = glm::mat4(1.0f);
}

DrawableItem::~DrawableItem()
{
}

void DrawableItem::SetTransform(glm::mat4 const& p_transform)
{
  m_transform = p_transform;
}

void DrawableItem::PrepareBufferFromMesh()
{
  unsigned int vpos_location = mShader->GetAttributeLocation("vPos");
  unsigned int normal_location = mShader->GetAttributeLocation("normal");
  unsigned int uv_location = mShader->GetAttributeLocation("uvcoords");

  rxLogInfo("PrepareBuffer with ");
  rxLogInfo("Triangle Count : "<< mMesh->GetTriangleCount());
  rxLogInfo("Vertex Count : "<< mMesh->GetVertexCount());
  
  mVertex.Build(GL_ARRAY_BUFFER, mMesh->GetVertexCount()*3*sizeof(float),
    mMesh->GetVertices());

  mNormal.Build(GL_ARRAY_BUFFER, mMesh->GetNormalCount()*3*sizeof(float),
    mMesh->GetNormals());

  if (mMesh->HasTangents())
  {
    mTangent.Build(GL_ARRAY_BUFFER, mMesh->GetTangentCount()*3*sizeof(float),
      mMesh->GetTangents());
  }

  if (mMesh->HasBitangents())
  {
    mBitangent.Build(GL_ARRAY_BUFFER, mMesh->GetBitangentCount()*3*sizeof(float),
      mMesh->GetBitangents());
  }

  mUV.Build(GL_ARRAY_BUFFER, mMesh->GetUVCoordsCount()*2*sizeof(float),
    mMesh->GetUVCoords());
  
  mIndex.Build(GL_ELEMENT_ARRAY_BUFFER, mMesh->GetTriangleCount()*3*sizeof(unsigned int),
    mMesh->GetTrianglesIndex());
  
  mVertexArray.Build();
  mVertexArray.Bind();  
  mVertexArray.BindBufferToLocation(mVertex, vpos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  mVertexArray.BindBufferToLocation(mNormal, normal_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  if (mMesh->HasTangents())
  {
    unsigned int tangent_location = mShader->GetAttributeLocation("tangent");
    mVertexArray.BindBufferToLocation(mTangent, tangent_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  if (mMesh->HasBitangents())
  {
    unsigned int bitangent_location = mShader->GetAttributeLocation("bitangent");
    mVertexArray.BindBufferToLocation(mBitangent, bitangent_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  mVertexArray.BindBufferToLocation(mUV, uv_location, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  m_elementCount = mMesh->GetTriangleCount()*3;
}

void DrawableItem::PrepareTextureFromMaterial()
{
    //Iterate on the shader Uniforms
  Shader::UniformMap const& shaderUniforms = mShader->GetUniformMap();

  Shader::UniformMap::const_iterator it = shaderUniforms.begin();

  bool loadtex = true;
  if(DrawableItem::m_savedIdsAndLocations.find(mMaterial->GetName()) != DrawableItem::m_savedIdsAndLocations.end())
  {
    loadtex = false;
    m_textureIdsLocation = DrawableItem::m_savedIdsAndLocations[mMaterial->GetName()];
  }

  for (; it != shaderUniforms.end(); ++it)
  {
    rxLogInfo("Linking uniform "<< it->first <<" to material.");
    std::string uniformName = it->first;
    Shader::UniformInfo info = it->second;
    std::string attributeKey;
    bool exists = mMaterial->GetUniformData(it->first, attributeKey);

    if (exists == true)
    {
      //Handle various uniform types
      GLenum type = info.first;
      if (type == GL_FLOAT)
      {
        rxLogInfo("Uniform "<< uniformName <<" is attribute "
          << attributeKey <<" of type GL_FLOAT");
      }
      else if (type == GL_FLOAT_VEC3)
      {
        rxLogInfo("Uniform "<< uniformName <<" is attribute "
          << attributeKey <<" of type GL_FLOAT_VEC3");
      }
      else if (type == GL_SAMPLER_2D && loadtex)
      {
        rxLogInfo("Uniform "<< uniformName <<" is attribute "
          <<attributeKey<<" of type GL_SAMPLER_2D");
        //Setup the textures

        unsigned int textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if (mMaterial->HasTextureData<unsigned char>(attributeKey))
        {
          auto const& tex = mMaterial->GetTextureData<unsigned char>(attributeKey);
          if (tex->m_channelCount == 1)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex->m_width,
              tex->m_height, 0, GL_RED, GL_UNSIGNED_BYTE, tex->m_data);
          }
          else if (tex->m_channelCount == 3)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->m_width,
              tex->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->m_data);
          }
          else if (tex->m_channelCount == 4)
          {
            rxLogWarning("Loading RGBA Texture " << attributeKey)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->m_width,
              tex->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->m_data);
          }
          else
          {
            rxLogInfo("UChar Texture channel count unsupported");
            assert(false);
          }
        }
        else if(mMaterial->HasTextureData<unsigned short>(attributeKey))
        {
          rx::Material::UShortTexture const& tex = mMaterial->GetTextureData<unsigned short>(attributeKey);
          if (tex->m_channelCount == 1)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, tex->m_width,
              tex->m_height, 0, GL_RED, GL_UNSIGNED_SHORT, tex->m_data);
          }
          else if (tex->m_channelCount == 3)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, tex->m_width,
              tex->m_height, 0, GL_RGB, GL_UNSIGNED_SHORT, tex->m_data);
          }
          else if (tex->m_channelCount == 4)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, tex->m_width,
              tex->m_height, 0, GL_RGBA, GL_UNSIGNED_SHORT, tex->m_data);
          }
          else
          {
            rxLogInfo("UShort Texture channel count unsupported");
            assert(false);
          }
        }
        else
        {
          rxLogError("No attribute " << attributeKey <<" found in material "
            << mMaterial->GetName());
        }

        glGenerateMipmap(GL_TEXTURE_2D);
//           rxLogInfo("tex->m_width " << tex->m_width);
//           rxLogInfo("tex->m_height " << tex->m_height);
//           rxLogInfo("tex->m_channelCount " << tex->m_channelCount);

          m_textureIdsLocation[textureId] = mShader->GetUniformLocation(uniformName);
        }
      }
    }

    if(loadtex == true)
    {
      DrawableItem::m_savedIdsAndLocations[mMaterial->GetName()] = m_textureIdsLocation;
    }
}

void DrawableItem::PrepareBuffer()
{
  PrepareBufferFromMesh();
  PrepareTextureFromMaterial();
}

void DrawableItem::SetupUniformAndTextures(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
  unsigned int modelLoc = mShader->GetUniformLocation("Model");
  unsigned int viewLoc = mShader->GetUniformLocation("View");
  unsigned int projection = mShader->GetUniformLocation("Projection");
  unsigned int lightLoc = mShader->GetUniformLocation("light");
  unsigned int camLoc = mShader->GetUniformLocation("cameraPos");
  
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(p_model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(p_view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(p_projection));

  glUniform3fv(lightLoc, 1,  glm::value_ptr(p_light));
  glUniform3fv(camLoc, 1,  glm::value_ptr(p_cameraPos));

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
        if( mMaterial->Has<float>(attributeKey) )
        {
          float unif = mMaterial->Get<float>(attributeKey);
          unsigned int loc = mShader->GetUniformLocation(uniformName);
          glUniform1f(loc, unif);
        }
      }
      else if (type == GL_FLOAT_VEC3)
      {
        if( mMaterial->Has<glm::vec3>(attributeKey) )
        {
          glm::vec3 univec3f = mMaterial->Get<glm::vec3>(attributeKey);
          unsigned int loc = mShader->GetUniformLocation(uniformName);
          glUniform3fv(loc, 1,  glm::value_ptr(univec3f));
        }
      }
    }
  }

  //Bind our textures
  IntIntMap::iterator it = m_textureIdsLocation.begin();
  unsigned int idTex = 0;
  for (; it != m_textureIdsLocation.end(); ++it, ++idTex)
  {
    glActiveTexture(GL_TEXTURE0 + idTex);
    glBindTexture(GL_TEXTURE_2D, it->first);
    glUniform1i(it->second, /*GL_TEXTURE*/ idTex);
  }
}

void DrawableItem::Draw(glm::mat4 const& p_view,
  glm::mat4 const& p_projection, glm::mat4 const& p_model,
  glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
  SetupUniformAndTextures(p_view, p_projection, p_model, p_light, p_cameraPos);
  DrawElements();
}

void DrawableItem::DrawElements()
{
  mVertexArray.Bind();
  mIndex.Bind();
  glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0);
}

void DrawableItem::DrawElementsInstanced(unsigned p_count)
{
  mVertexArray.Bind();
  mIndex.Bind();
  glDrawElementsInstanced(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0, p_count);
}

int DrawableItem::DrawSimple(Shader const& p_shader, glm::mat4 const& p_vpMat,
  glm::mat4 const& p_view, glm::mat4 const& p_projection, glm::mat4 const& p_model)
{
  unsigned int mvp_location = p_shader.GetUniformLocation("MVP");
  unsigned int modelLoc = p_shader.GetUniformLocation("Model");
  unsigned int viewLoc = p_shader.GetUniformLocation("View");
  unsigned int projection = p_shader.GetUniformLocation("Projection");

  glm::mat4 mvp = p_vpMat * m_transform;
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(p_model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(p_view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(p_projection));

  mVertexArray.Bind();
  mIndex.Bind();
  glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0);
}


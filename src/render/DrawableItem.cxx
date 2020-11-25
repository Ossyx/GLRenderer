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

DrawableItem::~DrawableItem()
{
}

void DrawableItem::SetTransform(glm::mat4 const& p_transform)
{
  m_transform = p_transform;
}

void DrawableItem::PrepareBufferFromMesh(rx::Mesh const& p_mesh, Shader const& p_shader)
{
  unsigned int vpos_location = p_shader.GetAttributeLocation("vPos");
  unsigned int normal_location = p_shader.GetAttributeLocation("normal");
  unsigned int uv_location = p_shader.GetAttributeLocation("uvcoords");

  rxLogInfo("PrepareBuffer with ");
  rxLogInfo("Triangle Count : "<< p_mesh.GetTriangleCount());
  rxLogInfo("Vertex Count : "<< p_mesh.GetVertexCount());
  
  mVertex.Build(GL_ARRAY_BUFFER, p_mesh.GetVertexCount()*3*sizeof(float),
    p_mesh.GetVertices());

  mNormal.Build(GL_ARRAY_BUFFER, p_mesh.GetNormalCount()*3*sizeof(float),
    p_mesh.GetNormals());

  if (p_mesh.HasTangents())
  {
    mTangent.Build(GL_ARRAY_BUFFER, p_mesh.GetTangentCount()*3*sizeof(float),
      p_mesh.GetTangents());
  }

  if (p_mesh.HasBitangents())
  {
    mBitangent.Build(GL_ARRAY_BUFFER, p_mesh.GetBitangentCount()*3*sizeof(float),
      p_mesh.GetBitangents());
  }

  mUV.Build(GL_ARRAY_BUFFER, p_mesh.GetUVCoordsCount()*2*sizeof(float),
    p_mesh.GetUVCoords());
  
  mIndex.Build(GL_ELEMENT_ARRAY_BUFFER, p_mesh.GetTriangleCount()*3*sizeof(unsigned int),
    p_mesh.GetTrianglesIndex());
  
  mVertexArray.Build();
  mVertexArray.Bind();  
  mVertexArray.BindBufferToLocation(mVertex, vpos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  mVertexArray.BindBufferToLocation(mNormal, normal_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  if (p_mesh.HasTangents())
  {
    unsigned int tangent_location = p_shader.GetAttributeLocation("tangent");
    mVertexArray.BindBufferToLocation(mTangent, tangent_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  if (p_mesh.HasBitangents())
  {
    unsigned int bitangent_location = p_shader.GetAttributeLocation("bitangent");
    mVertexArray.BindBufferToLocation(mBitangent, bitangent_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  mVertexArray.BindBufferToLocation(mUV, uv_location, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  m_elementCount = p_mesh.GetTriangleCount()*3;
}

void DrawableItem::PrepareTextureFromMaterial(const rx::Material& p_material, const Shader& p_shader)
{
    //Iterate on the shader Uniforms
  Shader::UniformMap const& shaderUniforms = p_shader.GetUniformMap();

  Shader::UniformMap::const_iterator it = shaderUniforms.begin();

  bool loadtex = true;
  if(DrawableItem::m_savedIdsAndLocations.find(p_material.GetName()) != DrawableItem::m_savedIdsAndLocations.end())
  {
    loadtex = false;
    m_textureIdsLocation = DrawableItem::m_savedIdsAndLocations[p_material.GetName()];
  }

  for (; it != shaderUniforms.end(); ++it)
  {
    rxLogInfo("Linking uniform "<< it->first <<" to material.");
    std::string uniformName = it->first;
    std::string attributeKey;
    bool exists = p_material.GetUniformData(it->first, attributeKey);

    if (exists == true)
    {
      //Handle various uniform types
      GLenum type = it->second;
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

        if (p_material.HasUCharTexData(attributeKey))
        {
          rx::Material::ByteTexture const& tex = p_material.GetByteTexture(attributeKey);
          if (tex.m_channelCount == 1)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex.m_width,
              tex.m_height, 0, GL_RED, GL_UNSIGNED_BYTE, tex.m_data);
          }
          else if (tex.m_channelCount == 3)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.m_width,
              tex.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.m_data);
          }
          else if (tex.m_channelCount == 4)
          {
            rxLogWarning("Loading RGBA Texture " << attributeKey)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.m_width,
              tex.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.m_data);
          }
          else
          {
            rxLogInfo("UChar Texture channel count unsupported");
            assert(false);
          }
        }
        else if(p_material.HasUShortTexData(attributeKey))
        {
          rx::Material::UShortTexture const& tex = p_material.GetUShortTexture(attributeKey);
          if (tex.m_channelCount == 1)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, tex.m_width,
              tex.m_height, 0, GL_RED, GL_UNSIGNED_SHORT, tex.m_data);
          }
          else if (tex.m_channelCount == 3)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, tex.m_width,
              tex.m_height, 0, GL_RGB, GL_UNSIGNED_SHORT, tex.m_data);
          }
          else if (tex.m_channelCount == 4)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, tex.m_width,
              tex.m_height, 0, GL_RGBA, GL_UNSIGNED_SHORT, tex.m_data);
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
            << p_material.GetName());
        }

        glGenerateMipmap(GL_TEXTURE_2D);
//           rxLogInfo("tex.m_width " << tex.m_width);
//           rxLogInfo("tex.m_height " << tex.m_height);
//           rxLogInfo("tex.m_channelCount " << tex.m_channelCount);

          m_textureIdsLocation[textureId] = p_shader.GetUniformLocation(uniformName);
        }
      }
    }

    if(loadtex == true)
    {
      DrawableItem::m_savedIdsAndLocations[p_material.GetName()] = m_textureIdsLocation;
    }
}

int DrawableItem::PrepareBuffer(rx::Mesh const& p_mesh, rx::Material const& p_material,
  Shader const& p_shader)
{
  PrepareBufferFromMesh(p_mesh, p_shader);
  PrepareTextureFromMaterial(p_material, p_shader);
}

void DrawableItem::SetupUniformAndTextures(Shader const& p_shader,
    rx::Material& p_material, glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
  unsigned int modelLoc = p_shader.GetUniformLocation("Model");
  unsigned int viewLoc = p_shader.GetUniformLocation("View");
  unsigned int projection = p_shader.GetUniformLocation("Projection");
  unsigned int lightLoc = p_shader.GetUniformLocation("light");
  unsigned int camLoc = p_shader.GetUniformLocation("cameraPos");
  
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(p_model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(p_view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(p_projection));

  glUniform3fv(lightLoc, 1,  glm::value_ptr(p_light));
  glUniform3fv(camLoc, 1,  glm::value_ptr(p_cameraPos));

  //Set up the uniforms from material that are not textures
  Shader::UniformMap const& shaderUniforms = p_shader.GetUniformMap();
  Shader::UniformMap::const_iterator itUniformMat = shaderUniforms.begin();
  for (; itUniformMat != shaderUniforms.end(); ++itUniformMat)
  {
    std::string uniformName = itUniformMat->first;
    std::string attributeKey;
    bool exists = p_material.GetUniformData(itUniformMat->first, attributeKey);
    if (exists == true)
    {
      //Handle various uniform types
      GLenum type = itUniformMat->second;
      if (type == GL_FLOAT)
      {
        float unif;
        if (p_material.GetData(attributeKey, unif))
        {
          unsigned int loc = p_shader.GetUniformLocation(uniformName);
          glUniform1f(loc, unif);
        }

      }
      else if (type == GL_FLOAT_VEC3)
      {
        glm::vec3 univec3f;
        if (p_material.GetData(attributeKey, univec3f))
        {
          unsigned int loc = p_shader.GetUniformLocation(uniformName);
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

int DrawableItem::Draw(Shader const& p_shader,
  rx::Material& p_material, glm::mat4 const& p_view,
  glm::mat4 const& p_projection, glm::mat4 const& p_model,
  glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
  SetupUniformAndTextures(p_shader, p_material, p_view,
    p_projection, p_model, p_light, p_cameraPos);
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


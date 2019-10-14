#include "DrawableItem.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

DrawableItem::DrawableItem()
{
  //simple triangle
//   m_vertex_buffer_data = new float[9];
//   m_index_buffer = new int[3];
//
//   m_vertex_buffer_data[0] = 0.0f;
//   m_vertex_buffer_data[1] = 0.5f;
//   m_vertex_buffer_data[2] = 0.0f;
//   m_vertex_buffer_data[3] = 0.5f;
//   m_vertex_buffer_data[4] = -0.5f;
//   m_vertex_buffer_data[5] = 0.0f;
//   m_vertex_buffer_data[6] = -0.5f;
//   m_vertex_buffer_data[7] = -0.5f;
//   m_vertex_buffer_data[8] = 0.0f;
//
//
//   m_index_buffer[0] = 0;
//   m_index_buffer[1] = 1;
//   m_index_buffer[2] = 2;
//
  m_transform = glm::mat4(1.0f);
}

DrawableItem::~DrawableItem()
{
}

void DrawableItem::SetTransform(glm::mat4 const& p_transform)
{
  m_transform = p_transform;
}

int DrawableItem::PrepareBuffer(rx::Mesh const& p_mesh, rx::Material const& p_material,
  Shader const& p_shader)
{
  unsigned int vpos_location = p_shader.GetAttributeLocation("vPos");
  unsigned int normal_location = p_shader.GetAttributeLocation("normal");
  unsigned int uv_location = p_shader.GetAttributeLocation("uvcoords");

  rxLogInfo("PrepareBuffer with ");
  rxLogInfo("Triangle Count : "<< p_mesh.GetTriangleCount());
  rxLogInfo("Vertex Count : "<< p_mesh.GetVertexCount());
  glGenBuffers(1, &m_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, p_mesh.GetVertexCount()*3*sizeof(float),
    p_mesh.GetVertices(), GL_STATIC_DRAW);

  glGenBuffers(1, &m_normalBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
  glBufferData(GL_ARRAY_BUFFER, p_mesh.GetNormalCount()*3*sizeof(float),
    p_mesh.GetNormals(), GL_STATIC_DRAW);

  if (p_mesh.HasTangents())
  {
    glGenBuffers(1, &m_tangentBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferId);
    glBufferData(GL_ARRAY_BUFFER, p_mesh.GetTangentCount()*3*sizeof(float),
    p_mesh.GetTangents(), GL_STATIC_DRAW);
  }

  if (p_mesh.HasBitangents())
  {
    glGenBuffers(1, &m_bitangentBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferId);
    glBufferData(GL_ARRAY_BUFFER, p_mesh.GetBitangentCount()*3*sizeof(float),
    p_mesh.GetBitangents(), GL_STATIC_DRAW);
  }

  glGenBuffers(1, &m_uvBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferId);
  glBufferData(GL_ARRAY_BUFFER, p_mesh.GetUVCoordsCount()*2*sizeof(float),
    p_mesh.GetUVCoords(), GL_STATIC_DRAW);

  glGenBuffers(1, &m_indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_mesh.GetTriangleCount()*3*sizeof(unsigned int),
    p_mesh.GetTrianglesIndex(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &m_vertexArrayId);
  glBindVertexArray(m_vertexArrayId);

  glEnableVertexAttribArray(vpos_location);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(normal_location);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
  glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  if (p_mesh.HasTangents())
  {
    unsigned int tangent_location = p_shader.GetAttributeLocation("tangent");
    glEnableVertexAttribArray(tangent_location);
    glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferId);
    glVertexAttribPointer(tangent_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  if (p_mesh.HasBitangents())
  {
    unsigned int bitangent_location = p_shader.GetAttributeLocation("bitangent");
    glEnableVertexAttribArray(bitangent_location);
    glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferId);
    glVertexAttribPointer(bitangent_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  glEnableVertexAttribArray(uv_location);
  glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferId);
  glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  m_elementCount = p_mesh.GetTriangleCount()*3;

  //Iterate on the shader Uniforms
  Shader::UniformMap const& shaderUniforms = p_shader.GetUniformMap();

  Shader::UniformMap::const_iterator it = shaderUniforms.begin();

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
      else if (type == GL_SAMPLER_2D)
      {
        rxLogInfo("Uniform "<< uniformName <<" is attribute "
          <<attributeKey<<" of type GL_SAMPLER_2D");
        //Setup the textures
        if (p_material.HasUCharTexData(attributeKey))
        {
          glGenTextures(1, &m_textureId);
          glBindTexture(GL_TEXTURE_2D, m_textureId);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

          rxLogInfo("Texture id : "<<m_textureId);

          rx::Material::ByteTexture const& tex = p_material.GetByteTexture(attributeKey);
          if (tex.m_channelCount == 3)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.m_width,
              tex.m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.m_data);
          }
          else if (tex.m_channelCount == 1)
          {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex.m_width,
              tex.m_height, 0, GL_RED, GL_UNSIGNED_BYTE, tex.m_data);
          }
          else
          {
            rxLogInfo("Texture channel count unsupported");
            assert(false);
          }
          glGenerateMipmap(GL_TEXTURE_2D);
          rxLogInfo("tex.m_width " << tex.m_width);
          rxLogInfo("tex.m_height " << tex.m_height);
          rxLogInfo("tex.m_channelCount " << tex.m_channelCount);

          m_textureIdsLocation[m_textureId] = p_shader.GetUniformLocation(uniformName);
        }
      }
    }
  }
}

int DrawableItem::Draw(Shader const& p_shader, glm::mat4 const& p_vpMat,
  rx::Material const& p_material, glm::mat4 const& p_view,
  glm::mat4 const& p_projection, glm::mat4 const& p_model,
  glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
  unsigned int mvp_location = p_shader.GetUniformLocation("MVP");
  unsigned int modelLoc = p_shader.GetUniformLocation("Model");
  unsigned int viewLoc = p_shader.GetUniformLocation("View");
  unsigned int projection = p_shader.GetUniformLocation("Projection");
  unsigned int lightLoc = p_shader.GetUniformLocation("light");
  unsigned int camLoc = p_shader.GetUniformLocation("cameraPos");

  glm::mat4 mvp = p_vpMat * m_transform;
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

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


  glBindVertexArray(m_vertexArrayId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
  glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0);
}

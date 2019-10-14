#ifndef DRAWABLEITEM_HXX
#define DRAWABLEITEM_HXX

#include "Mesh.hxx"
#include "Material.hxx"
#include "Shader.hxx"

#include <unordered_map>
#include <glm/mat4x4.hpp> // glm::mat4

class DrawableItem
{
public:

  DrawableItem();

  ~DrawableItem();

  int PrepareBuffer(Mesh const& p_mesh, Material const& p_material, Shader const& p_shader);

  void SetTransform(glm::mat4 const& p_transform);

  int Draw(Shader const& p_shader, glm::mat4 const& p_vpMat, Material const& p_material,
    glm::mat4 const& p_view, glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos);

private:

  //Transformation matrix
  glm::mat4 m_transform;

  unsigned int m_vertexBufferId;

  unsigned int m_indexBufferId;

  unsigned int m_vertexArrayId;

  unsigned int m_normalBufferId;

  unsigned int m_tangentBufferId;

  unsigned int m_bitangentBufferId;

  unsigned int m_uvBufferId;

  unsigned int m_elementCount;

  typedef std::unordered_map<unsigned int, unsigned int> IntIntMap;
  IntIntMap m_textureIdsLocation;

  unsigned int m_textureId;
};

#endif

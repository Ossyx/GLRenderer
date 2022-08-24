#ifndef DRAWABLEITEM_HXX
#define DRAWABLEITEM_HXX

#include "Mesh.hxx"
#include "Material.hxx"
#include "Shader.hxx"
#include "GLAbstraction.hxx"

#include <unordered_map>
#include <glm/mat4x4.hpp> // glm::mat4
#include "GLRendererExport.hxx"


class GLRenderer_EXPORT DrawableItem
{
public:

  DrawableItem();

  DrawableItem(rx::MeshPtr pMesh, rx::MaterialPtr pMaterial, ShaderPtr pShader);

  virtual ~DrawableItem();

  void PrepareBuffer();

  void SetTransform(glm::mat4 const& p_transform);

  virtual void Draw(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos);

  int DrawSimple(Shader const& p_shader, glm::mat4 const& p_vpMat,
    glm::mat4 const& p_view, glm::mat4 const& p_projection,
     glm::mat4 const& p_model);

protected:
  
  virtual void PrepareBufferFromMesh();
  
  virtual void PrepareTextureFromMaterial();

  void SetupUniformAndTextures(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos);

  void DrawElements();

  void DrawElementsInstanced(unsigned p_count);

  //Transformation matrix
  glm::mat4 m_transform;

  ArrayBuffer mVertex;

  ArrayBuffer mIndex;

  VertexArray mVertexArray;

  ArrayBuffer mNormal;

  ArrayBuffer mTangent;

  ArrayBuffer mBitangent;

  ArrayBuffer mUV;

  unsigned int m_elementCount;

  typedef std::unordered_map<unsigned int, unsigned int> IntIntMap;
  IntIntMap m_textureIdsLocation;

  static std::unordered_map<std::string, IntIntMap> m_savedIdsAndLocations;
  
  rx::MeshPtr mMesh;
  rx::MaterialPtr mMaterial;  
  ShaderPtr mShader;
};

using DrawableItemPtr = std::shared_ptr<DrawableItem>;

#endif

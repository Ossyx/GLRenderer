#ifndef TERRAINLOD_HXX
#define TERRAINLOD_HXX

#include <vector>
#include <iostream>
#include "FrustumCulling.hxx"
#include "DrawableItem.hxx"

struct QuadTreeNode
{
  QuadTreeNode(unsigned p_level, glm::vec3 const& p_position);
  ~QuadTreeNode();

  void Split();

  //NW NE SW SE
  QuadTreeNode* childs[4];
  unsigned level;
  AABB bbox;
  glm::vec3 center;
  bool leaf;
  float outerTessLevel[4];
  float inerLevel;
};

class TerrainLOD : public DrawableItem
{
public:
  TerrainLOD();
  virtual ~TerrainLOD();

  void PrepareBufferQuad();

  virtual int Draw(Shader const& p_shader, glm::mat4 const& p_vpMat,
    rx::Material& p_material, glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos);

  int DrawTerrain(glm::mat4 const& p_vpMat, glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos);

  void RecomputeTree(glm::vec3 const& p_cameraPosition);
private:
  void FindLeafs(std::vector<QuadTreeNode*>& p_leafs);
  void DestroySubtree(QuadTreeNode* p_node);
  void BuildPrimitiveBuffers(std::vector<QuadTreeNode*> const& p_leafs);
  void BuildSimple();

  QuadTreeNode* m_root;
  typedef std::unordered_map<unsigned int, float> IntFloatMap;
  IntFloatMap m_levelDistances;
  float m_scale;
  unsigned m_leafCount;

  rx::Material* terrainMaterial;
  Shader m_terrainShader;

  unsigned m_quadDataSSBO;
};

#endif

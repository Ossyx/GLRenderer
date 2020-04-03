#ifndef TERRAINLOD_HXX
#define TERRAINLOD_HXX

#include <vector>
#include <iostream>
#include <algorithm>
#include "FrustumCulling.hxx"
#include "DrawableItem.hxx"

enum DirectionType
{
  eNorth,
  eSouth,
  eWest,
  eEast
};

struct QuadTreeNode
{
  QuadTreeNode(unsigned p_level, glm::vec3 const& p_position);
  ~QuadTreeNode();

  void Split();
  QuadTreeNode* FindNeighbour(DirectionType p_direction);
  unsigned int GetChildIdInParent();

  void SetGeometry(glm::vec3 const& p_1, glm::vec3 const& p_2,
    glm::vec3 const& p_3, glm::vec3 const& p_4);

  QuadTreeNode* parent;
  //NW NE SW SE
  QuadTreeNode* childs[4];
  glm::vec3 geometry[4];
  unsigned level;
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

  void SetSize(float p_size);

private:
  void FindLeafs(std::vector<QuadTreeNode*>& p_leafs);
  void DestroySubtree(QuadTreeNode* p_node);
  void SetOuterTessellationLvl(std::vector<QuadTreeNode*>& p_leafs);
  void BuildPrimitiveBuffers(std::vector<QuadTreeNode*> const& p_leafs);
  void BuildSimple();

  QuadTreeNode* m_cubeRoots[8];
  typedef std::unordered_map<unsigned int, float> IntFloatMap;
  IntFloatMap m_levelDistances;
  IntFloatMap m_tessellationLevel;
  float m_scale;
  unsigned m_leafCount;
  float m_size;

  rx::Material* terrainMaterial;
  Shader m_terrainShader;

  unsigned m_quadDataSSBO;
  unsigned m_quadColorSSBO;
  unsigned m_quadOuterTessSSBO;
};

#endif

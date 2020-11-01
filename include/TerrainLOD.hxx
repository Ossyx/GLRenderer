#ifndef TERRAINLOD_HXX
#define TERRAINLOD_HXX

#include <vector>
#include <iostream>
#include <algorithm>
#include "FrustumCulling.hxx"
#include "DrawableItem.hxx"
#include "OceanSurface.h"
#include "Camera.hxx"

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

  void PrepareBufferQuad(Camera const& p_cam);

  virtual int Draw(Shader const& p_shader, rx::Material& p_material,
    glm::mat4 const& p_view, glm::mat4 const& p_projection,
    glm::mat4 const& p_model, glm::vec3 const& p_light,
    glm::vec3 const& p_cameraPos);

  int DrawTerrain(Camera const& p_cam, glm::mat4 const& p_model,
    glm::vec3 const& p_light);

  int DrawWater(Camera const& p_cam, glm::mat4 const& p_model,
    glm::vec3 const& p_light, rx::OceanSurface const& p_surf, unsigned int p_waterSurfTex);

  void RecomputeTree(glm::vec3 const& p_cameraPosition);

  void SetShader(Shader const& p_shader);

  void SetSize(float p_size);

  float GetSize() const;

  float GetNear() const;

  float GetFar() const;

  float ComputeDistanceToSurface(glm::vec3 const& p_position);

  glm::vec3 sphereProjectedPosition(glm::vec3 const& p_position);

  void SetOrigin(glm::vec3 const& p_origin);

  void SetScale(float p_scale);

private:
  void FindLeafs(std::vector<QuadTreeNode*>& p_leafs);
  void DestroySubtree(QuadTreeNode* p_node);
  void SetOuterTessellationLvl(std::vector<QuadTreeNode*>& p_leafs);
  void BuildPrimitiveBuffers(std::vector<QuadTreeNode*> const& p_leafs);
  void CullLeaves(std::vector<QuadTreeNode*>& p_leafs, glm::vec3 const& p_cameraPosition);
  void BuildSimple();
  void ComputeNearAndFar(glm::vec3 const& p_position, glm::vec3 const& p_direction, std::vector<QuadTreeNode*>& p_leafs);

  QuadTreeNode* FindClosestTree(glm::vec3 const& p_pos);
  QuadTreeNode* FindClosestLeaf(glm::vec3 const& p_pos, QuadTreeNode* p_node);

  QuadTreeNode* m_cubeRoots[6];
  typedef std::unordered_map<unsigned int, float> IntFloatMap;
  IntFloatMap m_levelDistances;
  IntFloatMap m_tessellationLevel;
  unsigned m_leafCount;
  float m_size;

  rx::Material* terrainMaterial;
  Shader m_renderShader;

  unsigned m_quadDataSSBO;
  unsigned m_quadColorSSBO;
  unsigned m_quadOuterTessSSBO;

  float m_near;
  float m_far;

  glm::vec3 m_origin;
  float m_scale;
};

#endif

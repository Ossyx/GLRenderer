#ifndef FRUSTUMCULLING_HXX
#define FRUSTUMCULLING_HXX

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4

#include "GLRendererExport.hxx"

struct GLRenderer_EXPORT Frustum
{
  glm::vec4 planes[6];
  void InitFromMatrix(glm::mat4 const& p_vpMatrix);
};

struct GLRenderer_EXPORT AABB
{
  glm::vec3 min;
  glm::vec3 max;
};

bool BoxInFrustum(Frustum const& p_fru, AABB const& p_bbox);

#endif

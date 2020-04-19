#ifndef HEIGHTMAPGENERATOR_HXX
#define HEIGHTMAPGENERATOR_HXX

#include <glm/vec3.hpp> // glm::glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/geometric.hpp>
#include <math.h>

class HeightmapGenerator
{
public:
  HeightmapGenerator();
  ~HeightmapGenerator();
  void Generate();

private:

  glm::vec3 fractV3(glm::vec3 const& p_v);
  glm::vec3 floorV3(glm::vec3 const& p_v);
  glm::vec3 yzx(glm::vec3 const& p_v);
  glm::vec3 yxz(glm::vec3 const& p_v);
  glm::vec3 zxy(glm::vec3 const& p_v);
  float hash31(glm::vec3 p3);
  glm::vec3 hash33(glm::vec3 p3);
  float simplex_noise(glm::vec3 p);
  float noise(glm::vec3 p);
  float noise_sum_H(glm::vec3 p);
  float noise_sum_M(glm::vec3 p);
  float noise_sum_L(glm::vec3 p);

  unsigned int m_width;
  unsigned int m_height;
  float* m_heightmap;
};

#endif

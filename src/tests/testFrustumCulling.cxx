#include "FrustumCulling.hxx"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

AABB UnitBoxAtPosition(glm::vec3 const& p_pos)
{
  float sqrt22 = sqrt(2.0f) / 2.0f;
  glm::vec3 halfDiag = glm::vec3(sqrt22, sqrt22, sqrt22);
  AABB box;
  box.min = p_pos - halfDiag;
  box.max = p_pos + halfDiag;
  return box;
};

int main()
{
  //Default frustum look to -z
  glm::mat4 projection = glm::perspective(glm::radians(45.0f),
  1920.0f / 1080.0f, 0.5f, 1000.f);

  Frustum f;
  f.InitFromMatrix(projection);

  //   AABB box;
  //   box.min = glm::vec3(0.0f, 0.0f, -10.0f);
  //   box.max = glm::vec3(10.0f, 10.0f, -5.0f);

  //Generate a ring of countBbox bbox at distance 500.0
  std::vector<AABB> bboxList;
  unsigned countBbox = 1000;
  for(unsigned i = 0; i < countBbox; ++i)
  {
    glm::vec3 pos;
    pos.x = cos(((float)i / (float)countBbox) * 3.14159f * 2.0f) * 500.0f;
    pos.z = sin(((float)i / (float)countBbox) * 3.14159f * 2.0f) * 500.0f;
    pos.y = 0.0f;
    bboxList.push_back(UnitBoxAtPosition(pos));
  }

  unsigned countInside = 0;
  for(unsigned i=0; i < bboxList.size(); ++i)
  {
    if(BoxInFrustum(f, bboxList[i]))
    {
      std::cout<<"BBox "<< i << " inside "<<std::endl;
      countInside++;
    }
  }

  std::cout<<"We have "<< countInside <<" bbox inside frustum." << std::endl;
}

#include "FrustumCulling.hxx"

void Frustum::InitFromMatrix(glm::mat4 const& p_vpMatrix)
{
  //From Fast Extraction of Viewing Frustum Planes from the World-
  //View-Projection Matrix

  // Left clipping plane
  planes[0].x = p_vpMatrix[3].x + p_vpMatrix[0].x;
  planes[0].y = p_vpMatrix[3].y + p_vpMatrix[0].y;
  planes[0].z = p_vpMatrix[3].z + p_vpMatrix[0].z;
  planes[0].w = p_vpMatrix[3].w + p_vpMatrix[0].w;
  //Right clipping plane
  planes[1].x = p_vpMatrix[3].x - p_vpMatrix[0].x;
  planes[1].y = p_vpMatrix[3].y - p_vpMatrix[0].y;
  planes[1].z = p_vpMatrix[3].z - p_vpMatrix[0].z;
  planes[1].w = p_vpMatrix[3].w - p_vpMatrix[0].w;
  //Top clipping plane
  planes[2].x = p_vpMatrix[3].x - p_vpMatrix[1].x;
  planes[2].y = p_vpMatrix[3].y - p_vpMatrix[1].y;
  planes[2].z = p_vpMatrix[3].z - p_vpMatrix[1].z;
  planes[2].w = p_vpMatrix[3].w - p_vpMatrix[1].w;
  //Bottom clipping plane
  planes[3].x = p_vpMatrix[3].x + p_vpMatrix[1].x;
  planes[3].y = p_vpMatrix[3].y + p_vpMatrix[1].y;
  planes[3].z = p_vpMatrix[3].z + p_vpMatrix[1].z;
  planes[3].w = p_vpMatrix[3].w + p_vpMatrix[1].w;
  //Near clipping plane
  planes[4].x = p_vpMatrix[3].x + p_vpMatrix[2].x;
  planes[4].y = p_vpMatrix[3].y + p_vpMatrix[2].y;
  planes[4].z = p_vpMatrix[3].z + p_vpMatrix[2].z;
  planes[4].w = p_vpMatrix[3].w + p_vpMatrix[2].w;
  //Far clipping plane
  planes[5].x = p_vpMatrix[3].x - p_vpMatrix[2].x;
  planes[5].y = p_vpMatrix[3].y - p_vpMatrix[2].y;
  planes[5].z = p_vpMatrix[3].z - p_vpMatrix[2].z;
  planes[5].w = p_vpMatrix[3].w - p_vpMatrix[2].w;
}

bool BoxInFrustum(const Frustum& p_fru, const AABB& p_box)
{
  //From inigo quilez "Fixing frustum culling"
  //Check box outside/inside of frustum
  for(unsigned i=0; i<6; i++ )
  {
    unsigned out = 0;
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.min.x, p_box.min.y, p_box.min.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.max.x, p_box.min.y, p_box.min.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.min.x, p_box.max.y, p_box.min.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.max.x, p_box.max.y, p_box.min.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.min.x, p_box.min.y, p_box.max.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.max.x, p_box.min.y, p_box.max.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.min.x, p_box.max.y, p_box.max.z, 1.0f) ) < 0.0 )?1:0);
    out += ((glm::dot( p_fru.planes[i], glm::vec4(p_box.max.x, p_box.max.y, p_box.max.z, 1.0f) ) < 0.0 )?1:0);

    if(out == 8)
    {
      return false;
    }

  }

  return true;
  // check frustum outside/inside box
  //TODO Compute corners
}

#include "TerrainLOD.hxx"
#include <glm/gtc/type_ptr.hpp>

#define sqrt2rx 1.41421356237f

TerrainLOD::TerrainLOD()
{
  //build eight vertex of unit cube;
  glm::vec3 p0(-0.5, 0.5, -0.5);
  glm::vec3 p1(-0.5, 0.5, 0.5);
  glm::vec3 p2(0.5, 0.5, 0.5);
  glm::vec3 p3(0.5, 0.5, -0.5);
  glm::vec3 p4(-0.5, -0.5, -0.5);
  glm::vec3 p5(0.5, -0.5, -0.5);
  glm::vec3 p6(0.5, -0.5, 0.5);
  glm::vec3 p7(-0.5, -0.5, 0.5);

  //Build the cube
  m_cubeRoots[0] = new QuadTreeNode(1, glm::vec3(0.0f, 0.5f, 0.0f));
  m_cubeRoots[0]->parent = nullptr;
  m_cubeRoots[0]->SetGeometry(p0, p1, p2, p3);

  m_cubeRoots[1] = new QuadTreeNode(1, glm::vec3(0.0f, 0.0f, 0.5f));
  m_cubeRoots[1]->parent = nullptr;
  m_cubeRoots[1]->SetGeometry(p1, p7, p6, p2);

  m_cubeRoots[2] = new QuadTreeNode(1, glm::vec3(0.5f, 0.0f, 0.0f));
  m_cubeRoots[2]->parent = nullptr;
  m_cubeRoots[2]->SetGeometry(p2, p6, p5, p3);

  m_cubeRoots[3] = new QuadTreeNode(1, glm::vec3(0.0f, 0.0f, -0.5f));
  m_cubeRoots[3]->parent = nullptr;
  m_cubeRoots[3]->SetGeometry(p3, p5, p4, p0);

  m_cubeRoots[4] = new QuadTreeNode(1, glm::vec3(-0.5f, 0.0f, 0.0f));
  m_cubeRoots[4]->parent = nullptr;
  m_cubeRoots[4]->SetGeometry(p0, p4, p7, p1);

  m_cubeRoots[5] = new QuadTreeNode(1, glm::vec3(0.0f, -0.5f, 0.0f));
  m_cubeRoots[5]->parent = nullptr;
  m_cubeRoots[5]->SetGeometry(p7, p4, p5, p6);

  m_levelDistances[1] = 16.0;
  m_levelDistances[2] = 8.0;
  m_levelDistances[4] = 4.0;
  m_levelDistances[8] = 2.0;
  m_levelDistances[16] = 1.5;
  m_levelDistances[32] = 0.75;
  m_levelDistances[64] = 0.325;
  m_levelDistances[128] = 0.15125;
  m_levelDistances[256] = 0.07;
  m_levelDistances[512] = 0.035;
  m_levelDistances[1024] = 0.015;

  m_tessellationLevel[1] = 8.0;
  m_tessellationLevel[2] = 8.0;
  m_tessellationLevel[4] = 8.0;
  m_tessellationLevel[8] = 8.0;
  m_tessellationLevel[16] = 16.0;
  m_tessellationLevel[32] = 16.0;
  m_tessellationLevel[64] = 16.0;
  m_tessellationLevel[128] = 16.0;
  m_tessellationLevel[256] = 32.0;
  m_tessellationLevel[512] = 32.0;
  m_tessellationLevel[1024] = 64.0;

  terrainMaterial = new rx::Material();
  terrainMaterial->SetName("CubeMaterial");
  terrainMaterial->SetData("Ka", glm::vec3(0.2,0.0,0.0));
  terrainMaterial->SetData("Ks", glm::vec3(1.0,0.0,0.0));
  terrainMaterial->SetData("Kd", glm::vec3(1.0,0.0,0.0));

  terrainMaterial->SetUniformData("ambient_color", "Ka");
  terrainMaterial->SetUniformData("specular_color", "Ks");
  terrainMaterial->SetUniformData("diffuse_color", "Kd");
}

TerrainLOD::~TerrainLOD()
{
}
void TerrainLOD::PrepareBufferQuad(Camera const& p_cam)
{
  std::vector<QuadTreeNode*> leafs;
  FindLeafs(leafs);
  m_leafCount = leafs.size();

  //Sort the leaf vector by level
  std::sort(leafs.begin(), leafs.end(),
    [](const QuadTreeNode* a, const QuadTreeNode* b)
  {
    return a->level > b->level;
  });

  SetOuterTessellationLvl(leafs);
  CullLeaves(leafs, p_cam.GetPosition());
  ComputeNearAndFar(p_cam.GetPosition(), p_cam.GetDirection(), leafs);
  BuildPrimitiveBuffers(leafs);
}

void TerrainLOD::BuildSimple()
{
  float posPri[12] = {-1.0, 0.0, -1.0,
    -1.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, -1.0};

  unsigned int vpos_location = m_renderShader.GetAttributeLocation("vPos");

  glGenBuffers(1, &m_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), posPri, GL_STATIC_DRAW);

  glGenVertexArrays(1, &m_vertexArrayId);
  glBindVertexArray(m_vertexArrayId);

  glEnableVertexAttribArray(vpos_location);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  float testDataQuad[4] = {0.0, 3.0, 0.0, 1.0};

  glGenBuffers(1, &m_quadDataSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_quadDataSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, 4*sizeof(float), testDataQuad, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_quadDataSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}

void TerrainLOD::FindLeafs(std::vector<QuadTreeNode*>& p_leafs)
{
  std::vector<QuadTreeNode*> nodeToExplore;
  for(unsigned int i=0; i < 6; ++i)
  {
    nodeToExplore.push_back(m_cubeRoots[i]);
  }

  while(nodeToExplore.size() > 0)
  {
    QuadTreeNode* last = nodeToExplore.back();
    nodeToExplore.pop_back();

    if(last->leaf == true)
    {
      p_leafs.push_back(last);
    }
    else
    {
      nodeToExplore.push_back(last->childs[0]);
      nodeToExplore.push_back(last->childs[1]);
      nodeToExplore.push_back(last->childs[2]);
      nodeToExplore.push_back(last->childs[3]);
    }
  }
}

void TerrainLOD::RecomputeTree(glm::vec3 const& p_cameraPosition)
{
  std::vector<QuadTreeNode*> nodeToExplore;
  for(unsigned int i=0; i < 6; ++i)
  {
    nodeToExplore.push_back(m_cubeRoots[i]);
  }

  while(nodeToExplore.size() > 0)
  {
    QuadTreeNode* last = nodeToExplore.back();
    nodeToExplore.pop_back();

    float dist = glm::distance(last->center * m_size, p_cameraPosition);
    dist = dist - (1.0f/last->level);
    if(last->leaf == false)
    {
      if(dist > m_levelDistances[last->level])
      {
        DestroySubtree(last);
      }
      else
      {
        nodeToExplore.push_back(last->childs[0]);
        nodeToExplore.push_back(last->childs[1]);
        nodeToExplore.push_back(last->childs[2]);
        nodeToExplore.push_back(last->childs[3]);
      }
    }
    else
    {
      if(dist < m_levelDistances[last->level] && last->level < 1024)
      {
        last->Split();
        nodeToExplore.push_back(last->childs[0]);
        nodeToExplore.push_back(last->childs[1]);
        nodeToExplore.push_back(last->childs[2]);
        nodeToExplore.push_back(last->childs[3]);
      }
    }
  }
}

QuadTreeNode* TerrainLOD::FindClosestTree(glm::vec3 const& p_pos)
{
  float minDist = std::numeric_limits<float>::max();
  QuadTreeNode* node = nullptr;
  for(unsigned int i=0; i < 6; ++i)
  {
    float dist = glm::distance(m_cubeRoots[i]->center * m_size, p_pos);
    if( dist < minDist )
    {
      minDist = dist;
      node = m_cubeRoots[i];
    }
  }
  return node;
}

QuadTreeNode* TerrainLOD::FindClosestLeaf(glm::vec3 const& p_pos, QuadTreeNode* p_node)
{
  QuadTreeNode* node = p_node;
  while(node->leaf == false)
  {
    unsigned id = 0;
    float minDist = std::numeric_limits<float>::max();
    for(unsigned int i=0; i < 4; ++i)
    {
      float d = glm::distance(node->childs[i]->center * m_size, p_pos);
      if( d < minDist)
      {
        id = i;
      }
    }
    node = node->childs[id];
  }
  return node;
}

void TerrainLOD::DestroySubtree(QuadTreeNode* p_node)
{
  std::vector<QuadTreeNode*> nodeToDestroy;
  p_node->leaf = true;
  nodeToDestroy.push_back(p_node->childs[0]);
  nodeToDestroy.push_back(p_node->childs[1]);
  nodeToDestroy.push_back(p_node->childs[2]);
  nodeToDestroy.push_back(p_node->childs[3]);

  while(nodeToDestroy.size() > 0)
  {
    QuadTreeNode* last = nodeToDestroy.back();
    nodeToDestroy.pop_back();

    if(last->leaf == true)
    {
      delete last;
    }
    else
    {
      nodeToDestroy.push_back(last->childs[0]);
      nodeToDestroy.push_back(last->childs[1]);
      nodeToDestroy.push_back(last->childs[2]);
      nodeToDestroy.push_back(last->childs[3]);
      delete last;
    }
  }
}

int TerrainLOD::Draw(Shader const& p_shader, glm::mat4 const& p_vpMat,
    rx::Material& p_material, glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
}

int TerrainLOD::DrawTerrain(glm::mat4 const& p_vpMat,
    glm::mat4 const& p_view, glm::mat4 const& p_projection,
    glm::mat4 const& p_model, glm::vec3 const& p_light,
    glm::vec3 const& p_cameraPos)
{
  glUseProgram(m_renderShader.GetProgram());
  unsigned block_index = 0;
  block_index = glGetProgramResourceIndex(m_renderShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutDataQuad");
  glShaderStorageBlockBinding(m_renderShader.GetProgram(), block_index, 3);

  block_index = glGetProgramResourceIndex(m_renderShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutColorQuad");
  glShaderStorageBlockBinding(m_renderShader.GetProgram(), block_index, 4);

  block_index = glGetProgramResourceIndex(m_renderShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutOuterTess");
  glShaderStorageBlockBinding(m_renderShader.GetProgram(), block_index, 5);

  unsigned int size_location = m_renderShader.GetUniformLocation("size");
  glUniform1f(size_location, m_size);

  unsigned int surfdist_location = m_renderShader.GetUniformLocation("surface_distance");
  glUniform1f(surfdist_location, ComputeDistanceToSurface(p_cameraPos));

  if(isSurfaceMode(p_cameraPos))
  {
    m_origin = sphereProjectedPosition(p_cameraPos);
    m_scale = 1000.0f;

    glm::mat4 view = glm::translate(p_view, p_cameraPos);
    view = glm::translate(view, - (p_cameraPos - m_origin) * m_scale);
    //glm::mat4 view = glm::scale(p_view, glm::vec3(1.01f, 1.01f, 1.01f));

    unsigned int scaleorigin_location = m_renderShader.GetUniformLocation("scaleOrigin");
    glUniform1f(scaleorigin_location, m_scale);

    unsigned int translateorigin_location = m_renderShader.GetUniformLocation("translateOrigin");
    glUniform3fv(translateorigin_location, 1,  glm::value_ptr(m_origin));

    SetupUniformAndTextures(m_renderShader, p_vpMat, *terrainMaterial,
    view, p_projection, p_model, p_light, p_cameraPos);
    rxLogInfo("Surface mode !");
  }
  else
  {
    m_origin = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = 1.0f;
    unsigned int scaleorigin_location = m_renderShader.GetUniformLocation("scaleOrigin");
    glUniform1f(scaleorigin_location, 1.0f);

    unsigned int translateorigin_location = m_renderShader.GetUniformLocation("translateOrigin");
    glUniform3fv(translateorigin_location, 1,  glm::value_ptr(m_origin));

    SetupUniformAndTextures(m_renderShader, p_vpMat, *terrainMaterial,
      p_view, p_projection, p_model, p_light, p_cameraPos);
  }

  unsigned int near_location = m_renderShader.GetUniformLocation("near");
  glUniform1f(near_location, m_near);

  unsigned int far_location = m_renderShader.GetUniformLocation("far");
  glUniform1f(far_location, m_far);

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glBindVertexArray(m_vertexArrayId);
  glDrawArrays(GL_PATCHES, 0, 4*m_leafCount);
}

int TerrainLOD::DrawWater(glm::mat4 const& p_vpMat, glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos,
    rx::OceanSurface const& p_surf, unsigned int p_waterSurfTex)
{
  glUseProgram(m_renderShader.GetProgram());
  unsigned block_index = 0;
  block_index = glGetProgramResourceIndex(m_renderShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutDataQuad");
  glShaderStorageBlockBinding(m_renderShader.GetProgram(), block_index, 3);

  block_index = glGetProgramResourceIndex(m_renderShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutColorQuad");
  glShaderStorageBlockBinding(m_renderShader.GetProgram(), block_index, 4);

  block_index = glGetProgramResourceIndex(m_renderShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutOuterTess");
  glShaderStorageBlockBinding(m_renderShader.GetProgram(), block_index, 5);

  unsigned int size_location = m_renderShader.GetUniformLocation("size");
  glUniform1f(size_location, m_size);

  SetupUniformAndTextures(m_renderShader, p_vpMat, *terrainMaterial,
    p_view, p_projection, p_model, p_light, p_cameraPos);

  //Bind water heightmap to tex 0
  unsigned hLocation = m_renderShader.GetUniformLocation("toto");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, p_waterSurfTex);
  glTexSubImage2D(GL_TEXTURE_2D,0,0,0,128,128,GL_RED,GL_FLOAT,p_surf.m_heightmapData);
  glUniform1i(hLocation, 0);

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glBindVertexArray(m_vertexArrayId);
  glDrawArrays(GL_PATCHES, 0, 4*m_leafCount);
}

void TerrainLOD::SetOuterTessellationLvl(std::vector<QuadTreeNode*>& p_leafs)
{

  for(unsigned i=0; i < p_leafs.size(); i++)
  {
    unsigned int lvl = p_leafs[i]->level;
    for(unsigned int j = 0; j < 4; ++j)
    {
      QuadTreeNode* nei = p_leafs[i]->FindNeighbour((DirectionType)j);
      if(nei != nullptr && nei->level < lvl)
      {
        p_leafs[i]->outerTessLevel[j] = m_tessellationLevel[nei->level] / 2;
      }
      else
      {
        p_leafs[i]->outerTessLevel[j] = m_tessellationLevel[lvl];
      }
    }
  }
}

void TerrainLOD::BuildPrimitiveBuffers(std::vector<QuadTreeNode*> const& p_leafs)
{
  rxLogDebug("Prepare buffer for "<< p_leafs.size()<< " leafs");
  float* dataQuad = new float[12*p_leafs.size()];
  float* dataSSBO = new float[4*p_leafs.size()];
  float* dataColorSSBO = new float[4*p_leafs.size()];
  float* dataOuterTessSSBO = new float[4*p_leafs.size()];
  unsigned idx = 0;
  unsigned idxTrans = 0;

  unsigned leafToShow = 0;

  QuadTreeNode* northN = p_leafs[leafToShow]->FindNeighbour(DirectionType::eNorth);
  QuadTreeNode* southN = p_leafs[leafToShow]->FindNeighbour(DirectionType::eSouth);
  QuadTreeNode* eastN = p_leafs[leafToShow]->FindNeighbour(DirectionType::eEast);
  QuadTreeNode* westN = p_leafs[leafToShow]->FindNeighbour(DirectionType::eWest);

  std::map<int, int> count;

  for(unsigned i=0; i < p_leafs.size(); i++)
  {
    float scale = 1.0f / p_leafs[i]->level;

    dataQuad[idx] = p_leafs[i]->geometry[0].x;
    dataQuad[idx+1] = p_leafs[i]->geometry[0].y;
    dataQuad[idx+2] = p_leafs[i]->geometry[0].z;

    dataQuad[idx+3] = p_leafs[i]->geometry[1].x;
    dataQuad[idx+4] = p_leafs[i]->geometry[1].y;
    dataQuad[idx+5] = p_leafs[i]->geometry[1].z;

    dataQuad[idx+6] = p_leafs[i]->geometry[2].x;
    dataQuad[idx+7] = p_leafs[i]->geometry[2].y;
    dataQuad[idx+8] = p_leafs[i]->geometry[2].z;

    dataQuad[idx+9] = p_leafs[i]->geometry[3].x;
    dataQuad[idx+10] = p_leafs[i]->geometry[3].y;
    dataQuad[idx+11] = p_leafs[i]->geometry[3].z;

    glm::vec3 color;
    if(p_leafs[i]->level == 16)
    {
      color = glm::vec3(0.0, 0.0, 0.0);//black
    }
    else if(p_leafs[i]->level == 32)
    {
      color = glm::vec3(0.0, 0.0, 1.0);//blue
    }
    else if(p_leafs[i]->level == 64)
    {
      color = glm::vec3(0.0, 1.0, 0.0);//green
    }
    else if(p_leafs[i]->level == 128)
    {
      color = glm::vec3(0.0, 1.0, 1.0);//turquoise
    }
    else if(p_leafs[i]->level == 256)
    {
      color = glm::vec3(1.0, 0.0, 0.0);//red
    }
    else if(p_leafs[i]->level == 512)
    {
      color = glm::vec3(1.0, 0.0, 1.0);//purple
    }
    else if(p_leafs[i]->level == 1024)
    {
      color = glm::vec3(1.0, 1.0, 0.0);//yellow
    }

    dataColorSSBO[idxTrans] = color.x;
    dataColorSSBO[idxTrans+1] = color.y;
    dataColorSSBO[idxTrans+2]= color.z;
    dataColorSSBO[idxTrans+3]= 1.0;

    dataOuterTessSSBO[idxTrans] = p_leafs[i]->outerTessLevel[0];
    dataOuterTessSSBO[idxTrans+1] = p_leafs[i]->outerTessLevel[1];
    dataOuterTessSSBO[idxTrans+2]= p_leafs[i]->outerTessLevel[2];
    dataOuterTessSSBO[idxTrans+3]= p_leafs[i]->outerTessLevel[3];

    idx+=12;
    idxTrans+=4;

    if(count.find(p_leafs[i]->level) == count.end())
    {
      count[p_leafs[i]->level] = 1;
    }
    else
    {
      count[p_leafs[i]->level]++;
    }
  }

  for(auto it = count.begin(); it != count.end(); ++it)
  {
    std::cout<<it->first<<"/"<<it->second<<" ";
  }

  std::cout<<std::endl;

  unsigned int vpos_location = m_renderShader.GetAttributeLocation("vPos");
  glGenBuffers(1, &m_vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, p_leafs.size()*12*sizeof(float), dataQuad, GL_STATIC_DRAW);

  glGenVertexArrays(1, &m_vertexArrayId);
  glBindVertexArray(m_vertexArrayId);

  glEnableVertexAttribArray(vpos_location);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
  glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &m_quadDataSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_quadDataSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, p_leafs.size()*4*sizeof(float), dataSSBO, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_quadDataSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

  glGenBuffers(1, &m_quadColorSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_quadColorSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, p_leafs.size()*4*sizeof(float), dataColorSSBO, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_quadColorSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

  glGenBuffers(1, &m_quadOuterTessSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_quadOuterTessSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, p_leafs.size()*4*sizeof(float), dataOuterTessSSBO, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_quadOuterTessSSBO);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

  delete[] dataQuad;
  delete[] dataSSBO;
  delete[] dataColorSSBO;
  delete[] dataOuterTessSSBO;
}

void TerrainLOD::CullLeaves(std::vector<QuadTreeNode*>& p_leafs, glm::vec3 const& p_cameraPosition)
{
  std::vector<QuadTreeNode*> notculled;
  float surfaceDist = std::max(ComputeDistanceToSurface(p_cameraPosition), 0.001f);
  float horizon = sqrt((surfaceDist + m_size ) * (surfaceDist + m_size) - m_size * m_size);

  glm::vec3 viewVec = glm::normalize(p_cameraPosition);
  for(unsigned int i=0; i < p_leafs.size(); ++i)
  {
    glm::vec3 centerReal = glm::normalize(p_leafs[i]->center) * m_size;
    float distNode = glm::distance(centerReal, p_cameraPosition);
    float cdotv = glm::dot(p_leafs[i]->center, viewVec);
    if( cdotv > 0 && distNode < horizon)
    {
      notculled.push_back(p_leafs[i]);
    }
  }
  rxLogInfo("SurfaceDist " << surfaceDist << " threshold "<< horizon <<" "<<p_leafs.size() - notculled.size() << " culled");
  p_leafs = notculled;
}

void TerrainLOD::SetSize(float p_size)
{
  m_size = p_size;
}

float TerrainLOD::GetSize() const
{
  return m_size;
}

float TerrainLOD::GetNear() const
{
  return m_near;
}

float TerrainLOD::GetFar() const
{
  return m_far;
}

void TerrainLOD::SetShader(Shader const& p_shader)
{
  m_renderShader = p_shader;
}

float TerrainLOD::ComputeDistanceToSurface(glm::vec3 const& p_position)
{
  //Approximation
  //Simple distance to the surface
  //of a sphere of diameter m_size
  //centered at origin
  return std::fabs(glm::length(p_position) - m_size);
}

bool TerrainLOD::isSurfaceMode(glm::vec3 const& p_position)
{
  return ComputeDistanceToSurface(p_position) < 0.01f;
}

glm::vec3 TerrainLOD::sphereProjectedPosition(glm::vec3 const& p_position)
{
  return glm::normalize(p_position) * m_size;
}

void TerrainLOD::ComputeNearAndFar(glm::vec3 const& p_position, glm::vec3 const& p_direction, std::vector<QuadTreeNode*>& p_leafs)
{

  //rxLogInfo("Direction " << p_direction.x << " " << p_direction.y << " " << p_direction.z );
  float minDistance = std::numeric_limits<float>::max();
  float maxDistance = std::numeric_limits<float>::min();

  int leafInSight = 0;
  for(unsigned int i=0; i < p_leafs.size(); ++i)
  {
    for(unsigned int j = 0; j < 4; ++j)
    {
      glm::vec3 dirG = (p_leafs[i]->geometry[j] * m_size) - p_position;
      float h = glm::length(dirG);
      dirG = glm::normalize(dirG);
      float cosA = glm::dot(p_direction, dirG);
      if ( cosA < 0.707 ) continue;
      float d = h * cosA;
      if( d < minDistance ) minDistance = d;
      if( d > maxDistance ) maxDistance = d;
    }
  }

  m_near = std::max(minDistance, 0.000001f) * m_scale;
  m_far = maxDistance * m_scale;
  rxLogInfo("Computed near and far " << m_near << " " << m_far << " in sight " << leafInSight );

  if(isSurfaceMode(p_position))
  {
    float distTerrain = ComputeDistanceToSurface(p_position);
    m_near = 0.001;
    m_far = sqrt((distTerrain + m_size ) * (distTerrain + m_size ) - m_size  * m_size );
    m_far = 300.0f;
  }
}

QuadTreeNode::QuadTreeNode(unsigned p_level, glm::vec3 const& p_position)
{
  childs[0] = nullptr;
  childs[1] = nullptr;
  childs[2] = nullptr;
  childs[3] = nullptr;
  level = p_level;
  center = p_position;
  leaf = true;
}

void QuadTreeNode::SetGeometry(glm::vec3 const& p_1, glm::vec3 const& p_2,
    glm::vec3 const& p_3, glm::vec3 const& p_4)
{
  geometry[0] = glm::normalize(p_1);
  geometry[1] = glm::normalize(p_2);
  geometry[2] = glm::normalize(p_3);
  geometry[3] = glm::normalize(p_4);
}

void QuadTreeNode::Split()
{
  leaf = false;
  glm::vec3 p_5 = geometry[0] + (geometry[3] - geometry[0]) * 0.5f;
  glm::vec3 p_6 = geometry[3] + (geometry[2] - geometry[3]) * 0.5f;
  glm::vec3 p_7 = geometry[2] + (geometry[1] - geometry[2]) * 0.5f;
  glm::vec3 p_8 = geometry[1] + (geometry[0] - geometry[1]) * 0.5f;
  glm::vec3 p_9 = geometry[0] + (geometry[2] - geometry[0]) * 0.5f;

  float scale = 1.0f/level;
  float subsize = 1.0f/4.0f;
  //glm::vec3 nwpos = center + glm::vec3(-subsize, 0.0f, -subsize) * scale;
  glm::vec3 nwpos = geometry[0] + (p_9 - geometry[0]) * 0.5f;
  childs[0] = new QuadTreeNode(level*2, nwpos);
  childs[0]->parent = this;
  childs[0]->SetGeometry(geometry[0], p_8, p_9, p_5);
  //glm::vec3 nepos = center + glm::vec3(subsize, 0.0f, -subsize) * scale;
  glm::vec3 nepos = geometry[3] + (p_9 - geometry[3]) * 0.5f;
  childs[1] = new QuadTreeNode(level*2, nepos);
  childs[1]->parent = this;
  childs[1]->SetGeometry(p_5, p_9, p_6, geometry[3]);
  //glm::vec3 swpos = center + glm::vec3(-subsize, 0.0f, subsize) * scale;
  glm::vec3 swpos = geometry[1] + (p_9 - geometry[1]) * 0.5f;
  childs[2] = new QuadTreeNode(level*2, swpos);
  childs[2]->parent = this;
  childs[2]->SetGeometry(p_8, geometry[1], p_7, p_9);
  //glm::vec3 sepos = center + glm::vec3(subsize, 0.0f, subsize) * scale;
  glm::vec3 sepos = geometry[2] + (p_9 - geometry[2]) * 0.5f;
  childs[3] = new QuadTreeNode(level*2, sepos);
  childs[3]->parent = this;
  childs[3]->SetGeometry(p_9, p_7, geometry[2], p_6);
}

unsigned int QuadTreeNode::GetChildIdInParent()
{
  assert(parent != nullptr);
  for(unsigned int i = 0; i < 4; ++i)
  {
    if(parent->childs[i] == this)
    {
      return i;
    }
  }
}

QuadTreeNode* QuadTreeNode::FindNeighbour(DirectionType p_direction)
{
  //Deux voisins evidents en fonction de la position du noeud dans le parent :
  if(parent == nullptr)
  {
    return nullptr;
  }
  unsigned int idInParent = GetChildIdInParent();
  switch(idInParent)
  {
    case 0 ://NW 00
      if(p_direction == DirectionType::eSouth)
      {
        return parent->childs[2];//SW
      }
      else if(p_direction == DirectionType::eEast)
      {
        return parent->childs[1];//NE
      }
    break;
    case 1 ://NE 01
      if(p_direction == DirectionType::eSouth)
      {
        return parent->childs[3];//SE
      }
      else if(p_direction == DirectionType::eWest)
      {
        return parent->childs[0];//NW
      }
    break;
    case 2 ://SW 10
      if(p_direction == DirectionType::eNorth)
      {
        return parent->childs[0];//NW
      }
      else if(p_direction == DirectionType::eEast)
      {
        return parent->childs[3];//NE
      }
    break;
    case 3 ://SE 11
      if(p_direction == DirectionType::eNorth)
      {
        return parent->childs[1];//NE
      }
      else if(p_direction == DirectionType::eWest)
      {
        return parent->childs[2];//SW
      }
    break;
  }

  //Remonter jusqu'a pouvoir aller dans la direction p_direction et empiler l'axe orthognale à p_direction
  //eNorth
  //Remonter jusqu'a trouver un noeud parent SW ou SE
  unsigned int q1, q2;
  switch(p_direction)
  {
    case eNorth:
      q1 = 2; q2 = 3;
      break;
    case eSouth:
      q1 = 0; q2 = 1;
      break;
    case eWest:
      q1 = 1; q2 = 3;
      break;
    case eEast:
      q1 = 0; q2 = 2;
      break;
  }

  bool found = false;
  QuadTreeNode* currentNode = this;
  std::vector<unsigned int> dirTaken;
  while(found == false && currentNode->parent != nullptr)
  {
    idInParent = currentNode->GetChildIdInParent();
    dirTaken.push_back(idInParent);
    if(idInParent == q1 || idInParent == q2)
    {
      found = true;
      switch(p_direction)
      {
        case eNorth:
          if(idInParent == 2) currentNode = currentNode->parent->childs[0];
          if(idInParent == 3) currentNode = currentNode->parent->childs[1];
          break;
        case eSouth:
          if(idInParent == 0) currentNode = currentNode->parent->childs[2];
          if(idInParent == 1) currentNode = currentNode->parent->childs[3];
          break;
        case eWest:
          if(idInParent == 1) currentNode = currentNode->parent->childs[0];
          if(idInParent == 3) currentNode = currentNode->parent->childs[2];
          break;
        case eEast:
          if(idInParent == 0) currentNode = currentNode->parent->childs[1];
          if(idInParent == 2) currentNode = currentNode->parent->childs[3];
          break;
      }
      dirTaken.pop_back();
      break;
    }
    currentNode = currentNode->parent;
  }
  //Redescendre en dépilant la liste d'axe orthognale à p_direction pour s'orienter
  if(found == true)
  {
    while(currentNode->leaf == false)
    {
      unsigned int q = dirTaken.back();
      dirTaken.pop_back();
      switch(p_direction)
      {
        case eNorth://S
          if(q == 0|| q == 2)//W
          {
            currentNode = currentNode->childs[2];
          }
          else//E
          {
            currentNode = currentNode->childs[3];
          }
          break;
        case eSouth://N
          if(q == 0 || q == 2)//W
          {
            currentNode = currentNode->childs[0];
          }
          else//E
          {
            currentNode = currentNode->childs[1];
          }
          break;
        case eWest://E
          if(q == 0 || q == 1)//N
          {
            currentNode = currentNode->childs[1];
          }
          else//S
          {
            currentNode = currentNode->childs[3];
          }
          break;
        case eEast://W
          if(q == 0 || q == 1)//N
          {
            currentNode = currentNode->childs[0];
          }
          else//S
          {
            currentNode = currentNode->childs[2];
          }
          break;
      }
    }
    return currentNode;
  }
  else
  {
    return nullptr;
  }
  return nullptr;
}

QuadTreeNode::~QuadTreeNode()
{
}

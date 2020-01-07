#include "TerrainLOD.hxx"
#include <glm/gtc/type_ptr.hpp>

#define sqrt2rx 1.41421356237f

TerrainLOD::TerrainLOD()
{
  m_root = new QuadTreeNode(1, glm::vec3(0.0f, 0.0f, 0.0f));

  //8 level quatree test
  for(unsigned i=0; i < 6; ++i)
  {
    std::vector<QuadTreeNode*> leafs;
    FindLeafs(leafs);
    std::cout<<"Split " << i <<" got "<< leafs.size() <<" leafs."<<std::endl;
    for(unsigned j=0; j < leafs.size(); ++j)
    {
      leafs[j]->Split();
    }
  }

  m_scale = 1.0f;

  m_levelDistances[1] = 5.0 * m_scale;
  m_levelDistances[2] = 2.5 * m_scale;
  m_levelDistances[4] = 1.25* m_scale;
  m_levelDistances[8] = 0.75 * m_scale;
  m_levelDistances[16] = 0.35 * m_scale;
  m_levelDistances[32] = 0.150 * m_scale;
  m_levelDistances[64] = 0.75 * m_scale;
  m_levelDistances[128] = 0.35 * m_scale;
  m_levelDistances[256] = 0.150 * m_scale;
  m_levelDistances[512] = 0.075 * m_scale;
  m_levelDistances[1024] = 0.035 * m_scale;

  m_terrainShader.SetVertexShaderSrc("renderTerrainVS.shader");
  m_terrainShader.SetFragmentShaderSrc("renderTerrainFS.shader");
  m_terrainShader.SetTessControlSrc("terrainTC.shader");
  m_terrainShader.SetTessEvalSrc("terrainTE.shader");
  m_terrainShader.LinkProgram();

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

void TerrainLOD::PrepareBufferQuad()
{
  std::vector<QuadTreeNode*> leafs;
  FindLeafs(leafs);
  m_leafCount = leafs.size();
  BuildPrimitiveBuffers(leafs);
}

void TerrainLOD::BuildSimple()
{
  float posPri[12] = {-1.0, 0.0, -1.0,
    -1.0, 0.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, -1.0};

  unsigned int vpos_location = m_terrainShader.GetAttributeLocation("vPos");

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
  nodeToExplore.push_back(m_root);

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
  nodeToExplore.push_back(m_root);

  while(nodeToExplore.size() > 0)
  {
    QuadTreeNode* last = nodeToExplore.back();
    nodeToExplore.pop_back();

    float dist = glm::distance(p_cameraPosition, last->center);
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
      if(dist < m_levelDistances[last->level])
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
//   RecomputeTree(p_cameraPos);
//   std::vector<QuadTreeNode*> leafs;
//   FindLeafs(leafs);
//
//   rxLogDebug("Draw "<< leafs.size()<< " leafs");
//   for(unsigned i=0; i < leafs.size(); i++)
//   {
//     p_material.SetData("Kd", glm::vec3(std::fabs(leafs[i]->center.x / 0.5), std::fabs(leafs[i]->center.z / 0.5), 0.0));
//     glm::mat4 model = glm::mat4(1.0f);
//     float scaleFactor = 1.0f* m_scale / leafs[i]->level;
//     model = glm::translate(model, -leafs[i]->center * m_scale);
//     model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
//     SetTransform(model);
//     DrawableItem::Draw(p_shader, p_vpMat, p_material, p_view,
//       p_projection, model, p_light, p_cameraPos);
//   }
  //exit(0);
}

int TerrainLOD::DrawTerrain(glm::mat4 const& p_vpMat,
    glm::mat4 const& p_view, glm::mat4 const& p_projection,
    glm::mat4 const& p_model, glm::vec3 const& p_light,
    glm::vec3 const& p_cameraPos)
{
  glUseProgram(m_terrainShader.GetProgram());
  unsigned block_index = 0;
  block_index = glGetProgramResourceIndex(m_terrainShader.GetProgram(), GL_SHADER_STORAGE_BLOCK, "layoutDataQuad");
  glShaderStorageBlockBinding(m_terrainShader.GetProgram(), block_index, 3);

  SetupUniformAndTextures(m_terrainShader, p_vpMat, *terrainMaterial,
    p_view, p_projection, p_model, p_light, p_cameraPos);

  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glBindVertexArray(m_vertexArrayId);
  glDrawArrays(GL_PATCHES, 0, 4*m_leafCount);
}

void TerrainLOD::BuildPrimitiveBuffers(std::vector<QuadTreeNode*> const& p_leafs)
{
  rxLogDebug("Prepare buffer for "<< p_leafs.size()<< " leafs");
  float* dataQuad = new float[12*p_leafs.size()];
  float* dataSSBO = new float[4*p_leafs.size()];
  unsigned idx = 0;
  unsigned idxTrans = 0;
  for(unsigned i=0; i < p_leafs.size(); i++)
  {
    dataQuad[idx] = -0.5; dataQuad[idx+1] = 0.0; dataQuad[idx+2] = -0.5;
    dataQuad[idx+3] = -0.5; dataQuad[idx+4] = 0.0; dataQuad[idx+5] = 0.5;
    dataQuad[idx+6] = 0.5; dataQuad[idx+7] = 0.0; dataQuad[idx+8] = 0.5;
    dataQuad[idx+9] = 0.5; dataQuad[idx+10] = 0.0; dataQuad[idx+11] = -0.5;

    float scaleFactor = 1.0f * m_scale / p_leafs[i]->level;
    dataSSBO[idxTrans] = -p_leafs[i]->center.x;
    dataSSBO[idxTrans+1] = -p_leafs[i]->center.y;
    dataSSBO[idxTrans+2]= -p_leafs[i]->center.z;
    dataSSBO[idxTrans+3]= scaleFactor;

    idx+=12;
    idxTrans+=4;
  }

  unsigned int vpos_location = m_terrainShader.GetAttributeLocation("vPos");
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

  delete[] dataQuad;
  delete[] dataSSBO;
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

void QuadTreeNode::Split()
{
  leaf = false;
  float scale = 1.0f/level;
  float subsize = 1.0f/4.0f;
  glm::vec3 nwpos = center + glm::vec3(-subsize, 0.0f, -subsize) * scale;
  childs[0] = new QuadTreeNode(level*2, nwpos);
  glm::vec3 nepos = center + glm::vec3(subsize, 0.0f, -subsize) * scale;
  childs[1] = new QuadTreeNode(level*2, nepos);
  glm::vec3 swpos = center + glm::vec3(subsize, 0.0f, subsize) * scale;
  childs[2] = new QuadTreeNode(level*2, swpos);
  glm::vec3 sepos = center + glm::vec3(-subsize, 0.0f, subsize) * scale;
  childs[3] = new QuadTreeNode(level*2, sepos);
}

QuadTreeNode::~QuadTreeNode()
{
}

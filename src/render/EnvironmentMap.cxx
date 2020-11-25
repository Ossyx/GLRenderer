#include "EnvironmentMap.hxx"

#include <glm/gtc/type_ptr.hpp>

EnvironmentMap::EnvironmentMap(rx::MaterialPtr pCubeMapMaterial, ShaderPtr pShader):
mCubeMapMaterial(pCubeMapMaterial),
mShader(pShader)
{
}

EnvironmentMap::~EnvironmentMap()
{
}

void EnvironmentMap::PrepareTextureFromMaterial(rx::Material const& p_material,
  Shader const& p_shader)
{
  bool loadtex = true;
  if(DrawableItem::m_savedIdsAndLocations.find(p_material.GetName()) != DrawableItem::m_savedIdsAndLocations.end())
  {
    loadtex = false;
    m_textureIdsLocation = DrawableItem::m_savedIdsAndLocations[p_material.GetName()];
  }
  mCubeMapLocation = p_shader.GetUniformLocation("titi");
  
  std::string uniformName[6] = 
  {
    "map_px",
    "map_nx",
    "map_py",
    "map_ny",
    "map_pz",
    "map_nz"
  };
  
  glGenTextures(1, &mCubeMapTex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMapTex);
  
  for (unsigned int i = 0; i < 6; i++)
  {
    if (p_material.HasFloatTexData(uniformName[i]))
    {
      rxLogWarning("Loading CubeMap channel " << i );
      rx::Material::FloatTexture const& tex = p_material.GetFloatTexture(uniformName[i]);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, tex.m_width, tex.m_height, 0, GL_RGB, GL_FLOAT, tex.m_data);
    }
    else
    {
      rxLogError("No attribute " << uniformName[i] <<" found in material " << p_material.GetName());
    }
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  
    
  for(auto elem : p_shader.GetUniformMap())
  {
    rxLogDebug("Got uniform : " << elem.first );
  }
  
  if(loadtex == true)
  {
    DrawableItem::m_savedIdsAndLocations[p_material.GetName()] = m_textureIdsLocation;
  }
  
  rxLogInfo("Cube map texture location " << mCubeMapLocation);
  rxLogInfo("Cube map texture id " << mCubeMapTex);
}

void EnvironmentMap::InitCubeMap()
{
  PrepareBufferFromMesh(mCubeMesh, *mShader);
  PrepareTextureFromMaterial(*mCubeMapMaterial, *mShader);
}

void EnvironmentMap::DrawCubeMap(glm::mat4 const& p_view,
  glm::mat4 const& p_projection, glm::mat4 const& p_model)
{
  glUseProgram(mShader->GetProgram());
  
  unsigned int viewLoc = mShader->GetUniformLocation("View");
  unsigned int projection = mShader->GetUniformLocation("Projection");
  unsigned int model = mShader->GetUniformLocation("Model");
  unsigned int toto = mShader->GetUniformLocation("toto");
  
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(p_view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(p_projection));
  glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(p_model));
  glUniform1f(toto, 0.5f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMapTex);
  glUniform1i(mCubeMapLocation, /*GL_TEXTURE*/ 0);
  
  DrawElements();
}



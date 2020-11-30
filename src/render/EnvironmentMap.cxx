#include "EnvironmentMap.hxx"

#include <glm/gtc/type_ptr.hpp>

EnvironmentMap::EnvironmentMap(rx::MeshPtr pMesh, rx::MaterialPtr pMaterial, ShaderPtr pShader):
DrawableItem(pMesh, pMaterial, pShader)
{
}

EnvironmentMap::~EnvironmentMap()
{
}

void EnvironmentMap::PrepareTextureFromMaterial()
{
  bool loadtex = true;
  if(DrawableItem::m_savedIdsAndLocations.find(mMaterial->GetName()) != DrawableItem::m_savedIdsAndLocations.end())
  {
    loadtex = false;
    m_textureIdsLocation = DrawableItem::m_savedIdsAndLocations[mMaterial->GetName()];
  }
  mCubeMapLocation = mShader->GetUniformLocation("cubeMap");
  
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
    if (mMaterial->HasTextureData<float>(uniformName[i]))
    {
      rxLogWarning("Loading CubeMap channel " << i );
      rx::Material::FloatTexture const& tex = mMaterial->GetTextureData<float>(uniformName[i]);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, tex->m_width, tex->m_height, 0, GL_RGB, GL_FLOAT, tex->m_data);
    }
    else
    {
      rxLogError("No attribute " << uniformName[i] <<" found in material " << mMaterial->GetName());
    }
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  
    
  for(auto elem : mShader->GetUniformMap())
  {
    rxLogDebug("Got uniform : " << elem.first );
  }
  
  if(loadtex == true)
  {
    DrawableItem::m_savedIdsAndLocations[mMaterial->GetName()] = m_textureIdsLocation;
  }
  
  rxLogInfo("Cube map texture location " << mCubeMapLocation);
  rxLogInfo("Cube map texture id " << mCubeMapTex);
}

void EnvironmentMap::Draw(glm::mat4 const& p_view,
    glm::mat4 const& p_projection, glm::mat4 const& p_model,
    glm::vec3 const& p_light, glm::vec3 const& p_cameraPos)
{
  glUseProgram(mShader->GetProgram());
  
  unsigned int viewLoc = mShader->GetUniformLocation("View");
  unsigned int projection = mShader->GetUniformLocation("Projection");
  unsigned int model = mShader->GetUniformLocation("Model");
  
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(p_view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(p_projection));
  glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(p_model));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMapTex);
  glUniform1i(mCubeMapLocation, /*GL_TEXTURE*/ 0);
  
  DrawElements();
}



#include "MaterialTexturesHandle.hxx"

MaterialTextureHandle::TypeToConfig MaterialTextureHandle::smGLTextureConfig =
{
  { GL_UNSIGNED_BYTE, {
      { 1, {GL_RED, GL_RED} },
      { 2, {GL_RG, GL_RG} },
      { 3, {GL_RGB, GL_RGB} },
      { 4, {GL_RGBA, GL_RGBA} },
  }},
  { GL_UNSIGNED_SHORT, {
      { 1, {GL_R16, GL_RED} },
      { 2, {GL_RG16, GL_RG} },
      { 3, {GL_RGB16, GL_RGB} },
      { 4, {GL_RGBA16, GL_RGBA} },
  }},
  { GL_FLOAT, {
      { 1, {GL_R32F, GL_RED} },
      { 2, {GL_RG32F, GL_RG} },
      { 3, {GL_RGB32F, GL_RGB} },
      { 4, {GL_RGBA32F, GL_RGBA} },
  }}
};

MaterialTextureHandle::MaterialTextureHandle(rx::MaterialPtr pMaterial)
{
  for(auto uniformD : pMaterial->GetUniforms())
  {
    std::string uniform = uniformD.first;
    std::string matAttribute = uniformD.second;
    
    if (pMaterial->HasUCharTexData(matAttribute))
    {
      rxLogInfo("Uniform "<< uniform <<" is attribute " <<matAttribute<<" of type UByte");
      GenerateTexture<rx::Material::ByteTexture>(
        pMaterial->GetByteTexture(matAttribute), GL_UNSIGNED_BYTE, uniform);
    }
    else if(pMaterial->HasUShortTexData(matAttribute))
    {
      rxLogInfo("Uniform "<< uniform <<" is attribute " <<matAttribute<<" of type Texture UShort");
      GenerateTexture<rx::Material::UShortTexture>(
        pMaterial->GetUShortTexture(matAttribute), GL_UNSIGNED_SHORT, uniform);
    }
    else if(pMaterial->HasFloatTexData(matAttribute))
    {
      rxLogInfo("Uniform "<< uniform <<" is attribute " <<matAttribute<<" of type Texture Float");
      GenerateTexture<rx::Material::FloatTexture>(
        pMaterial->GetFloatTexture(matAttribute), GL_FLOAT, uniform);
    }
  }
}

MaterialTextureHandle::~MaterialTextureHandle()
{
  
}

template <typename T>
void MaterialTextureHandle::GenerateTexture(T const& pTex, GLenum pType, std::string const& pUniform)
{
  unsigned int textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
  auto texConf = smGLTextureConfig[pType][pTex.m_channelCount];
  glTexImage2D(GL_TEXTURE_2D, 0, texConf.internalFormat, pTex.m_width,
      pTex.m_height, 0, texConf.format, pType, pTex.m_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  mTexturedIds[pUniform] = textureId;
}

MaterialTextureHandle::TextureIdMap MaterialTextureHandle::TextureIds()
{
  return mTexturedIds;
}



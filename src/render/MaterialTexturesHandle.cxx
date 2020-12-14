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

MaterialTextureHandle::MaterialTextureHandle()
{
}

MaterialTextureHandle::MaterialTextureHandle(rx::MaterialPtr pMaterial)
{
  for(auto attribute : pMaterial->GetMembers())
  {    
    if (pMaterial->HasTextureData<unsigned char>(attribute))
    {
      GenerateTexture(pMaterial->GetTextureData<unsigned char>(attribute), GL_UNSIGNED_BYTE, attribute);
    }
    else if(pMaterial->HasTextureData<unsigned short>(attribute))
    {
      GenerateTexture(pMaterial->GetTextureData<unsigned short>(attribute), GL_UNSIGNED_SHORT, attribute);
    }
    else if(pMaterial->HasTextureData<float>(attribute))
    {
      GenerateTexture(pMaterial->GetTextureData<float>(attribute), GL_FLOAT, attribute);
    }
  }
}

MaterialTextureHandle::~MaterialTextureHandle()
{
}

template <typename T>
void MaterialTextureHandle::GenerateTexture(T pTex, GLenum pType, std::string const& pAttribute)
{
  GLTexture t;
  t.Build();
  t.Bind();
  t.SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  t.SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  t.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  t.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  
  auto texConf = smGLTextureConfig[pType][pTex->m_channelCount];
  t.SetData(texConf.internalFormat, texConf.format, pType,
    pTex->m_width, pTex->m_height, pTex->m_data);
  
  t.GenerateMipMap();
  
  mTexturedIds[pAttribute] = t.mId;
  mTextures[t.mId] = t;
}

MaterialTextureHandle::TextureIdMap MaterialTextureHandle::TextureIds()
{
  return mTexturedIds;
}

MaterialTextureHandle::TextureMap MaterialTextureHandle::Textures()
{
  return mTextures;
}




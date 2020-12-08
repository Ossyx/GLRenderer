#ifndef MATERIALTEXTUREHANDLE_HXX
#define MATERIALTEXTUREHANDLE_HXX

#include <unordered_map>
#include "Material.hxx"
#include "GLAbstraction.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class MaterialTextureHandle
{
public:
  MaterialTextureHandle();
  MaterialTextureHandle(rx::MaterialPtr pMaterial);  
  ~MaterialTextureHandle();

  struct GLTexConfig
  {
    GLenum internalFormat;
    GLenum format;
  };
  
  //channelCount -> [internalFormat, format]
  using ChannelCountToFormat = std::unordered_map<unsigned int, GLTexConfig>;
  //Type -> channelCount -> [internalFormat, format]
  using TypeToConfig = std::unordered_map<GLenum, ChannelCountToFormat>;  
  static TypeToConfig smGLTextureConfig;
  
  using TextureIdMap = std::unordered_map<std::string, unsigned int>;
  using TextureMap = std::unordered_map<unsigned int, GLTexture>;
  
  TextureIdMap TextureIds();
  TextureMap Textures();
  
private:
  
  template <typename T>
  void GenerateTexture(T pTex, GLenum pType, std::string const& pUniform);
  
  TextureIdMap mTexturedIds;
  TextureMap mTextures;
};

using MaterialTextureHandlePtr = std::shared_ptr<MaterialTextureHandle>;

#endif

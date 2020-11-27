#ifndef MATERIALTEXTUREHANDLE_HXX
#define MATERIALTEXTUREHANDLE_HXX

#include <unordered_map>
#include "Material.hxx"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class MaterialTextureHandle
{
public:
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
  
  TextureIdMap TextureIds();
  
private:
  
  template <typename T>
  void GenerateTexture(T const& pTex, GLenum pType, std::string const& pUniform);
  
  TextureIdMap mTexturedIds;
};

using MaterialTextureHandlePtr = std::shared_ptr<MaterialTextureHandle>;

#endif

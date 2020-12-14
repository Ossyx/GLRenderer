#ifndef GLABSTRACTION_HXX
#define GLABSTRACTION_HXX

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>

struct ArrayBuffer
{
  void Build(GLenum pType, GLuint pSize, const void *pData);
  void Bind();
  
  unsigned int mId;
  GLenum mType;
  unsigned int mSize;
};

struct VertexArray
{
  void Build();
  void Bind();
  void BindBufferToLocation(ArrayBuffer const& pBuffer, GLuint pLocation,
    GLint pSize, GLenum pType, GLboolean pNormalized, GLsizei pStride,
    const void *pPointer);
  
  unsigned int mId;
};

struct GLTexture
{
  void Build();
  void Bind();
  void SetParameter(GLenum pParameter, GLenum pValue);
  void SetData(GLenum pInternalFormat, GLenum pFormat, GLenum pType,
    unsigned int pWidth, unsigned int pHeight, const void* data);
  void GenerateMipMap();
  
  unsigned int mId;
  GLenum mInternalFormat;
  GLenum mFormat;
  GLenum mType;
  unsigned int mWidth;
  unsigned int mHeight;
  bool mMipMap = false;
  
};

struct GBufferData
{
  //Frame buffer object
  unsigned int mFBO;
  
  //Render targets
  unsigned int mRT1;
  unsigned int mRT2;
  unsigned int mRT3;
  unsigned int mRT4;
};

struct FramebufferObject
{
  void Build();
  void Bind();
  void Unbind();
  void AddRenderTarget(std::string const& pName, GLenum pAttachment,
    GLenum pInternalFormat, GLenum pFormat, GLenum pType,
    unsigned int pWidth, unsigned int pHeight);
  bool CheckCompletion();
  
  unsigned int mId;
  std::unordered_map<std::string, GLTexture> mRenderTargets;
  std::vector<GLenum> mColorAttachments;
};

#endif

#ifndef GLABSTRACTION_HXX
#define GLABSTRACTION_HXX

#include <glad/glad.h>

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
  unsigned int mId;
  void Build();
  void Bind();
  void SetParameter(GLenum pParameters, GLenum pValue);
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

#endif

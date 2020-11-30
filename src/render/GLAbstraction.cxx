#include "GLAbstraction.hxx"

#include <GLFW/glfw3.h>

void ArrayBuffer::Build(GLenum pType, GLuint pSize,
const void* pData)
{
  mType = pType;
  mSize = pSize;
  glGenBuffers(1, &mId);
  glBindBuffer(mType, mId);
  glBufferData(mType, mSize, pData, GL_STATIC_DRAW);
}

void ArrayBuffer::Bind()
{
  glBindBuffer(mType, mId);
}

void VertexArray::Build()
{
  glGenVertexArrays(1, &mId);
}

void VertexArray::Bind()
{
  glBindVertexArray(mId);
}

void VertexArray::BindBufferToLocation(ArrayBuffer const& pBuffer, GLuint pLocation,
    GLint pSize, GLenum pType, GLboolean pNormalized, GLsizei pStride, const void *pPointer)
{
  glBindVertexArray(mId);
  glEnableVertexAttribArray(pLocation);
  glBindBuffer(pBuffer.mType, pBuffer.mId);
  glVertexAttribPointer(pLocation, pSize, pType, pNormalized, pStride, pPointer);
}

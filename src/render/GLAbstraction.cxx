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

void SSPlaneData::PrepareScreenSpacePlane(unsigned int pVertexLocation)
{
  const float vertex_buffer_data[] =
  {
    -1.0f,  -1.0f,  0.0f,
    1.0f,  -1.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,
    1.0f,  1.0f,  0.0f
  };

  const unsigned int index_buffer[] = {0, 1, 2, 1, 3, 2};

  mVertex.Build(GL_ARRAY_BUFFER, 4*3*sizeof(float), vertex_buffer_data);
  mIndex.Build(GL_ELEMENT_ARRAY_BUFFER, 2*3*sizeof(unsigned int), index_buffer);
  
  mVArray.Build();
  mVArray.Bind();
  mVArray.BindBufferToLocation(mVertex, pVertexLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

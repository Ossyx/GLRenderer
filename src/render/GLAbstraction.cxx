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

void GLTexture::Build()
{
  glGenTextures(1, &mId);
}

void GLTexture::Bind()
{
  glBindTexture(GL_TEXTURE_2D, mId);
}

void GLTexture::SetParameter(GLenum pParameter, GLenum pValue)
{
  Bind();
  glTexParameteri(GL_TEXTURE_2D, pParameter, pValue);
}

void GLTexture::SetData(GLenum pInternalFormat, GLenum pFormat, GLenum pType, unsigned int pWidth, unsigned int pHeight, const void* data)
{
  mInternalFormat = pInternalFormat;
  mFormat = pFormat;
  mType = pType;
  mWidth = pWidth;
  mHeight = pHeight;
  glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, data);  
}

void GLTexture::GenerateMipMap()
{
  glGenerateMipmap(GL_TEXTURE_2D);
  mMipMap = true;
}

void FramebufferObject::Build()
{
  glGenFramebuffers(1, &mId);  
}

void FramebufferObject::Bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, mId);
}

void FramebufferObject::Unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObject::AddRenderTarget(std::string const& pName, GLenum pAttachment, GLenum pInternalFormat, GLenum pFormat, GLenum pType, unsigned int pWidth, unsigned int pHeight)
{
  GLTexture t;
  t.Build();
  t.Bind();
  t.SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  t.SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  t.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  t.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  t.SetData(pInternalFormat, pFormat, pType, pWidth, pHeight, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, pAttachment, GL_TEXTURE_2D, t.mId, 0);  
  mRenderTargets[pName] = t;
  mColorAttachments.push_back(pAttachment);
}



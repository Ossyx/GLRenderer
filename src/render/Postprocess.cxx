#include "Postprocess.hxx"
#include "MaterialTexturesHandle.hxx"


Postprocess::Postprocess(GeometryHandlePtr pGeoHandle, ShaderPtr pShader, 
  GLenum pOutputType, unsigned int pWidth, unsigned int pHeight):
Renderable(pGeoHandle, nullptr, pShader, nullptr),
mOutputType(pOutputType),
mWidth(pWidth),
mHeight(pHeight)
{
  InitializeOutputTarget();
}

Postprocess::~Postprocess()
{
}

void Postprocess::InitializeOutputTarget()
{
  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
  
  glGenTextures(1, &mOutput);
  glBindTexture(GL_TEXTURE_2D, mOutput);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
  std::vector<GLubyte> emptyData(mWidth * mHeight * 4, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth,
      mHeight, 0, GL_RGBA, mOutputType, emptyData.data());
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mOutput, 0);
  
  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("Gbuffer framebuffer incomplete ! ");
    assert(false);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Postprocess::Execute(TextureParameter const& pInput, rx::GLSLTypeStore const& pParameters)
{
//   glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
//   GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
//   glDrawBuffers(1, buffers);
  glUseProgram(mShader->GetProgram());
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pInput.at("input_one"));
  mShader->SetUniform("input_one", 0);
  
  mShader->SetUniform("toto", 0.75f);
  
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);
  
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Postprocess::RetrieveOutput()
{
  return mOutput;
}


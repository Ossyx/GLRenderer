#include "Postprocess.hxx"
#include "MaterialTexturesHandle.hxx"


Postprocess::Postprocess(ShaderPtr pShader, GLenum pOutputType,
  unsigned int pWidth, unsigned int pHeight):
Renderable(std::make_shared<SSPlaneData>(), nullptr, pShader, nullptr),
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
  mFBO.Build();
  mFBO.Bind();
  mFBO.AddRenderTarget("output", GL_COLOR_ATTACHMENT0, GL_RGBA, GL_RGBA, mOutputType, mWidth, mHeight);  
  GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  if (completeness != GL_FRAMEBUFFER_COMPLETE)
  {
    rxLogError("Gbuffer framebuffer incomplete ! ");
    assert(false);
  }
  mFBO.Unbind();
}

void Postprocess::Execute(TextureParameter const& pInput, rx::GLSLTypeStore const& pParameters)
{
  mFBO.Bind();
  glDrawBuffers(1, mFBO.mColorAttachments.data());
  glUseProgram(mShader->GetProgram());
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pInput.at("input_one"));
  mShader->SetUniform("input_one", 0);
  
  mShader->SetUniform("toto", 0.75f);
  
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);
  mFBO.Unbind();
}

unsigned int Postprocess::RetrieveOutput()
{
  return mOutput;
}


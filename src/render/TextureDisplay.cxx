#include "TextureDisplay.hxx"
#include "MaterialTexturesHandle.hxx"


TextureDisplay::TextureDisplay(ShaderPtr pShader):
Renderable(std::make_shared<SSPlaneData>(), nullptr, pShader, nullptr, nullptr)
{
}

TextureDisplay::~TextureDisplay()
{
}

void TextureDisplay::Draw(rx::GLSLTypeStore const& pParameters, Renderable::TextureParameter const& pTexParameters)
{
  glUseProgram(mShader->GetProgram());
  mCurrentIdTexture = 0;
  SetParametersUniforms(pParameters);
  SetTextureUniforms(pTexParameters);
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);

}


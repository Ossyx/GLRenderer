#include "TextureDisplay.hxx"
#include "MaterialTexturesHandle.hxx"


TextureDisplay::TextureDisplay(ShaderPtr pShader):
Renderable(std::make_shared<SSPlaneData>(), nullptr, pShader, nullptr)
{
}

TextureDisplay::~TextureDisplay()
{
}

void TextureDisplay::Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters)
{
  glUseProgram(mShader->GetProgram());  
  SetTextureUniforms(pTexParameters);
  mVertexArray.Bind();
  mGeometryHandle->mIndex.Bind();
  glDrawElements(GL_TRIANGLES, mGeometryHandle->m_elementCount, GL_UNSIGNED_INT, 0);

}


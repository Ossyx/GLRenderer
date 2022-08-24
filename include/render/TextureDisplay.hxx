#ifndef TEXTUREDISPLAY_HXX
#define TEXTUREDISPLAY_HXX

#include "Shader.hxx"
#include "GLAbstraction.hxx"
#include "GeometryHandle.hxx"
#include "Renderable.hxx"
#include "GLRendererExport.hxx"

class GLRenderer_EXPORT TextureDisplay : public Renderable
{
public:
  
  TextureDisplay(ShaderPtr pShader);
  ~TextureDisplay();
  
  virtual void Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters) override;
  
};
#endif

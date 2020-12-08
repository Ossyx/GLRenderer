#ifndef TEXTUREDISPLAY_HXX
#define TEXTUREDISPLAY_HXX

#include "Shader.hxx"
#include "GLAbstraction.hxx"
#include "GeometryHandle.hxx"
#include "Renderable.hxx"

class TextureDisplay : public Renderable
{
public:
  using TextureParameter = std::unordered_map<std::string, unsigned int>;
  
  TextureDisplay(ShaderPtr pShader);
  ~TextureDisplay();
  
  virtual void Draw(rx::GLSLTypeStore const& pParameters, TextureParameter const& pTexParameters) override;
  
};
#endif

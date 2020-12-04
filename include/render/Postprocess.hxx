#ifndef POSTPROCESS_HXX
#define POSTPROCESS_HXX

#include "Shader.hxx"
#include "GLAbstraction.hxx"
#include "GeometryHandle.hxx"
#include "Renderable.hxx"

class Postprocess : public Renderable
{
public:
  using TextureParameter = std::unordered_map<std::string, unsigned int>;
  
  Postprocess(GeometryHandlePtr pGeoHandle, ShaderPtr pShader, 
    GLenum pOutputType, unsigned int pWidth, unsigned int pHeight);
  ~Postprocess();
  
  void SetInput();
  void Execute(TextureParameter const& pInput, rx::GLSLTypeStore const& pParameters);
  unsigned int RetrieveOutput();
  
private:
  
  void InitializeOutputTarget();
  TextureParameter mInput;
  unsigned int mFbo;
  unsigned int mOutput;
  
  GLenum mOutputType;
  unsigned int mWidth;
  unsigned int mHeight;
};
#endif

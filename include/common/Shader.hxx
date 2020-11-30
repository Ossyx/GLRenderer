#ifndef SHADER_HXX
#define SHADER_HXX

#include <string>
#include <unordered_map>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader
{
public:

  Shader();

  Shader(std::string const& p_name);

  ~Shader();

  void SetName(std::string const& p_name);

  void SetVertexShaderSrc(std::string const& p_file);

  void SetFragmentShaderSrc(std::string const& p_file);

  void SetTessControlSrc(std::string const& p_file);

  void SetTessEvalSrc(std::string const& p_file);

  void SetPreprocessorConfig(std::string const& p_config);

  bool LinkProgram();
  
  std::string GetName() const;

  unsigned int GetProgram() const;

  unsigned int GetAttributeLocation(std::string const& p_parameter) const;

  unsigned int GetUniformLocation(std::string const& p_parameter) const;
  
  using UniformInfo = std::pair<GLenum, unsigned int>;

  typedef std::unordered_map<std::string, UniformInfo> UniformMap;
  UniformMap const& GetUniformMap() const;
  
  bool GetLinked();
  
  void SetUniform(unsigned int pLocation, unsigned int pValue);

private:

  unsigned int CompileShader(unsigned int p_shaderType, std::string const& p_source);
  
  void GatherUniforms();

  std::string m_name;

  std::string m_verterShaderSrc;

  std::string m_fragmentShaderSrc;

  std::string m_tessControlSrc;

  std::string m_tessEvalSrc;

  std::string m_preprocessorConfig;
  
  bool m_linked;

  unsigned int m_vertexShader;

  unsigned int m_fragmentShader;

  unsigned int m_tessControlShader;

  unsigned int m_tessEvalShader;

  unsigned int m_program;

  UniformMap m_uniforms;
};

using ShaderPtr = std::shared_ptr<Shader>;

#endif

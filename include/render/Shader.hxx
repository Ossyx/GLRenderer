#ifndef SHADER_HXX
#define SHADER_HXX

#include <string>
#include <unordered_map>

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

  unsigned int GetProgram() const;

  unsigned int GetAttributeLocation(std::string const& p_parameter) const;

  unsigned int GetUniformLocation(std::string const& p_parameter) const;

  typedef std::unordered_map<std::string, GLenum> UniformMap;
  UniformMap const& GetUniformMap() const;

private:

  unsigned int CompileShader(unsigned int p_shaderType, std::string const& p_source);

  std::string m_name;

  std::string m_verterShaderSrc;

  std::string m_fragmentShaderSrc;

  std::string m_tessControlSrc;

  std::string m_tessEvalSrc;

  std::string m_preprocessorConfig;

  unsigned int m_vertexShader;

  unsigned int m_fragmentShader;

  unsigned int m_tessControlShader;

  unsigned int m_tessEvalShader;

  unsigned int m_program;

  UniformMap m_uniforms;
};

#endif

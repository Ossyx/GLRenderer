#include "Shader.hxx"
#include "Common.hxx"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader():
m_vertexShader(-1),
m_fragmentShader(-1),
m_verterShaderSrc(""),
m_fragmentShaderSrc(""),
m_program(-1),
m_name(""),
m_preprocessorConfig(""),
m_linked(false)
{
}

Shader::Shader(std::string const& p_name):
m_vertexShader(-1),
m_fragmentShader(-1),
m_verterShaderSrc(""),
m_fragmentShaderSrc(""),
m_program(-1),
m_name(p_name),
m_preprocessorConfig(""),
m_linked(false)
{
}

Shader::~Shader()
{
}

void Shader::SetName(std::string const& p_name)
{
  m_name = p_name;
}

void Shader::SetVertexShaderSrc(std::string const& p_file)
{
  m_verterShaderSrc = p_file;
}

void Shader::SetFragmentShaderSrc(std::string const& p_file)
{
  m_fragmentShaderSrc = p_file;
}

void Shader::SetTessControlSrc(std::string const& p_file)
{
  m_tessControlSrc = p_file;
}

void Shader::SetTessEvalSrc(std::string const& p_file)
{
  m_tessEvalSrc = p_file;
}

unsigned int Shader::CompileShader(unsigned int p_shaderType, std::string const& p_source)
{
  rxLogInfo("Compiling "<< p_source);
  GLint isCompiled = GL_FALSE;
  int InfoLogLength;
  std::ifstream shaderInput(p_source);
  std::stringstream stream;
  stream << "#version 460" << std::endl;
  stream << m_preprocessorConfig << shaderInput.rdbuf();
  std::string sStr = stream.str();

  unsigned int shaderId = glCreateShader(p_shaderType);
  const char* shaderDataPtr = sStr.c_str();

  glShaderSource(shaderId, 1, &shaderDataPtr, NULL);
  glCompileShader(shaderId);

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);

  if ( isCompiled == 0 )
  {
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    rxLogError("Issue with shader from file "<< p_source);
    rxLogError("Shader compilation failed with following log :");
    char* rawLog = new char[InfoLogLength+1];
    glGetShaderInfoLog(shaderId, InfoLogLength, &InfoLogLength, rawLog);
    rxLogError(rawLog);
    delete [] rawLog;
    assert(false);
  }
  return shaderId;
}

bool Shader::LinkProgram()
{
  m_vertexShader = CompileShader(GL_VERTEX_SHADER, m_verterShaderSrc);
  m_fragmentShader = CompileShader(GL_FRAGMENT_SHADER, m_fragmentShaderSrc);

  if(m_tessControlSrc != "")
  {
    m_tessControlShader = CompileShader(GL_TESS_CONTROL_SHADER, m_tessControlSrc);
    if(m_tessControlShader == -1)
    {
      rxLogError("Tesselation control shader compiltation failed.");
      assert(false);
    }
  }

  if(m_tessEvalSrc != "")
  {
    m_tessEvalShader = CompileShader(GL_TESS_EVALUATION_SHADER, m_tessEvalSrc);
    if(m_tessEvalShader == -1)
    {
      rxLogError("Tesselation evaluation shader compiltation failed.");
      assert(false);
    }
  }

  if (m_vertexShader == -1 || m_fragmentShader == -1)
  {
    rxLogError("An error occured while compiling one or several shaders.");
    assert(false);
  }

  m_program = glCreateProgram();
  glAttachShader(m_program, m_vertexShader);
  if(m_tessControlSrc != "")
  {
    glAttachShader(m_program, m_tessControlShader);
  }
  if(m_tessEvalSrc != "")
  {
    glAttachShader(m_program, m_tessEvalShader);
  }
  glAttachShader(m_program, m_fragmentShader);
  glLinkProgram(m_program);
  
  GLint isLinked = GL_FALSE;  
  glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);  
  if( isLinked == 0 )
  {
    rxLogError("Program for shader " << m_name << " did not linked.");
    int InfoLogLength;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char* rawLog = new char[InfoLogLength+1];
    glGetProgramInfoLog(m_program, InfoLogLength, &InfoLogLength, rawLog);
    rxLogError(rawLog);
    assert(false);
  }

  //Then retrieve the uniforms
  GatherUniforms();
  m_linked = true;
  return true;
}

void Shader::GatherUniforms()
{
  rxLogInfo("Uniform list for shader "<< m_name);
  int activeUniforms = 0;
  glGetProgramInterfaceiv(m_program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &activeUniforms);
  std::vector<GLchar> nameData(256);
  for (int uniformIdx = 0; uniformIdx < activeUniforms; ++uniformIdx)
  {
    GLint arraySize = 0;
    GLenum type = 0;
    GLsizei actualLength = 0;
    glGetActiveUniform(m_program, uniformIdx, nameData.size(),
      &actualLength, &arraySize, &type, &nameData[0]);
    std::string name((char*)&nameData[0], actualLength);
    rxLogInfo("Found uniform "<< name <<" of type "<< type);
    m_uniforms[name] = UniformInfo(type, GetUniformLocation(name));
  }
}

std::string Shader::GetName() const
{
  return m_name;
}

unsigned int Shader::GetProgram() const
{
  return m_program;
}

unsigned int Shader::GetAttributeLocation(std::string const& p_parameter) const
{
  return glGetAttribLocation(m_program, p_parameter.c_str());
}

unsigned int Shader::GetUniformLocation(std::string const& p_parameter) const
{
  return glGetUniformLocation(m_program, p_parameter.c_str());
}

Shader::UniformMap const& Shader::GetUniformMap() const
{
  return m_uniforms;
}

void Shader::SetPreprocessorConfig(std::string const& p_config)
{
  m_preprocessorConfig = p_config;
}

bool Shader::GetLinked()
{
  return m_linked;
}

void Shader::SetUniform(std::string const& pName, int pValue)
{
  unsigned int location = GetUniformLocation(pName);
  glUniform1i(location, pValue);
}

void Shader::SetUniform(std::string const& pName, unsigned int pValue)
{
  unsigned int location = GetUniformLocation(pName);
  glUniform1ui(location, pValue);
}

void Shader::SetUniform(std::string const& pName, float pValue)
{
  unsigned int location = GetUniformLocation(pName);
  glUniform1f(location, pValue);
}

void Shader::SetUniform(std::string const& pName, glm::vec2 const& pValue)
{
  unsigned int loc = GetUniformLocation(pName);
  glUniform2fv(loc, 1,  glm::value_ptr(pValue));
}

void Shader::SetUniform(std::string const& pName, glm::vec3 const& pValue)
{
  unsigned int loc = GetUniformLocation(pName);
  glUniform3fv(loc, 1,  glm::value_ptr(pValue));
}

void Shader::SetUniform(std::string const& pName, glm::vec4 const& pValue)
{
  unsigned int loc = GetUniformLocation(pName);
  glUniform4fv(loc, 1,  glm::value_ptr(pValue));
}

void Shader::SetUniform(std::string const& pName, glm::mat4 const& pValue)
{
  unsigned int loc = GetUniformLocation(pName);
  glUniformMatrix4fv(loc, 1, GL_FALSE,  glm::value_ptr(pValue));
}


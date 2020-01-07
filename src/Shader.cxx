#include "Shader.hxx"
#include "Common.hxx"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include <vector>

Shader::Shader():
m_vertexShader(-1),
m_fragmentShader(-1),
m_verterShaderSrc(""),
m_fragmentShaderSrc(""),
m_program(-1),
m_name(""),
m_preprocessorConfig("")
{
}

Shader::Shader(std::string const& p_name):
m_vertexShader(-1),
m_fragmentShader(-1),
m_verterShaderSrc(""),
m_fragmentShaderSrc(""),
m_program(-1),
m_name(p_name),
m_preprocessorConfig("")
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
    return -1;
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

  //Then retrieve the uniforms
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
    m_uniforms[name] = type;
  }
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

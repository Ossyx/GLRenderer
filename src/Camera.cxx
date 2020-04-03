#include "Camera.hxx"
#include "Common.hxx"

#include <iostream>
#include <math.h>

Camera::Camera():
m_position(glm::vec3(0.5f, 1.0f, 0.5f)),
m_direction(glm::vec3(0.0f, 0.0f, 0.0f)),
m_translationSpeed(0.0f),
m_translationAcceleration(0.0f),
m_azimuth(0),
m_elevation(0),
m_keyPressed(0)
{
  m_wireframe = false;

  m_treeRecompute = true;;
}

Camera::~Camera()
{
}

void Camera::Translate(glm::vec3 const& p_direction)
{
  m_position += p_direction;
}

void Camera::ChangeAzimuth(float p_delta)
{
}

void Camera::ChangeElevation(float p_delta)
{
}

glm::vec3 Camera::GetPosition() const
{
  return m_position;
}

float Camera::GetAzimuth() const
{
  return m_azimuth;
}

float Camera::GetElevation() const
{
  return m_elevation;
}

glm::mat4 Camera::ComputeViewMatrix() const
{
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::rotate(view, m_elevation, glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, m_azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::translate(view, m_position);
}

void Camera::SmoothMovement(float p_deltaT)
{
  ReadDirectionFromKeys();
  m_translationSpeed += p_deltaT * m_translationAcceleration;
  m_translationSpeed = std::max(0.0f, m_translationSpeed);
  m_translationSpeed = std::min(200.0f, m_translationSpeed);

  //to optimise
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::rotate(view, m_elevation, glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, m_azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::inverse(view);

  glm::vec4 dirH;
  dirH.x = m_direction.x;
  dirH.y = m_direction.y;
  dirH.z = m_direction.z;
  dirH.w = 0.0;

  dirH = view * dirH;
  glm::vec3 dir;
  dir.x = dirH.x;
  dir.y = dirH.y;
  dir.z = dirH.z;

  m_position = m_position + (m_translationSpeed * dir);
}

void Camera::MoveCamera(float p_deltaT, float p_speed)
{
  ReadDirectionFromKeys();
  m_translationSpeed = p_deltaT * p_speed;
  //m_translationSpeed = std::max(0.0f, m_translationSpeed);
  //m_translationSpeed = std::min(10.0f, m_translationSpeed);

  //to optimise
  glm::mat4 view = glm::mat4(1.0f);
  view = glm::rotate(view, m_elevation, glm::vec3(1.0f, 0.0f, 0.0f));
  view = glm::rotate(view, m_azimuth, glm::vec3(0.0f, 1.0f, 0.0f));
  view = glm::inverse(view);

  glm::vec4 dirH;
  dirH.x = m_direction.x;
  dirH.y = m_direction.y;
  dirH.z = m_direction.z;
  dirH.w = 0.0;

  dirH = view * dirH;
  glm::vec3 dir;
  dir.x = dirH.x;
  dir.y = dirH.y;
  dir.z = dirH.z;

  float dist = fabs(glm::dot(m_position, glm::vec3(0.0f,1.0f,0.0f)));
  float factor = log(1.0f + dist);
  factor = std::max(0.1f, factor);
  factor = std::min(1.0f, factor);

  m_position = m_position + (m_translationSpeed * factor * dir);
}

void Camera::ReadDirectionFromKeys()
{
  bool keyPressed = false;
  if (m_keyPressed & eKeyD)
  {
    m_direction += glm::vec3(1.0, 0.0, 0.0);
    keyPressed =  true;
  }
  if (m_keyPressed & eKeyA)
  {
    m_direction += glm::vec3(-1.0, 0.0, 0.0);
    keyPressed =  true;
  }
  if (m_keyPressed & eKeyW)
  {
    m_direction += glm::vec3(0.0, 0.0, -1.0);
    keyPressed =  true;
  }
  if (m_keyPressed & eKeyS)
  {
    m_direction += glm::vec3(0.0, 0.0, 1.0);
    keyPressed =  true;
  }
  if (m_keyPressed & eKeyQ)
  {
    m_direction += glm::vec3(0.0, 1.0, 0.0);
    keyPressed =  true;
  }
  if (m_keyPressed & eKeyZ)
  {
    m_direction += glm::vec3(0.0, -1.0, 0.0);
    keyPressed =  true;
  }

  if (keyPressed)
  {
    m_translationAcceleration = 10.0f;
    if (glm::length(m_direction) != 0)
    {
      m_direction = glm::normalize(m_direction);
    }
  }
  else
  {
    m_direction = glm::vec3(0.0f, 0.0f, 0.0f);
    m_translationAcceleration = -10.0;
  }
}

void Camera::HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto MapKey = [this](int p_glfwKeyFlag, KeyFlag p_keyFlag, int p_key, int p_action)
  {
    if (p_key == p_glfwKeyFlag)
    {
      if (p_action == GLFW_PRESS)
      {
        m_keyPressed |= p_keyFlag;
      }
      else if (p_action == GLFW_RELEASE)
      {
        m_keyPressed &= ~p_keyFlag;
      }
    }
  };

  MapKey(GLFW_KEY_A, eKeyA, key, action);
  MapKey(GLFW_KEY_S, eKeyS, key, action);
  MapKey(GLFW_KEY_W, eKeyW, key, action);
  MapKey(GLFW_KEY_D, eKeyD, key, action);
  MapKey(GLFW_KEY_Q, eKeyQ, key, action);
  MapKey(GLFW_KEY_Z, eKeyZ, key, action);

  if(action == GLFW_PRESS && key == GLFW_KEY_J)
  {
    m_wireframe = !m_wireframe;
  }
  if(action == GLFW_PRESS && key == GLFW_KEY_K)
  {
    m_treeRecompute = !m_treeRecompute;
  }
}

void Camera::HandleCursorEvent(double p_xpos, double p_ypos, double p_deltaX, double p_deltaY)
{
  m_azimuth += (p_deltaX / 360.0) * 2.0 * M_PI;
  if (m_azimuth >= M_PI)
  {
    m_azimuth = -M_PI + (m_azimuth - M_PI);
  }
  if (m_azimuth <= -M_PI)
  {
    m_azimuth = M_PI + (m_azimuth + M_PI);
  }

  m_elevation += (p_deltaY / 360.0) * 2.0 * M_PI;
  float epsilon = M_PI/180.0;
  if (m_elevation > (M_PI/2.0 - epsilon))
  {
    m_elevation = M_PI/2.0 - epsilon;
  }
  if (m_elevation < (-M_PI/2.0 + epsilon))
  {
    m_elevation = (-M_PI/2.0 + epsilon);
  }
}

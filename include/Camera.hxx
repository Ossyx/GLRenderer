#ifndef CAMERA_HXX
#define CAMERA_HXX

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>

#include "EventInterface.hxx"

class Camera : public EventInterface
{
public:

  Camera();

  ~Camera();

  void Translate(glm::vec3 const& p_direction);
  void ChangeAzimuth(float p_delta);
  void ChangeElevation(float p_delta);

  glm::vec3 GetPosition() const;
  float GetAzimuth() const;
  float GetElevation() const;
  glm::mat4 ComputeViewMatrix() const;


  void SmoothMovement(float p_deltaT);

  void MoveCamera(float p_deltaT, float p_speed);

  virtual void HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

  virtual void HandleCursorEvent(double p_xpos, double p_ypos, double p_deltaX, double p_deltaY);


private:

  void ReadDirectionFromKeys();

  glm::vec3 m_position;

  glm::vec3 m_direction;

  //Speed in unit/s
  float m_translationSpeed;

  float m_translationAcceleration;

  float m_azimuth;

  float m_elevation;

  int m_keyPressed;
};

#endif

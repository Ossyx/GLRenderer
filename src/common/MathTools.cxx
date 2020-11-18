#include "MathTools.hxx"

glm::quat EulerAngleToQuaternion(float pRoll, float pPitch, float pYaw)
{
  //Convert euler angle to a quaternion
  glm::quat q;
  double cy = cos(pYaw * 0.5);
  double sy = sin(pYaw * 0.5);
  double cp = cos(pPitch * 0.5);
  double sp = sin(pPitch * 0.5);
  double cr = cos(pRoll * 0.5);
  double sr = sin(pRoll * 0.5);
  
  q.w = cr * cp * cy + sr * sp * sy;
  q.x = sr * cp * cy - cr * sp * sy;
  q.y = cr * sp * cy + sr * cp * sy;
  q.z = cr * cp * sy - sr * sp * cy;
  return q;
}

glm::vec3 QuaternionToEulerAngler(glm::quat const& pQuat)
{
  double sinr_cosp = 2 * (pQuat.w * pQuat.x + pQuat.y * pQuat.z);
  double cosr_cosp = 1 - 2 * (pQuat.x * pQuat.x + pQuat.y * pQuat.y);
  float roll = std::atan2(sinr_cosp, cosr_cosp);

  // pitch (y-axis rotation)
  double sinp = 2 * (pQuat.w * pQuat.y - pQuat.z * pQuat.x);
  float pitch = 0.0f;
  if (std::abs(sinp) >= 1)
      pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
  else
      pitch = std::asin(sinp);

  // yaw (z-axis rotation)
  double siny_cosp = 2 * (pQuat.w * pQuat.z + pQuat.x * pQuat.y);
  double cosy_cosp = 1 - 2 * (pQuat.y * pQuat.y + pQuat.z * pQuat.z);
  float yaw = std::atan2(siny_cosp, cosy_cosp);

  return glm::vec3(roll, pitch, yaw);
}

#include "MathTools.hxx"
#include <iostream>

int main()
{
  float roll = M_PI / 2.0f;
  float pitch = M_PI / 6.0f;
  float yaw = M_PI * 1.5f;
  
  std::cout << "(roll, pitch, yaw) = " << 
    "(" << roll << "," << pitch << ","
    << yaw << ")" << std::endl;
    
  glm::quat q = EulerAngleToQuaternion(roll, pitch, yaw);
  
  std::cout << "quat = " << "(" << q.w << "," << q.x << ","
    << q.y << "," << q.z << ")" << std::endl;
    
  glm::vec3 angles = QuaternionToEulerAngler(q);
  
  std::cout << "Converted (roll, pitch, yaw) = " << 
    "(" << angles.x << "," << angles.y << ","
    << angles.z << ")" << std::endl;
    
  return 0;
}

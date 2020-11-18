#ifndef MATHTOOLS_HXX
#define MATHTOOLS_HXX

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

glm::quat EulerAngleToQuaternion(float pRoll, float pPitch, float pYaw);
glm::vec3 QuaternionToEulerAngler(glm::quat const& pQuat);

#endif

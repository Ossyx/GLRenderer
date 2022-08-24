#ifndef MATHTOOLS_HXX
#define MATHTOOLS_HXX

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "GLRendererExport.hxx"
#define M_PI 3.14

GLRenderer_EXPORT glm::quat EulerAngleToQuaternion(float pRoll, float pPitch, float pYaw);
GLRenderer_EXPORT glm::vec3 QuaternionToEulerAngler(glm::quat const& pQuat);

#endif

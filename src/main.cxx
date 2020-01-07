#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include "ModelLoader.hxx"
#include "DrawableItem.hxx"
#include "Shader.hxx"
#include "Camera.hxx"
#include "EventDispatcher.hxx"
#include "SceneRenderer.hxx"

void error_callback(int error, const char* description);

int main()
{
  rxLogDebug(" Hello Space War 2 !");

  if (glfwInit() == false)
  {
    rxLogError("Error : GLFW fails to initialize.");
    return 1;
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  GLFWwindow* window = glfwCreateWindow(1920, 1080, "SpaceWar2 !", NULL, NULL);

  if (window == NULL)
  {
    rxLogDebug("Error : GLFW Windows fails to create.");
    return 1;
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  EventDispatcher* dispatcher = EventDispatcher::Get();
  glfwSetKeyCallback(window, EventDispatcher::HandleKeyEvent);
  glfwSetCursorPosCallback(window, EventDispatcher::HandleCursorEvent);

  SceneRenderer renderer(window);
  //renderer.AddModel();
  renderer.AddTerrain();

  int maxSize;
  glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxSize);
  rxLogInfo("GL_MAX_UNIFORM_BLOCK_SIZE : "<<maxSize);

  int maxtessgenlvl;
  glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxtessgenlvl);
  rxLogInfo("GL_MAX_TESS_GEN_LEVEL : "<<maxtessgenlvl);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  auto frameTimer = std::chrono::steady_clock::now();
  while (glfwWindowShouldClose(window) == false)
  {
    auto newTime = std::chrono::steady_clock::now();
    float millis = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - frameTimer).count();
    rxLogInfo("FPS : " << 1000.0f / millis);
    renderer.UpdateCamera(millis);
    //renderer.RenderShadowMap(window);
    //renderer.Render(window);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    renderer.RenderTerrain(window);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    renderer.RenderGBufferDebug(window);
    frameTimer = newTime;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void error_callback(int error, const char* description)
{
  rxLogDebug("Error " << error << " : " <<description);
}

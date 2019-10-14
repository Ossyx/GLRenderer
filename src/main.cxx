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

static const float g_vertex_buffer_data[] = {
   0.0f,  0.5f,  0.0f,
   0.5f, -0.5f,  0.0f,
  -0.5f, -0.5f,  0.0f
};

static const unsigned int g_index_buffer[] = {0, 1, 2};

void error_callback(int error, const char* description);

int main()
{
  std::cout<<" Hello Space War 2 !"<<std::endl;

  if(!glfwInit())
  {
    std::cout<<"Error : GLFW fails to initialize."<<std::endl;
    return 1;
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  GLFWwindow* window = glfwCreateWindow(1920, 1080, "SpaceWar2 !", NULL, NULL);

  if(!window)
  {
    std::cout<<"Error : GLFW Windows fails to create."<<std::endl;
    return 1;
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  EventDispatcher* dispatcher = EventDispatcher::Get();
  glfwSetKeyCallback(window, EventDispatcher::HandleKeyEvent);
  glfwSetCursorPosCallback(window, EventDispatcher::HandleCursorEvent);

  SceneRenderer renderer(window);
  renderer.AddModel();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window))
  {
    renderer.Render(window);
    renderer.RenderGBufferDebug(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void error_callback(int error, const char* description)
{
  std::cout<<"Error "<<error<<" : "<<description<<std::endl;
}

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "EventDispatcher.hxx"
#include "SceneRenderer.hxx"
#include "SimpleRenderer.hxx"
#include "ResourcesLoader.hxx"
#include "SceneGraph.hxx"
#include "SceneGraphLoader.hxx"

void error_callback(int error, const char* description);

int main()
{
  rxLogDebug(" Simple renderer test ");

  if (glfwInit() == false)
  {
    rxLogError("Error : GLFW fails to initialize.");
    return 1;
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  GLFWwindow* window = glfwCreateWindow(1920, 1080, "SimpleRendererTest !", NULL, NULL);

  if (window == NULL)
  {
    rxLogDebug("Error : GLFW Windows fails to create.");
    return 1;
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  //Init ImGui
  EventDispatcher* dispatcher = EventDispatcher::Get();
  glfwSetKeyCallback(window, EventDispatcher::HandleKeyEvent);
  glfwSetCursorPosCallback(window, EventDispatcher::HandleCursorEvent);
  
  //Load resources
  rx::ResourcesHolderPtr holder = std::make_shared<rx::ResourcesHolder>();
  rx::ResourcesLoader loader;
  loader.LoadDescription("/home/bertrand/Work/GLRenderer/test/data/resources_scenegraphtest.json", *holder);
  loader.LoadResources(*holder);
  
  //Load scene graph
  rx::SceneGraphLoader graphLoder;
  rx::SceneGraphPtr graph = std::make_shared<rx::SceneGraph>();
  
  graphLoder.Load("/home/bertrand/Work/GLRenderer/test/data/scenegraph.json", *graph, *holder);

  SimpleRenderer renderer;
  renderer.Init(graph, holder);
  
  while (glfwWindowShouldClose(window) == false)
  {
    renderer.Render(window);
  }
  return 0;
}

void error_callback(int error, const char* description)
{
  rxLogDebug("Error " << error << " : " <<description);
}

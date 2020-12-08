#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "EventDispatcher.hxx"
#include "SimpleRenderer.hxx"
#include "ResourcesLoader.hxx"
#include "SceneGraph.hxx"
#include "SceneGraphLoader.hxx"

void error_callback(int error, const char* description);

int main(int argc, char** argv)
{
  std::filesystem::path resourcePath;
  std::filesystem::path sceneGraphPath;
  
  if(argc >= 3)
  {
    resourcePath = argv[1];
    sceneGraphPath = argv[2];
  }
  else
  {
    rxLogError("No resource and scengraph provided. Exiting.");
    return 0;
  }
  
  rxLogDebug(" Simple renderer test ");
  rxLogInfo("Using resource "<< resourcePath);
  rxLogInfo("Using scene graph "<< sceneGraphPath);

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
  loader.LoadDescription(resourcePath, *holder);
  loader.LoadResources(*holder);
  
  while(loader.GetStatus() != rx::ResourcesLoader::Loaded);
  
  //Load scene graph
  rx::SceneGraphLoader graphLoader;
  rx::SceneGraphPtr graph = std::make_shared<rx::SceneGraph>();
  
  graphLoader.Load(sceneGraphPath, *graph, *holder);

  SimpleRenderer renderer;
  renderer.Initialize(graph, holder);
  
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

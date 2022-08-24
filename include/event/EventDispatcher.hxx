#ifndef EVENTDISPATCHER_HXX
#define EVENTDISPATCHER_HXX

#include <unordered_map>

#include "EventInterface.hxx"
#include <GLFW/glfw3.h>
#include <string>

class __declspec(dllexport) EventDispatcher
{
public:
  static void HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

  static void HandleCursorEvent(GLFWwindow* window, double xpos, double ypos);

  ~EventDispatcher();

  static EventDispatcher* Get();

  void AddEventListener(std::string p_key, EventInterface* p_listener);

  void RemoveEventListener(std::string p_key);
private:

  EventDispatcher();

  static EventDispatcher* m_singleton;

  typedef std::unordered_map<std::string, EventInterface*> EventInterfaceMap;
  static EventInterfaceMap m_registeredInterface;

  static double m_cursorX;

  static double m_cursorY;

};

#endif

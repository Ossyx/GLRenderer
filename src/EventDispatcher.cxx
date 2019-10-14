#include "EventDispatcher.hxx"
#include <iostream>

EventDispatcher* EventDispatcher::m_singleton = nullptr;

EventDispatcher::EventInterfaceMap EventDispatcher::m_registeredInterface
  = EventDispatcher::EventInterfaceMap();

double EventDispatcher::m_cursorX = 0.0;
double EventDispatcher::m_cursorY = 0.0;

EventDispatcher::EventDispatcher()
{
}

EventDispatcher::~EventDispatcher()
{
}

void EventDispatcher::HandleKeyEvent(GLFWwindow* window, int key,
  int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  EventInterfaceMap::iterator it = m_registeredInterface.begin();
  for (; it != m_registeredInterface.end(); ++it)
  {
    EventInterface* listener = it->second;
    listener->HandleKeyEvent(window, key, scancode, action, mods);
  }
}

void EventDispatcher::HandleCursorEvent(GLFWwindow* window, double xpos, double ypos)
{
  EventInterfaceMap::iterator it = m_registeredInterface.begin();
  for (; it != m_registeredInterface.end(); ++it)
  {
    EventInterface* listener = it->second;
    listener->HandleCursorEvent(xpos, ypos, xpos - m_cursorX, ypos - m_cursorY);
    m_cursorX = xpos;
    m_cursorY = ypos;
  }
}

EventDispatcher* EventDispatcher::Get()
{
  if (m_singleton == false)
  {
    m_singleton = new EventDispatcher();
  }
  return m_singleton;
}

void EventDispatcher::AddEventListener(std::string p_key, EventInterface* p_listener)
{
  m_registeredInterface[p_key] = p_listener;
}

EventInterface* EventDispatcher::RemoveEventListener(std::string p_key)
{
  EventInterfaceMap::iterator it = m_registeredInterface.find(p_key);
  if (it != m_registeredInterface.end())
  {
    m_registeredInterface.erase(it);
  }
}

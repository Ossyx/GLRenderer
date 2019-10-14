#ifndef EVENTINTERFACE_HXX
#define EVENTINTERFACE_HXX

#include <GLFW/glfw3.h>

enum  KeyFlag {
  eKeyA = 0x1,  
  eKeyZ = 0x2,
  eKeyE = 0x4,
  eKeyQ = 0x8,
  eKeyS = 0x10,
  eKeyD = 0x20,
  eKeyW = 0x40,
  eKeyX = 0x80,
  eKeyC = 0x100
};

class EventInterface
{
public:
  EventInterface();
  
  ~EventInterface();
    
  virtual void HandleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
  
  virtual void HandleCursorEvent(double p_xpos, double p_ypos, double p_deltaX, double p_deltaY) = 0;
  
};
#endif

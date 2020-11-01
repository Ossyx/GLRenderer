#ifndef TERRAINGEN_HXX
#define TERRAINGEN_HXX

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct RenderingDebugInfo
{
  float m_fps;
  unsigned int m_nbLeafTotal;
  unsigned int m_nbShowedLeaf;
  unsigned int m_nbCulledLeaf;
  float m_surfaceDistance;
  float m_horizonThreshold;
  float m_near;
  float m_far;
  bool m_culling;
  bool m_renderWater;
  float m_coastKFactor;

  int m_noiseLOctave;
  int m_noiseMOctave;
  int m_noiseHOctave;

  static RenderingDebugInfo* m_instance;
  static RenderingDebugInfo* Get()
  {
    if(m_instance == nullptr)
    {
      m_instance = new RenderingDebugInfo();
    }
    return m_instance;
  }
};

class TerrainGenGui
{
public:
  TerrainGenGui(GLFWwindow* p_window);

  ~TerrainGenGui();

  void InitGuiFrame();

  void PrepareGui();

  void RenderGui();

  bool showParameters;
};

#endif

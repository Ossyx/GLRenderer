#include "TerrainGenGui.hxx"

RenderingDebugInfo* RenderingDebugInfo::m_instance = nullptr;

TerrainGenGui::TerrainGenGui(GLFWwindow* p_window)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(p_window, true);
  ImGui_ImplOpenGL3_Init();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  RenderingDebugInfo* debugInfo = RenderingDebugInfo::Get();

  debugInfo->m_fps = 0.0f;
  debugInfo->m_nbLeafTotal = 0;
  debugInfo->m_nbShowedLeaf = 0;
  debugInfo->m_nbCulledLeaf = 0;
  debugInfo->m_surfaceDistance = 0.0f;
  debugInfo->m_horizonThreshold = 0.0f;
  debugInfo->m_culling = true;
  debugInfo->m_renderWater = true;
  debugInfo->m_coastKFactor = 1000000.0f;

  debugInfo->m_noiseLOctave = 4;
  debugInfo->m_noiseMOctave = 16;
  debugInfo->m_noiseHOctave = 16;

  showParameters = false;
}

TerrainGenGui::~TerrainGenGui()
{
}

void TerrainGenGui::InitGuiFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void TerrainGenGui::PrepareGui()
{
  // render your GUI
  //ImGui::ShowDemoWindow();
  ImGuiWindowFlags window_flags = 0;

  window_flags |= ImGuiWindowFlags_NoTitleBar;
  window_flags |= ImGuiWindowFlags_NoScrollbar;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;
  window_flags |= ImGuiWindowFlags_NoCollapse;
  window_flags |= ImGuiWindowFlags_NoNav;
  window_flags |= ImGuiWindowFlags_NoBackground;

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

  if (!ImGui::Begin("Dear ImGui Demo", NULL, window_flags))
  {
      ImGui::End();
      return;
  }

  RenderingDebugInfo* debugInfo = RenderingDebugInfo::Get();

  // Early out if the window is collapsed, as an optimization.
  ImGui::Text("FPS  %f", debugInfo->m_fps);
  ImGui::Text("Surface distance  %f", debugInfo->m_surfaceDistance);
  ImGui::Text("Horizon threshold  %f", debugInfo->m_horizonThreshold);
  ImGui::Text("Leaves  %i", debugInfo->m_nbLeafTotal);
  ImGui::Text("Showed leaves  %i", debugInfo->m_nbShowedLeaf);
  ImGui::Text("Culled leaves  %i", debugInfo->m_nbCulledLeaf);
  ImGui::Text("Near  %f", debugInfo->m_near);
  ImGui::Text("Far  %f", debugInfo->m_far);



  ImGui::Spacing();
  ImGui::End();

  if(showParameters)
  {
    if (!ImGui::Begin("Parameters", NULL, 0))
    {
        ImGui::End();
        return;
    }
    ImGui::Checkbox("Culling ", &debugInfo->m_culling);
    ImGui::Checkbox("RenderWater ", &debugInfo->m_renderWater);
    ImGui::InputFloat("Coast K Factor", &debugInfo->m_coastKFactor, 100.0f, 1000.0f, "%.3f");
    ImGui::InputInt("NoiseL octave", &debugInfo->m_noiseLOctave);
    ImGui::InputInt("NoiseM octave", &debugInfo->m_noiseMOctave);
    ImGui::InputInt("NoiseH octave", &debugInfo->m_noiseHOctave);
    ImGui::End();
  }
}

void TerrainGenGui::RenderGui()
{
  PrepareGui();
  // Render dear imgui into screen
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

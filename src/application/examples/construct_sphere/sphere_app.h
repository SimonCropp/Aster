#include "../app_base.h"

#include <engine/utils/center_pointed_camera.h>

class SphereConstructDemo final : public ApplicationBase
{
public:
  SphereConstructDemo(GLFWwindow* wnd, RHI::Vulkan::Core* vkCore, int wndWidth, int wndHeight);

  void ProcessKeyInput(int scancode, int action, int mods);
  void ProcessMouseInput(double xpos, double ypos);
  void ProcessMouseButtonInput(int button, int action, int mods);
  void ProcessScrollInput(double xoffset, double yoffset);

protected:
  virtual void Dt(float dt) override;

private:
  void Render();
  void RebuildMesh();

private:
  bool m_RotateScene;
  
  Utils::CenterPointedCamera m_Camera;

  struct {
    double xPos = 0.0f, yPos = 0.0f;
  } m_LastMouseInput;

  float m_SphereRadius;
  int m_Segments;
  bool m_CullNone;

  struct Mesh
  {
    RHI::Vulkan::Buffer vertexBuffer;
    RHI::Vulkan::Buffer indexBuffer;
    uint32_t indexCount = 0;
  } m_Mesh;

  std::unique_ptr<RHI::Vulkan::ShaderProgram> m_LineShader;
};
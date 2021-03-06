#include "sphere_app.h"
#include <engine/rendering/vulkan/core.h>
#include <engine/rendering/vulkan/fileutils.h>
#include <engine/utils/mesh_generation.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace
{
  void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    auto* handler = reinterpret_cast<SphereConstructDemo*>(glfwGetWindowUserPointer(window));
    handler->ProcessKeyInput(scancode, action, mods);
  }

  void GLFW_CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
  {
    auto* handler = reinterpret_cast<SphereConstructDemo*>(glfwGetWindowUserPointer(window));
    handler->ProcessMouseInput(xpos, ypos);
  }

  void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
  {
    auto* handler = reinterpret_cast<SphereConstructDemo*>(glfwGetWindowUserPointer(window));
    handler->ProcessMouseButtonInput(button, action, mods);
  }

  void GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    auto* handler = reinterpret_cast<SphereConstructDemo*>(glfwGetWindowUserPointer(window));
    handler->ProcessScrollInput(xoffset, yoffset);
  }

  RHI::Vulkan::VertexInputDeclaration GetLinesVID()
  {
    RHI::Vulkan::VertexInputDeclaration vid;

    vid.AddBindingDescription(0, sizeof(glm::vec3));
    vid.AddAttributeDescription(vk::Format::eR32G32B32Sfloat, 0, 0, 0);

    return vid;
  }
}

SphereConstructDemo::SphereConstructDemo(GLFWwindow* wnd, RHI::Vulkan::Core* vkCore, int wndWidth, int wndHeight)
  : ApplicationBase(wnd, vkCore, wndWidth, wndHeight)
  , m_Camera(5.0f, -90.0f, 120.0f, { wndWidth, wndHeight })
  , m_RotateScene(false)
  , m_SphereRadius(1.0f)
  , m_Segments(0)
  , m_CullNone(false)
{
  glfwSetWindowUserPointer(wnd, this);
  glfwSetKeyCallback(wnd, GLFW_KeyCallback);
  glfwSetCursorPosCallback(wnd, GLFW_CursorPositionCallback);
  glfwSetMouseButtonCallback(wnd, GLFW_MouseButtonCallback);
  glfwSetScrollCallback(wnd, GLFW_ScrollCallback);

  {
    auto vertex = m_VkCore->CreateShader(RHI::Vulkan::ReadFile("../data/shaders/spirv/examples/lines.vert.spv"));
    auto fragment = m_VkCore->CreateShader(RHI::Vulkan::ReadFile("../data/shaders/spirv/examples/lines.frag.spv"));
    m_LineShader = std::make_unique<RHI::Vulkan::ShaderProgram>(*m_VkCore, std::move(vertex), std::move(fragment));
  }
}

void SphereConstructDemo::ProcessKeyInput(int scancode, int action, int mods)
{

}

void SphereConstructDemo::ProcessMouseInput(double xpos, double ypos)
{
  static bool isInitialized = false;
  if (!isInitialized)
  {
    //to prevent `wtf` rotation after a first input, since lastX,lastY = 0.0f at the start.
    m_LastMouseInput.xPos = xpos;
    m_LastMouseInput.yPos = ypos;
    isInitialized = true;
    return;
  }

  const double dx = xpos - m_LastMouseInput.xPos;
  const double dy = ypos - m_LastMouseInput.yPos;

  m_LastMouseInput.xPos = xpos;
  m_LastMouseInput.yPos = ypos;

  if (m_RotateScene)
    m_Camera.Rotate(dx, dy);
}

void SphereConstructDemo::ProcessMouseButtonInput(int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_MIDDLE)
  {
    if (action == GLFW_PRESS && !m_RotateScene)
      m_RotateScene = true;
    else if (action == GLFW_RELEASE && m_RotateScene)
      m_RotateScene = false;
  }
}

void SphereConstructDemo::ProcessScrollInput(double xoffset, double yoffset)
{
  m_Camera.AddRadius(-yoffset * 0.3f);
}

void SphereConstructDemo::Dt(float dt)
{
  RHI::Vulkan::RenderGraph* rg =  m_VkCore->BeginFrame();

  rg->AddRenderSubpass()
    .AddExistOutputColorAttachment(BACKBUFFER_RESOURCE_ID)
    .SetRenderCallback([&](RHI::Vulkan::FrameContext& ctx) 
    {
        if (m_Mesh.indexCount == 0)
          return;

        const RHI::Vulkan::RasterizationMode rasterMode = m_CullNone ? RHI::Vulkan::WireframeNoCullMode : RHI::Vulkan::WireframeMode;
        RHI::Vulkan::Pipeline* pipeline = ctx.GetPipeline(*m_LineShader, GetLinesVID(), vk::PrimitiveTopology::eTriangleList, RHI::Vulkan::DisableDepthTest, rasterMode);
        ctx.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetPipeline());

        struct
        {
          glm::mat4 Projection;
          glm::mat4 View;
        } camera {
            m_Camera.GetProjection(),
            m_Camera.GetView()
        };

        RHI::Vulkan::UniformsAccessor* uniforms = ctx.GetUniformsAccessor(*m_LineShader);
        uniforms->SetUniformBuffer("Camera", &camera);
        std::vector<vk::DescriptorSet> descriptorSets = uniforms->GetUpdatedDescriptorSets();

        vk::DeviceSize offset = 0;
        ctx.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->GetLayout(), 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
        ctx.commandBuffer.bindVertexBuffers(0, 1, &m_Mesh.vertexBuffer.GetBuffer(), &offset);
        ctx.commandBuffer.bindIndexBuffer(m_Mesh.indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);

        ctx.commandBuffer.drawIndexed(m_Mesh.indexCount, 1, 0, 0, 0);
    });

  const auto lastSegments = m_Segments;
  const auto lastRadius = m_SphereRadius;

  m_GUI.AddGUIRenderingSubpass(rg, false, [&]()
  {
    ImGui::Begin("Settings");
    ImGui::SliderInt("segments", &m_Segments, 0, 7, "%d");
    ImGui::SliderFloat("sphere's radius", &m_SphereRadius, 1.0f, 10.0f, "%.3f");
    ImGui::Checkbox("cull none", &m_CullNone);
    ImGui::End();
  });

  m_VkCore->EndFrame();

  const bool needToRebuildMesh = (lastSegments != m_Segments) || (lastRadius != m_SphereRadius);
  if (needToRebuildMesh)
    RebuildMesh();
}


void SphereConstructDemo::RebuildMesh()
{
  auto [vertices, indices] = Utils::GenerateSphere(m_Segments, m_SphereRadius);

  m_Mesh.vertexBuffer = m_VkCore->AllocateDeviceBuffer(vertices.data(), vertices.size() * sizeof(vertices[0]), vk::BufferUsageFlagBits::eVertexBuffer);
  m_Mesh.indexBuffer = m_VkCore->AllocateDeviceBuffer(indices.data(), indices.size() * sizeof(indices[0]), vk::BufferUsageFlagBits::eIndexBuffer);
  m_Mesh.indexCount = indices.size();
}


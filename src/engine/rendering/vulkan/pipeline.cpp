#include "pipeline.h"
#include "vertex_input_declaration.h"

Pipeline::Pipeline(vk::Device logicalDevice,
  const ShaderProgram& program,
  const VertexInputDeclaration& vertexInputDeclaration,
  const std::vector<vk::DescriptorSetLayout> layouts,
  const vk::PrimitiveTopology topology,
  const DepthStencilSettings& depthStencilSettings,
  const vk::Extent2D viewportExtent,
  const vk::RenderPass renderpass,
  const uint32_t subpass)
{
  const auto vertexStageCreateInfo = vk::PipelineShaderStageCreateInfo()
    .setStage(vk::ShaderStageFlagBits::eVertex)
    .setModule(program.GetVertexShader().GetModule())
    .setPName("main");

  const auto fragmentStageCreateInfo = vk::PipelineShaderStageCreateInfo()
    .setStage(vk::ShaderStageFlagBits::eFragment)
    .setModule(program.GetFragmentShader().GetModule())
    .setPName("main");

  vk::PipelineShaderStageCreateInfo stageCreateInfos[]{
    fragmentStageCreateInfo,
    vertexStageCreateInfo
  };

  const auto vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo()
    .setPVertexAttributeDescriptions(vertexInputDeclaration.GetAttributeDescriptions().data())
    .setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexInputDeclaration.GetAttributeDescriptions().size()))
    .setPVertexBindingDescriptions(vertexInputDeclaration.GetBindingDescriptions().data())
    .setVertexBindingDescriptionCount(static_cast<uint32_t>(vertexInputDeclaration.GetBindingDescriptions().size()));

  const auto inputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo()
    .setTopology(topology)
    .setPrimitiveRestartEnable(false);

  const vk::Viewport viewports[]{
    vk::Viewport()
    .setHeight((float)viewportExtent.height)
    .setWidth((float)viewportExtent.width)
    .setX(0.0f)
    .setY(0.0f)
    .setMinDepth(0.0f)
    .setMaxDepth(1.0f)
  };

  const vk::Rect2D scissors[]{
    vk::Rect2D()
    .setExtent(viewportExtent)
    .setOffset({0, 0})
  };

  const auto viewportState = vk::PipelineViewportStateCreateInfo()
    .setPScissors(scissors)
    .setScissorCount(1)
    .setPViewports(viewports)
    .setViewportCount(1);

  const auto rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
    .setDepthClampEnable(false)
    .setRasterizerDiscardEnable(false)
    .setPolygonMode(vk::PolygonMode::eFill)
    .setCullMode(vk::CullModeFlagBits::eBack)
    .setFrontFace(vk::FrontFace::eClockwise)
    .setDepthBiasEnable(false)
    .setLineWidth(1.0f);

  const auto multisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
    .setSampleShadingEnable(false)
    .setRasterizationSamples(vk::SampleCountFlagBits::e1);

  const auto colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState()
    .setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB)
    .setBlendEnable(false);

  const auto colorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo()
    .setLogicOpEnable(false)
    .setAttachmentCount(1)
    .setPAttachments(&colorBlendAttachmentState);

  const auto depthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo()
    .setDepthTestEnable(depthStencilSettings.depthTestEnabled)
    .setDepthWriteEnable(depthStencilSettings.depthWriteEnabled)
    .setDepthCompareOp(vk::CompareOp::eLess)
    .setDepthBoundsTestEnable(false)
    .setStencilTestEnable(false);

  const auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
    .setSetLayoutCount(layouts.size())
    .setPSetLayouts(layouts.data());

  layout = logicalDevice.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

  const auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
    .setStageCount(2)
    .setPStages(stageCreateInfos)
    .setPVertexInputState(&vertexInputStateCreateInfo)
    .setPInputAssemblyState(&inputAssemblyStateCreateInfo)
    .setPTessellationState(nullptr)
    .setPViewportState(&viewportState)
    .setPRasterizationState(&rasterizationStateCreateInfo)
    .setPMultisampleState(&multisampleStateCreateInfo)
    .setPDepthStencilState(&depthStencilStateCreateInfo)
    .setPColorBlendState(&colorBlendStateCreateInfo)
    .setPDynamicState(nullptr)
    .setLayout(layout.get())
    .setRenderPass(renderpass)
    .setSubpass(subpass);

  pipeline = logicalDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
}

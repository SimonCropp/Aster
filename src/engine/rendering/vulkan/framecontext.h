#pragma once

#include "common.h"

#include <vulkan/vulkan.hpp>

class RenderGraph;
class UniformsAccessor;
class UniformsAccessorStorage;
class PipelineStorage;
class ImageView;
class Pipeline;
class ShaderProgram;
class VertexInputDeclaration;

struct FrameContext
{
  friend class RenderGraph;

  const ImageView& GetImageView(const ResourceId& id) const;
  Pipeline* GetPipeline(const ShaderProgram& program, const VertexInputDeclaration& vertexInputDeclaration, vk::PrimitiveTopology topology, const DepthStencilSettings& depthStencilSettings);
  UniformsAccessor* GetUniformsAccessor(const ShaderProgram& program);

  vk::Extent2D BackbufferSize;
  UniformsAccessorStorage* uniformsAccessorStorage;
  PipelineStorage* pipelineStorage;
  vk::RenderPass renderPass;
  vk::Framebuffer framebuffer;
  vk::CommandBuffer commandBuffer;
  uint32_t subpassNumber;
  uint16_t outputColorAttachmentsNumber;

private:
  RenderGraph* renderGraph;
};
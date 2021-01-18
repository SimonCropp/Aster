#include "framecontext.h"

#include "common.h"
#include "framegraph.h"
#include "shader.h"
#include "vertex_input_declaration.h"
#include "pipeline_storage.h"
#include "uniforms_accessor_storage.h"

namespace RHI::Vulkan
{
  const ImageView& FrameContext::GetImageView(const ResourceId& id) const
  {
    return renderGraph->GetImageView(id);
  }

  Pipeline* FrameContext::GetPipeline(const ShaderProgram& program, const VertexInputDeclaration& vertexInputDeclaration, vk::PrimitiveTopology topology, const DepthStencilSettings& depthStencilSettings)
  {
    return pipelineStorage->GetPipeline(program, vertexInputDeclaration, topology, depthStencilSettings, *this);
  }

  UniformsAccessor* FrameContext::GetUniformsAccessor(const ShaderProgram& program)
  {
    return uniformsAccessorStorage->GetUniformsAccessor(program);
  }
}
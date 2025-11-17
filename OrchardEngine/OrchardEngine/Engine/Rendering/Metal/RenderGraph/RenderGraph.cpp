#include "RenderGraph.hpp"
#include "../MetalContext.hpp"
#include <iostream>

namespace Orchard {

RenderGraph::RenderGraph(MetalContext* context) : m_Context(context) {
}

RenderGraph::~RenderGraph() {
}

void RenderGraph::AddPass(const std::string& name, std::unique_ptr<RenderPass> pass) {
    m_Passes.push_back({name, std::move(pass)});
    m_Compiled = false;
}

void RenderGraph::AddResource(const std::string& name, RenderResource resource) {
    m_Resources[name] = resource;
}

RenderResource* RenderGraph::GetResource(const std::string& name) {
    auto it = m_Resources.find(name);
    if (it != m_Resources.end()) {
        return &it->second;
    }
    return nullptr;
}

void RenderGraph::Compile() {
    BuildExecutionOrder();
    
    for (auto& [name, pass] : m_Passes) {
        pass->Setup();
    }
    
    m_Compiled = true;
    std::cout << "Render graph compiled with " << m_Passes.size() << " passes" << std::endl;
}

void RenderGraph::Execute() {
    if (!m_Compiled) {
        Compile();
    }
    
    for (size_t idx : m_ExecutionOrder) {
        if (idx < m_Passes.size()) {
            m_Passes[idx].second->Execute(m_Context);
        }
    }
}

void RenderGraph::Reset() {
    m_Passes.clear();
    m_Resources.clear();
    m_ExecutionOrder.clear();
    m_Compiled = false;
}

void RenderGraph::BuildExecutionOrder() {
    m_ExecutionOrder.clear();
    for (size_t i = 0; i < m_Passes.size(); ++i) {
        m_ExecutionOrder.push_back(i);
    }
}

void GeometryPass::Execute(MetalContext* context) {
#ifdef __APPLE__
    if (!context) return;
    
    auto encoder = context->BeginRenderPass(context->GetMetalLayer().renderPassDescriptor);
    if (encoder) {
        [encoder setViewport:(MTLViewport){0.0, 0.0,
            static_cast<double>(context->GetMetalLayer().drawableSize.width),
            static_cast<double>(context->GetMetalLayer().drawableSize.height),
            0.0, 1.0}];
        
        context->EndRenderPass();
    }
#endif
}

void LightingPass::Execute(MetalContext* context) {
}

void PostProcessPass::Execute(MetalContext* context) {
}

void ShadowPass::Execute(MetalContext* context) {
}

}

#include "Renderer.hpp"
#include "Metal/MetalContext.hpp"
#include "Metal/RenderGraph/RenderGraph.hpp"
#include <iostream>

namespace Orchard {

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

bool Renderer::Initialize(const std::string& appName, uint32_t width, uint32_t height) {
    m_Width = width;
    m_Height = height;
    
    m_Context = std::make_unique<MetalContext>();
    if (!m_Context->Initialize(appName, width, height)) {
        std::cerr << "Failed to initialize Metal context" << std::endl;
        return false;
    }
    
    m_RenderGraph = std::make_unique<RenderGraph>(m_Context.get());
    
    std::cout << "Renderer initialized: " << width << "x" << height << std::endl;
    return true;
}

void Renderer::Shutdown() {
    m_RenderGraph.reset();
    m_Context->Shutdown();
    m_Context.reset();
    
    std::cout << "Renderer shut down" << std::endl;
}

void Renderer::BeginFrame() {
    if (m_Context) {
        m_Context->BeginFrame();
    }
}

void Renderer::EndFrame() {
    if (m_Context) {
        m_Context->EndFrame();
        m_Context->Present();
    }
    m_FrameIndex++;
}

void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    m_Width = width;
    m_Height = height;
}

void Renderer::SetClearColor(float r, float g, float b, float a) {
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}

void Renderer::RenderScene(Scene* scene) {
    if (m_RenderGraph) {
        m_RenderGraph->Execute();
    }
}

void Renderer::Resize(uint32_t width, uint32_t height) {
    m_Width = width;
    m_Height = height;
}

}

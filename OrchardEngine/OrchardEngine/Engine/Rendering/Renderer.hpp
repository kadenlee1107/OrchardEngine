#pragma once

#include <string>
#include <memory>
#include <vector>

namespace Orchard {

class RenderGraph;
class MetalContext;
class Scene;
struct RenderPass;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool Initialize(const std::string& appName, uint32_t width, uint32_t height);
    void Shutdown();
    
    void BeginFrame();
    void EndFrame();
    
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void SetClearColor(float r, float g, float b, float a);
    
    void RenderScene(Scene* scene);
    
    MetalContext* GetContext() const { return m_Context.get(); }
    RenderGraph* GetRenderGraph() const { return m_RenderGraph.get(); }
    
    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    
    void Resize(uint32_t width, uint32_t height);
    
private:
    std::unique_ptr<MetalContext> m_Context;
    std::unique_ptr<RenderGraph> m_RenderGraph;
    
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint64_t m_FrameIndex = 0;
    
    float m_ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
};

}

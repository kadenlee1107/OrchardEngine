#pragma once

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

#include <memory>
#include <string>

namespace Orchard {

class MetalContext {
public:
    MetalContext();
    ~MetalContext();
    
    bool Initialize(const std::string& appName, uint32_t width, uint32_t height);
    void Shutdown();
    
#ifdef __APPLE__
    id<MTLDevice> GetDevice() const { return m_Device; }
    id<MTLCommandQueue> GetCommandQueue() const { return m_CommandQueue; }
    id<MTLCommandBuffer> GetCurrentCommandBuffer() const { return m_CurrentCommandBuffer; }
    CAMetalLayer* GetMetalLayer() const { return m_MetalLayer; }
    
    id<MTLRenderCommandEncoder> BeginRenderPass(MTLRenderPassDescriptor* descriptor);
    void EndRenderPass();
    
    void BeginFrame();
    void EndFrame();
    void Present();
    
private:
    id<MTLDevice> m_Device;
    id<MTLCommandQueue> m_CommandQueue;
    id<MTLCommandBuffer> m_CurrentCommandBuffer;
    id<MTLRenderCommandEncoder> m_CurrentEncoder;
    CAMetalLayer* m_MetalLayer;
    id<CAMetalDrawable> m_CurrentDrawable;
    
    MTLRenderPassDescriptor* m_RenderPassDescriptor;
#endif
    
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
};

}

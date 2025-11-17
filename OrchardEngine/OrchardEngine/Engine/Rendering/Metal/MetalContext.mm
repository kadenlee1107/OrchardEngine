#include "MetalContext.hpp"
#include <iostream>

#ifdef __APPLE__

namespace Orchard {

MetalContext::MetalContext() {
}

MetalContext::~MetalContext() {
}

bool MetalContext::Initialize(const std::string& appName, uint32_t width, uint32_t height) {
    m_Width = width;
    m_Height = height;
    
    m_Device = MTLCreateSystemDefaultDevice();
    if (!m_Device) {
        std::cerr << "Metal is not supported on this device" << std::endl;
        return false;
    }
    
    m_CommandQueue = [m_Device newCommandQueue];
    if (!m_CommandQueue) {
        std::cerr << "Failed to create command queue" << std::endl;
        return false;
    }
    
    m_MetalLayer = [CAMetalLayer layer];
    m_MetalLayer.device = m_Device;
    m_MetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
    m_MetalLayer.framebufferOnly = YES;
    m_MetalLayer.drawableSize = CGSizeMake(width, height);
    
    m_RenderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    std::cout << "Metal context initialized on device: " 
              << [m_Device.name UTF8String] << std::endl;
    
    return true;
}

void MetalContext::Shutdown() {
    if (m_CurrentCommandBuffer) {
        [m_CurrentCommandBuffer release];
        m_CurrentCommandBuffer = nil;
    }
    
    if (m_CommandQueue) {
        [m_CommandQueue release];
        m_CommandQueue = nil;
    }
    
    if (m_Device) {
        [m_Device release];
        m_Device = nil;
    }
    
    std::cout << "Metal context shut down" << std::endl;
}

void MetalContext::BeginFrame() {
    m_CurrentDrawable = [m_MetalLayer nextDrawable];
    if (!m_CurrentDrawable) {
        return;
    }
    
    m_CurrentCommandBuffer = [m_CommandQueue commandBuffer];
    
    m_RenderPassDescriptor.colorAttachments[0].texture = m_CurrentDrawable.texture;
    m_RenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    m_RenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.2, 0.2, 0.2, 1.0);
    m_RenderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
}

void MetalContext::EndFrame() {
    if (m_CurrentEncoder) {
        [m_CurrentEncoder endEncoding];
        m_CurrentEncoder = nil;
    }
}

void MetalContext::Present() {
    if (m_CurrentDrawable && m_CurrentCommandBuffer) {
        [m_CurrentCommandBuffer presentDrawable:m_CurrentDrawable];
        [m_CurrentCommandBuffer commit];
        
        m_CurrentCommandBuffer = nil;
        m_CurrentDrawable = nil;
    }
}

id<MTLRenderCommandEncoder> MetalContext::BeginRenderPass(MTLRenderPassDescriptor* descriptor) {
    if (!m_CurrentCommandBuffer) return nil;
    
    m_CurrentEncoder = [m_CurrentCommandBuffer renderCommandEncoderWithDescriptor:descriptor];
    return m_CurrentEncoder;
}

void MetalContext::EndRenderPass() {
    if (m_CurrentEncoder) {
        [m_CurrentEncoder endEncoding];
        m_CurrentEncoder = nil;
    }
}

}

#endif

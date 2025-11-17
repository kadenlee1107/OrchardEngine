#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace Orchard {

class MetalContext;
struct RenderResource;

enum class RenderResourceType {
    Texture,
    Buffer
};

struct RenderResource {
    std::string name;
    RenderResourceType type;
    uint32_t width = 0;
    uint32_t height = 0;
    bool isExternal = false;
    
#ifdef __APPLE__
    id<MTLTexture> texture = nil;
    id<MTLBuffer> buffer = nil;
#endif
};

class RenderPass {
public:
    virtual ~RenderPass() = default;
    
    virtual void Setup() {}
    virtual void Execute(MetalContext* context) = 0;
    
    void AddInput(const std::string& name) { m_Inputs.push_back(name); }
    void AddOutput(const std::string& name) { m_Outputs.push_back(name); }
    
    const std::vector<std::string>& GetInputs() const { return m_Inputs; }
    const std::vector<std::string>& GetOutputs() const { return m_Outputs; }
    
protected:
    std::vector<std::string> m_Inputs;
    std::vector<std::string> m_Outputs;
};

class RenderGraph {
public:
    RenderGraph(MetalContext* context);
    ~RenderGraph();
    
    void AddPass(const std::string& name, std::unique_ptr<RenderPass> pass);
    void AddResource(const std::string& name, RenderResource resource);
    
    RenderResource* GetResource(const std::string& name);
    
    void Compile();
    void Execute();
    
    void Reset();
    
private:
    MetalContext* m_Context;
    
    std::vector<std::pair<std::string, std::unique_ptr<RenderPass>>> m_Passes;
    std::unordered_map<std::string, RenderResource> m_Resources;
    
    bool m_Compiled = false;
    std::vector<size_t> m_ExecutionOrder;
    
    void BuildExecutionOrder();
};

class GeometryPass : public RenderPass {
public:
    void Execute(MetalContext* context) override;
};

class LightingPass : public RenderPass {
public:
    void Execute(MetalContext* context) override;
};

class PostProcessPass : public RenderPass {
public:
    void Execute(MetalContext* context) override;
};

class ShadowPass : public RenderPass {
public:
    void Execute(MetalContext* context) override;
};

}
